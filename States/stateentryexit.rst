.. role:: fsmlang(code)
	:language: fsmlang

====================
State Entry and Exit
====================

The following shows how to declare entry or exit (inclusive or) functions for a state.

.. code-block:: fsmlang

    state IN_SESSION
        on entry start_session_timer
        on exit  stop_session_timer
        ;

As with some data translation functions, the names here are optional.  When not supplied, names will be concocted similar to
these:

.. code-block:: c

	void hsmCommunicator_onEntryTo_IN_SESSION(pHSM_COMMUNICATOR_DATA);
	void hsmCommunicator_onExitFrom_IN_SESSION(pHSM_COMMUNICATOR_DATA);

As it is, though, we have:

.. code-block:: c

	void hsmCommunicator_start_session(pHSM_COMMUNICATOR_DATA);
	void hsmCommunicator_stop_session(pHSM_COMMUNICATOR_DATA);

Before looking at the changes to the main FSM function, we'll add a machine transition function.  Here, the thought should be
"on transition," because it is a function that will be called on every state transition, rather than being a function called
in order to determine what transition should be taken (those are the subject of the next chapter).

To declare a machine transition function, we write:

.. code-block:: fsmlang
	:emphasize-lines: 2

	machine hsmCommunicator
	on transition track_transitions;
	native impl
	{
	    #define INIT_FSM_DATA {msg_none, { 0 }}
	}

Looking again in the private header we find:

.. code-block:: c

	void UFMN(track_transitions)(pHSM_COMMUNICATOR,HSM_COMMUNICATOR_STATE);

The function will get the machine's current state from the fsm pointer; it will have the new state passed in as the second
argument.  As with *all* user functions, the code *must not* alter anything in the machine structure other than data.

The machine transition function feature was created in response to a user request for additional debugging help.  Though there
may be other valid uses, one should assiduously avoid putting too much logic into the function.

With that, let's look at how this works out in the action array (``-tc``) main FSM function:

.. code-block:: c
	:emphasize-lines: 29 - 39

	void hsmCommunicatorFSM(pHSM_COMMUNICATOR pfsm, pHSM_COMMUNICATOR_EVENT event)
	{
	    HSM_COMMUNICATOR_EVENT_ENUM new_e;
	    HSM_COMMUNICATOR_EVENT_ENUM e = event->event;
	    HSM_COMMUNICATOR_STATE new_s;
	
	    translateEventData(&pfsm->data, event);
	
	    while (e != THIS(noEvent)) {
	
	#ifdef HSM_COMMUNICATOR_DEBUG
	        if (EVENT_IS_NOT_EXCLUDED_FROM_LOG(e))
	        {
	            DBG_PRINTF("event: %s; state: %s"
	                       ,HSM_COMMUNICATOR_EVENT_NAMES[e]
	                       ,HSM_COMMUNICATOR_STATE_NAMES[pfsm->state]
	                      );
	        }
	#endif
	
	        /* This is read-only data to facilitate error reporting in action functions */
	        pfsm->event = e;
	
	        if (e < hsmCommunicator_noEvent)
	        {
	
	            new_e = ((* (*pfsm->actionArray)[e][pfsm->state].action)(pfsm));
	
	            new_s = (*pfsm->actionArray)[e][pfsm->state].transition;
	
	
	            if (pfsm->state != new_s)
	            {
	                UFMN(track_transitions)(pfsm,new_s);
	                runAppropriateExitFunction(&pfsm->data, pfsm->state);
	                runAppropriateEntryFunction(&pfsm->data, new_s);
	                pfsm->state = new_s;
	
	            }
	
	            e = new_e;
	
	        }
	        else
	        {
	            e = findAndRunSubMachine(pfsm, e);
	        }
	
	    }
	
	}

The new state is captured as a local variable and is checked against the current state; if different, the machine transition
function is called, then any Exit and Entry functions.  The machine transition function is provided the machine pointer and
the new state; the Exit function gets a pointer to the machine's data and the current state; the Entry function gets a pointer
to the machine's data and the new state.  Once these functions complete, the new state is assigned into the machine structure.

For state and event function array generation (``-ts`` and ``-te``), this logic is in the state or event handlers,
respectively.

.. code-block:: c
	:caption: State Function
	:emphasize-lines: 18-25

	static HSM_COMMUNICATOR_EVENT_ENUM UNINITIALIZED_stateFn(pHSM_COMMUNICATOR pfsm,HSM_COMMUNICATOR_EVENT_ENUM e)
	{
		HSM_COMMUNICATOR_EVENT_ENUM retVal = THIS(noEvent);
		HSM_COMMUNICATOR_STATE new_s = hsmCommunicator_UNINITIALIZED;
	
		switch(e)
		{
		case THIS(INIT):
			retVal = UFMN(initialize)(pfsm);
			new_s = hsmCommunicator_IDLE;
			break;
		default:
			DBG_PRINTF("hsmCommunicator_noAction");
			break;
		}
	
	
		if (hsmCommunicator_UNINITIALIZED != new_s)
		{
			UFMN(track_transitions)(pfsm, new_s);
			runAppropriateExitFunction(&pfsm->data, hsmCommunicator_UNINITIALIZED);
			runAppropriateEntryFunction(&pfsm->data, new_s);
			pfsm->state = new_s;
	
		}
	
	
		return retVal;
	}

.. code-block:: c
	:caption: Event Function
	:emphasize-lines: 17-22

	static ACTION_RETURN_TYPE hsmCommunicator_handle_INIT(FSM_TYPE_PTR pfsm)
	{
		HSM_COMMUNICATOR_STATE s = pfsm->state;
		ACTION_RETURN_TYPE event = THIS(noEvent);
	
		switch (pfsm->state)
		{
			case STATE(UNINITIALIZED):
				event = UFMN(initialize)(pfsm);
				s = STATE(IDLE);
				break;
			default:
				DBG_PRINTF("hsmCommunicator_noAction");
				break;
		}
	
		if (s != pfsm->state)
		{
			UFMN(track_transitions)(pfsm,s);
			runAppropriateExitFunction(&pfsm->data, pfsm->state);
			runAppropriateEntryFunction(&pfsm->data, s);
		}
	
		pfsm->state = s;
	
	
		return event;
	}

Our implementation of these new user functions is vacuous.

.. code-block:: c

	void hsmCommunicator_store_message(pHSM_COMMUNICATOR_DATA pfsm_data, pHSM_COMMUNICATOR_MESSAGE_RECEIVED_DATA pedata)
	{
	    DBG_PRINTF("%s", __func__);
	
	    pfsm_data->current_msg = pedata->message;
	}
	
	void UFMN(track_transitions)(pHSM_COMMUNICATOR pfsm, HSM_COMMUNICATOR_STATE s)
	{
	    DBG_PRINTF("%s", __func__);
	    (void) pfsm;
	}
	
	void hsmCommunicator_start_session_timer(pHSM_COMMUNICATOR_DATA pfsm)
	{
	    DBG_PRINTF("%s", __func__);
	    (void) pfsm;
	}
	
	void hsmCommunicator_stop_session_timer(pHSM_COMMUNICATOR_DATA pfsm)
	{
	    DBG_PRINTF("%s", __func__);
	    (void) pfsm;
	}



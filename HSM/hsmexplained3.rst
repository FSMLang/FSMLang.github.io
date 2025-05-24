.. role:: fsmlang(code)
	:language: fsmlang

=============================
Sub-machine: establishSession
=============================

In this section, we look at adding actions and transitions for the sub-machine, establishSession.

----------
The Design
----------

*establishSession*, with events and states looks like this:

.. code-block:: fsmlang

	event MESSAGE_RECEIVED;

	machine establishSession
	{
	   event ESTABLISH_SESSION_REQUEST
	         , STEP0_RESPONSE
	         , STEP1_RESPONSE
	         , parent::MESSAGE_RECEIVED
	         ;

	   state IDLE
	         , AWAITING_RESPONSE
	         ;

	}

The *ESTABLISH_SESSION_REQUEST* event is expected only in the *IDLE* state.  When it is received, the machine sends the STEP0 message
and moves to the *AWAITING_RESPONSE* state to receive the STEP0_RESPONSE.

.. code-block:: fsmlang

	event ESTABLISH_SESSION_REQUEST;
	state IDLE, AWAITING_RESPONSE;

	/** Start the session establishment process. */
	action sendStep0Message[ESTABLISH_SESSION_REQUEST, IDLE]  transition AWAITING_RESPONSE;

	sendStep0Message returns noEvent;

Obligingly, while in the *AWAITING_RESPONSE* state the *MESSAGE_RECEIVED* event is shared from the parent machine.  *establishSession*
must parse that message to see whether it is a step0 or step1 response.

.. code-block:: fsmlang

	event MESSAGE_RECEIVED, STEP0_RESPONSE, STEP1_RESPONSE;
	state AWAITING_RESPONSE;

	/** Parse the incoming message */
	action parseMessage[MESSAGE_RECEIVED, AWAITING_RESPONSE];

	parseMessage returns STEP0_RESPONSE, STEP1_RESPONSE, noEvent;

When the *STEP0_RESPONSE* is found, the step1 message is sent, and the machine remains in the *AWAITING_RESPONSE* state to await the
next *MESSAGE_RECEIVED* event.  When received, that message will again be parsed.

States are used to disambiguate actions which must be different upon subsequent receptions of a single event.  It might be thought that
since the *MESSAGE_RECEIVED* event is received twice, that the machine should move to a different state to await the event.  This is not
necessary, though, since the action is the same in each case: the message is parsed.  Because the parsing yields a unique event for each
message type, there is no ambiguity as to the action which must be taken.

Any temptation to design with the expectation that the second *MESSAGE_RECEIVED* event will be the expected *STEP1_RESPONSE* event should
be effectively resisted.  This example is the "happy path" to session establishment; it could very well be in the real world that an
error indication may be received from the peer.  Remaining in the *AWAITING_RESPONSE* state until the received message is parsed makes for
the easist approache to designing for the "unhappy path."

.. code-block:: fsmlang

	event STEP0_RESPONSE;
	state AWAITING_RESPONSE;

	/** Continue session establisment */
	action sendStep1Message[STEP0_RESPONSE, AWAITING_RESPONSE];

	sendStep1Message returns noEvent;

When the *STEP1_RESPONSE* is found, the machine notifies the parent that the session is established and returns to the *IDLE* state.

.. code-block:: fsmlang

	event SESSION_ESTABLISHED;

	/** Notify parent that session is established. */
	action notifyParent[STEP1_RESPONSE, AWAITING_RESPONSE] transition IDLE;

	notifyParent     returns parent::SESSION_ESTABLISHED;

Remember that the parent machine begins to have messages send by the *sendMessage* sub-machine (the existance of which this machine is
ignorant) once the session is established.  Because of that, the parent will receive its *MESSAGE_RECEIVED* event when ACKs are sent
from the peer.  These events will be shared with this sub-machine, but will not be acted upon in the *IDLE* state.  This highlights an
important design principle, namely that sub-machines must return to a neutral state when their task is complete.  For sub-machines such
as this, which expect to be called more than once, that state is usually the initial state.  For machines which expect to act only once,
though, it may be required that they enter a *done* state, in which they react to no events.

A use-case for this last example might be a sub-machine which is acting as a co-routine to accomplish a long calculation.  Once the
calculation is finished, the machine must quiesce, even though the periodic event driving the machine may continue to be shared to it.

The full *establishSession* machine looks like this:

.. code-block:: fsmlang

	event MESSAGE_RECEIVED, SESSION_ESTABLISHED;

	machine establishSession
	{
	   event ESTABLISH_SESSION_REQUEST
	         , STEP0_RESPONSE
	         , STEP1_RESPONSE
	         , parent::MESSAGE_RECEIVED
	         ;

	   state IDLE
	         , AWAITING_RESPONSE
	         ;

	   /** Start the session establishment process. */
	   action sendStep0Message[ESTABLISH_SESSION_REQUEST, IDLE]  transition AWAITING_RESPONSE;

	   /** Parse the incoming message */
	   action parseMessage[MESSAGE_RECEIVED, AWAITING_RESPONSE];

	   /** Continue session establisment */
	   action sendStep1Message[STEP0_RESPONSE, AWAITING_RESPONSE];

	   /** Notify parent that session is established. */
	   action notifyParent[STEP1_RESPONSE, AWAITING_RESPONSE] transition IDLE;

	   sendStep0Message returns noEvent;
	   sendStep1Message returns noEvent;
	   parseMessage     returns STEP0_RESPONSE, STEP1_RESPONSE, noEvent;
	   notifyParent     returns parent::SESSION_ESTABLISHED;

	}

------------------
The Generated Code
------------------

As mentioned in a previous section, the command line, ``fsm -tc --generate-weak-fns=false hsmCommunicator.fsm``, produces the
following files:

Source files:

* hsmCommunicator.c
* **establishSession.c**
* sendMessage.c 

Header files:

* hsmCommunicator_priv.h
* hsmCommunicator.h
* hsmCommunicator_submach.h
* hsmCommunicator_events.h
* **establishSession_priv.h**
* sendMessage_priv.h 

In this section, we look at only the files related to this sub-machine, *i.e.* the ones beginning with *establishSession*.

Being a sub-machine, *establishSession* has no function to call it directly from the outside world, nor does it publish its own events.
Thus, neither *establishSession.h* nor *establishSession_events.h* are needed.

.. note::

	Should *establishSession* have also been a parent machine, having at least one sub-machine, it would have needed the
	*establishSession_submach.h* file.

Also because it is a sub-machine of a machine having actions which return events, the FSM function for *establishSession* must return
an event.  But, because it has no sub-machines of its own, its FSM structure does not have a sub-machine interface block array.  So, we 
find the following in *establishSession_priv.h*:

.. code-block:: c

	typedef HSM_COMMUNICATOR_EVENT (*ESTABLISH_SESSION_FSM)(FSM_TYPE_PTR,HSM_COMMUNICATOR_EVENT);

	struct _establishSession_struct_ {
	    ESTABLISH_SESSION_STATE            state;
	    HSM_COMMUNICATOR_EVENT             event;
	    ESTABLISH_SESSION_STATE_FN   const (*statesArray)[establishSession_numStates];
	    ESTABLISH_SESSION_FSM              fsm;
	};

As with the top-level, this header contains everything needed by the action functions file(s).  Convenience macros are re-defined as
necessary to fit the needs of this machine.

In the source file, *establishSession* must provide the sub-fsm interface block needed by its parent.

.. code-block:: c

	HSM_COMMUNICATOR_EVENT THIS(sub_machine_fn)(HSM_COMMUNICATOR_EVENT e)
	{
	    return establishSessionFSM(pestablishSession,e);
	}
	
	HSM_COMMUNICATOR_SUB_FSM_IF hsmCommunicator_establishSession_sub_fsm_if =
	{
	    .subFSM = THIS(sub_machine_fn)
	    , .first_event = THIS(firstEvent)
	    , .last_event = THIS(noEvent)
	};

The structure needed for each event shared from the parent must also be provided.

.. code-block:: c

	HSM_COMMUNICATOR_SHARED_EVENT_STR establishSession_share_hsmCommunicator_MESSAGE_RECEIVED_str = {
	    .event               = THIS(MESSAGE_RECEIVED)
	    , .psub_fsm_if         = &hsmCommunicator_establishSession_sub_fsm_if
	};

The FSM function implementation is closer to that of a flat FSM, needing only to pass on any event returned from an action function,
when that event does not belong to this machine.

.. code-block:: c

	HSM_COMMUNICATOR_EVENT establishSessionFSM(pESTABLISH_SESSION pfsm, HSM_COMMUNICATOR_EVENT event)
	{
	    HSM_COMMUNICATOR_EVENT e = event;
	
	    while ((e != THIS(noEvent))
	            && (e >= THIS(firstEvent))
	          )
	    {
	
	#ifdef HSM_COMMUNICATOR_ESTABLISH_SESSION_DEBUG
	        if ((EVENT_IS_NOT_EXCLUDED_FROM_LOG(e))
	                && (e >= THIS(firstEvent))
	                && (e < THIS(noEvent))
	           )
	        {
	            DBG_PRINTF("event: %s; state: %s"
	                       ,ESTABLISH_SESSION_EVENT_NAMES[e - THIS(firstEvent)]
	                       ,ESTABLISH_SESSION_STATE_NAMES[pfsm->state]
	                      );
	        }
	#endif
	
	        /* This is read-only data to facilitate error reporting in action functions */
	        pfsm->event = e;
	
	        if ((e >= THIS(firstEvent))
	                && (e < THIS(noEvent))
	           )
	        {
	            e = ((* (*pfsm->statesArray)[pfsm->state])(pfsm,e));
	        }
	
	    }
	
	    return e == THIS(noEvent) ? PARENT(noEvent) : e;
	}

Note that a local *noEvent* will stop the loop, but must be transformed to the parent's *noEvent* in order to be returned (otherwise, the
parent would hand it back!).

Also note that should *establishSession* itself had had sub-machines, the conditional would have had an ``else`` block and 
a ``findAndRunSubmachine`` function.

The check on the event range for both the ``while`` and ``if`` constructs serves to capture local events.  In our example, recall that
the *parseMessage* action will return the local *STEP0_RESPONSE* and *STEP1_RESPONSE* events; these will fall in the range allowed in the
loop and the conditional.

.. code-block:: c

	HSM_COMMUNICATOR_EVENT UFMN(parseMessage)(FSM_TYPE_PTR pfsm)
	{
	    DBG_PRINTF("%s", __func__);
	    (void) pfsm;
	    static bool first = true;
	
	    return first ? (first = false, THIS(STEP0_RESPONSE)) : THIS(STEP1_RESPONSE);
	}

(This simplistic action function serves only to illustrate our point, of course.)

Our *notifyParent* action illustrates meaningfull communication back to the parent.

.. code-block:: c

	HSM_COMMUNICATOR_EVENT UFMN(notifyParent)(FSM_TYPE_PTR pfsm)
	{
	    DBG_PRINTF("%s", __func__);
	    (void) pfsm;
	    return PARENT(SESSION_ESTABLISHED);
	}

The parent *SESSION_ESTABLISED* event falls outside of our range check (being below THIS(firstEvent) - see *hsmCommunicator_events.h*), and
will thus end the loop.  Since it is not equal to our local *noEvent*, it will not be transformed, but will be returned unchanged to
the parent FSM function.  That function will see it as one of its own events and will act on it as directed by its own state chart.


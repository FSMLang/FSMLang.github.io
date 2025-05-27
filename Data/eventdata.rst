.. role:: fsmlang(code)
	:language: fsmlang

==========
Event Data
==========

Attaching data to events is the natural way to introduce those data to the state machine.  Recall the definition of
the top-level machine's current message data: :ref:`peer_message`.  This definition can be easily attached to the
machines MESSAGE_RECEIVED event.

.. code-block:: fsmlang

    event MESSAGE_RECEIVED
          data
          {
             msg_e_t message;
          }
          ;

FSMLang creates a structure out of the declared data.  Then, a union is made to house the data from all events
declaring any.  Finally, the machine's *event* becomes a structure containing the event enumeration and the union.
This is done in the public header, since the events concerned originate in the outside world.

.. code-block:: c

	typedef struct _hsmCommunicator_MESSAGE_RECEIVED_data_ {
		msg_e_t message;
	} HSM_COMMUNICATOR_MESSAGE_RECEIVED_DATA, *pHSM_COMMUNICATOR_MESSAGE_RECEIVED_DATA;
	
	typedef union {
		HSM_COMMUNICATOR_MESSAGE_RECEIVED_DATA MESSAGE_RECEIVED_data;
	} HSM_COMMUNICATOR_EVENT_DATA, *pHSM_COMMUNICATOR_EVENT_DATA;
	
	typedef struct {
		HSM_COMMUNICATOR_EVENT_ENUM event;
		HSM_COMMUNICATOR_EVENT_DATA event_data;
	} HSM_COMMUNICATOR_EVENT, *pHSM_COMMUNICATOR_EVENT;

The public function made available to run the machine now takes a pointer to an event structure, rather than
the event enumeration.

.. code-block:: c

	void run_hsmCommunicator(pHSM_COMMUNICATOR_EVENT);

And, the ACTION_RETURN_TYPE macro is redefined to match the new name of the event enumeration.

.. code-block:: c

	#define ACTION_RETURN_TYPE HSM_COMMUNICATOR_EVENT_ENUM

FSMLang chose to rename the event enumeration to allow the event structure to bear the most natural name.  Since
handy convenience macros are provided, well written source code should not need to be adjusted to accommodate this.

The task of getting the event's data into the machine happens through a translator function called inside of the
main FSM function.

.. code-block:: c
	:emphasize-lines: 5

	void hsmCommunicatorFSM(pHSM_COMMUNICATOR pfsm, pHSM_COMMUNICATOR_EVENT event)
	{
	    HSM_COMMUNICATOR_EVENT_ENUM new_e;
	    HSM_COMMUNICATOR_EVENT_ENUM e = event->event;
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
	
	            pfsm->state = (*pfsm->actionArray)[e][pfsm->state].transition;
	
	            e = new_e;
	
	        }
	        else
	        {
	            e = findAndRunSubMachine(pfsm, e);
	        }
	
	    }
	
	}

``translateEvent`` is defined as:

.. code-block:: c

	static void translateEventData(pHSM_COMMUNICATOR_DATA pfsm_data, pHSM_COMMUNICATOR_EVENT pevent)
	{
	    switch(pevent->event)
	    {
	    case hsmCommunicator_MESSAGE_RECEIVED:
	        UFMN(translate_MESSAGE_RECEIVED_data)(pfsm_data, &pevent->event_data.MESSAGE_RECEIVED_data);
	        break;
	    default:
	        break;
	    }
	
	}

The name, ``translate_MESSAGE_RECEIVED_data`` was concted by FSMLang; to provide your own name, simply specify a
translator as part of the event's data declaration.

.. code-block:: fsmlang

    event MESSAGE_RECEIVED
          data
          translator store_message
          {
             msg_e_t message;
          }
          ;

Then, the specified name is used in the generated code:

.. code-block:: c

	static void translateEventData(pHSM_COMMUNICATOR_DATA pfsm_data, pHSM_COMMUNICATOR_EVENT pevent)
	{
		switch(pevent->event)
		{
		case hsmCommunicator_MESSAGE_RECEIVED:
			UFMN(store_message)(pfsm_data, &pevent->event_data.MESSAGE_RECEIVED_data);
			break;
		default:
			break;
		}
	
	}

The signature for the translator functions is unique to each event, but, rather than publishing typedefs for
each, the functions are declared in the private header.

.. code-block:: c

	void hsmCommunicator_store_message(pHSM_COMMUNICATOR_DATA,pHSM_COMMUNICATOR_MESSAGE_RECEIVED_DATA);

Finally, we noted in :ref:`data_translation_function`, that when parent and submachine have data, provision is made for
a data translation function to be called when events are shared down to the submachine.  It only remains, then, to declare
those functions in the submachines.  Unlike the top-level events, only the keyword :fsmlang:`translator` is insufficient;
FSMLang will not concoct a suitible name.

.. code-block:: fsmlang

    event parent::MESSAGE_RECEIVED data translator copy_current_message;


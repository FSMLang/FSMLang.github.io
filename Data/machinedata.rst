.. role:: fsmlang(code)
	:language: fsmlang

============
Machine Data
============

We will use the *hsmCommunicator* as our example, since there is more to see in the hierarchical case.  And, since the
top level machine behaves in this respect like a flat machine, there is nothing lost in the illustration.

Our peer messages are of three types: the step 0 response, the step 1 response, and, the ACK.  So, to differentiate, and
to carry all of the necessary content, a simple enumeration will suffice:

.. code-block:: c

	typedef enum
	{
	   msg_none
	   , msg_step0_response
	   , msg_step1_response
	   , msg_ack
	} msg_e_t;

The simple nature of our requirements allow us to put all of the material directly into the state machine
definition.

More complex data structures, of course, are better placed into headers and simply included into the machine.

Leaving out most of the machine structure for the moment, here is how we add the data and the mandatory data
initialization macro:

.. code-block:: fsmlang
	:name: peer_message
	:caption: Peer Message

	native
	{
	   typedef enum
	   {
	      msg_none
	      , msg_step0_response
	      , msg_step1_response
	      , msg_ack
	   } msg_e_t;
	}
	machine hsmCommunicator
	native impl
	{
	   #define INIT_FSM_DATA {msg_none, { 0 }}
	}
	{
	   data
	   {
	      msg_e_t current_msg;
	   }

	   /* other machine stuff here */

	}

That's it.

Sub-machines can have their own data.  Indeed, in our case, it is the sub-machines which parse the message, so each
having its own copy would work well (and illustrate how machines share data at run-time).  So, let's give each
sub-machine its own copy of the current message.  While we're at it, we'll move the "queue" into the data block as
well, and give it a structure.

.. code-block:: fsmlang

	
	native
	{
	   typedef enum
	   {
	      msg_none
	      , msg_step0_response
	      , msg_step1_response
	      , msg_ack
	   } msg_e_t;

	   typedef struct _queue_str_
	   {
	      unsigned queue_count;
	   } queue_str_t;
	}
	machine hsmCommunicator
	native impl
	{
	   #define INIT_FSM_DATA {msg_none, { 0 }}
	}
	{
	   data
	   {
	      msg_e_t current_msg;
	      queue_str_t queue;
	   }

	   machine establishSession
	   native impl
	   {
	      #define INIT_FSM_DATA {msg_none}
	   }
	   {

	      data
	      {
	         msg_e_t current_msg;
	      }

	   }


	   machine sendMessage
	   native impl
	   {
	      #define INIT_FSM_DATA {msg_none, NULL}
	   }
	   {

	      data
	      {
	         msg_e_t current_msg;
	         queue_str_t *pqueue;
	      }

	   }

	}

Looking at the generated code, we find the differences we might expect.

First, the Data structure is defined.  For machines with children, this happens in the submachine header; for
childless machines, it is in the private header.

Then, the data structure is placed in the FSM structure.  For all machines, this is in the private header.

*hsmCommunicator_submach.h*

.. code-block:: c

	typedef struct _hsmCommunicator_data_struct_ HSM_COMMUNICATOR_DATA, *pHSM_COMMUNICATOR_DATA;
	
	struct _hsmCommunicator_data_struct_ {
		msg_e_t current_msg;
		queue_str_t queue;
	};

*hsmCommunicator_priv.h*:

.. code-block:: c
	:emphasize-lines: 2

	struct _hsmCommunicator_struct_ {
		HSM_COMMUNICATOR_DATA           data;
		HSM_COMMUNICATOR_STATE          state;
		HSM_COMMUNICATOR_EVENT          event;
		HSM_COMMUNICATOR_ACTION_TRANS   const	(*actionArray)[THIS(numEvents)][hsmCommunicator_numStates];
		pHSM_COMMUNICATOR_SUB_FSM_IF     const	(*subMachineArray)[hsmCommunicator_numSubMachines];
		HSM_COMMUNICATOR_FSM           fsm;
	};

*establishSession_priv.h*:

.. code-block:: c
	:emphasize-lines: 8

	typedef struct _establishSession_data_struct_ ESTABLISH_SESSION_DATA, *pESTABLISH_SESSION_DATA;
	
	struct _establishSession_data_struct_ {
		msg_e_t current_msg;
	};

	struct _establishSession_struct_ {
		ESTABLISH_SESSION_DATA           data;
		ESTABLISH_SESSION_STATE          state;
		HSM_COMMUNICATOR_EVENT           event;
		ESTABLISH_SESSION_ACTION_TRANS   const	(*actionArray)[THIS(numEvents)][establishSession_numStates];
		ESTABLISH_SESSION_FSM           fsm;
	};

*sendMessage_priv.h*

.. code-block:: c
	:emphasize-lines: 9

	typedef struct _sendMessage_data_struct_ SEND_MESSAGE_DATA, *pSEND_MESSAGE_DATA;
	
	struct _sendMessage_data_struct_ {
		msg_e_t current_msg;
		queue_str_t * pqueue;
	};
	
	struct _sendMessage_struct_ {
		SEND_MESSAGE_DATA           data;
		SEND_MESSAGE_STATE          state;
		HSM_COMMUNICATOR_EVENT      event;
		SEND_MESSAGE_ACTION_TRANS   const	(*actionArray)[THIS(numEvents)][sendMessage_numStates];
		SEND_MESSAGE_FSM           fsm;
	};

To sum up, data declared for a machine are wrapped into a structure which becomes a field in the main FSM
structure.  At run-time, user functions access this data structure through the pointer they are given to the
machine structure.

How do submachines gain access?

Taking a look, again, into the submachine header, we find that the shared event structure has been altered.

.. code-block:: c
	:emphasize-lines: 4

	struct _hsmCommunicator_shared_event_str_
	{
		HSM_COMMUNICATOR_EVENT                event;
		HSM_COMMUNICATOR_DATA_TRANSLATION_FN  data_translation_fn;
		pHSM_COMMUNICATOR_SUB_FSM_IF          psub_fsm_if;
	};

Then, looking in the parent machine's source file, we find the sharing function has been modified to take advantage
of the ``data_translation_function`` field.

.. code-block:: c
	:emphasize-lines: 8,9
	:name: data_translation_function
	:caption: Submachine Data Translation Function

	HSM_COMMUNICATOR_EVENT hsmCommunicator_pass_shared_event(pHSM_COMMUNICATOR pfsm,pHSM_COMMUNICATOR_SHARED_EVENT_STR sharer_list[])
	{
		HSM_COMMUNICATOR_EVENT return_event = THIS(noEvent);
		for (pHSM_COMMUNICATOR_SHARED_EVENT_STR *pcurrent_sharer = sharer_list;
		     *pcurrent_sharer && return_event == THIS(noEvent);
		     pcurrent_sharer++)
		{
			if ((*pcurrent_sharer)->data_translation_fn)
				(*(*pcurrent_sharer)->data_translation_fn)(&pfsm->data);
			return_event = (*(*pcurrent_sharer)->psub_fsm_if->subFSM)((*pcurrent_sharer)->event);
		}
	
		return return_event;
	}

These lines are present because both the parent and the child have data, and the child shares an event with the
parent.

Note that the submachine event's data translation function is given a pointer only to the parent's data, not to
the entire machine structure.  Also note that the translation function is called once, before the event is passed
to the submachine's FSM function.  For any data to be visible to the submachine's user functions, the data must
somehow be made part of the submachine's own data structure.  This is why tranlators are only provided for when
both parent and child have data.

Any submachine data which derive from the parent machine's data must be initialized at run-time.  A common way to do this
is to provide an initialization event which is shared to submachines as required.  It is also common to then have an
*uninitialized* state which only reacts to this event and is exited once the event is received.

Our top-level machine, then adds:

.. code-block:: fsmlang

    /** System initialization */
    event INIT;

    /** The wakeup state. */
    state UNINITIALIZED;

    /** Initialize the machine */
    action initialize[INIT, UNINITIALIZED] transition IDLE;

*sendMessage* is the only sub-machine needing to be initialized, requiring, as it does, a pointer to the common queue.

.. code-block:: fsmlang

    event parent::INIT data translator init_data;

    transition [INIT, UNINITIALIZED] IDLE;

The initialization work takes place in the data translator function, so only a transition to the IDLE state is needed.  (Were
there any sub-machines here requiring initialization, however, an action would be needed in order to share the event to them.)

*sendMessages*'s init_data function looks like this:

.. code-block:: c

	void UFMN(init_data)(pHSM_COMMUNICATOR_DATA pfsm_data)
	{
	    DBG_PRINTF("%s", __func__);
	
		psendMessage->data.pqueue = &pfsm_data->queue;
	
	}

At the top-level, we've seen no way to bring data to the machine from the outside, save by exposing the
pointer to the machine, or by providing *setter* functions.  The proper event-driven state machine way to do this is through
the events themselves, which is the topic of our next section.


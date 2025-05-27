.. role:: fsmlang(code)
	:language: fsmlang

==========================
Top-Level: hsmCommunicator
==========================

In this section, we look at adding actions and transitions for the top-level, hsmCommunicator.  The FSMLang representation given here
is not syntactically correct, since the sub-machines are not included.  Their omission, however, makes the presentation easier to read.

----------
The Design
----------

The top-level machine with events and states looks like this:

.. code-block:: fsmlang

	machine hsmCommunicator
	{

	   event SEND_MESSAGE
	         , MESSAGE_RECEIVED
	         , TIMER_EXPIRED
	         ;

	   state IDLE
	         , ESTABLISHING_SESSION
	         , IN_SESSION
	         ;

	}

Normal operation would see the SEND_MESSAGE event appear first to the machine.  The machine must somehow save the
message, since the rule is that a session must first be established with the peer before a message may be sent.  The
top-level machine must also ask the establishSession machine to begin work.  So, we'll add the following:


.. code-block:: fsmlang

	   event SEND_MESSAGE, ESTABLISH_SESSION_REQUEST;
	   state IDLE;

	   machine establishSession;

	   /** This action also adds the message to the queue. */
	   action startSessionEstablishment[SEND_MESSAGE, IDLE] transition ESTABLISHING_SESSION;

	   startSessionEstablishment returns establishSession::ESTABLISH_SESSION_REQUEST;


Should other requests to send messages arrive before the session is established, the messages are simply queued.


.. code-block:: fsmlang

	   event SEND_MESSAGE;
	   state ESTABLISHING_SESSION;

	   /** This action also adds the message to the queue. */
	   action queueMessage[SEND_MESSAGE, ESTABLISHING_SESSION];

	   queueMessage returns noEvent;

When the session is established, the establishSession sub-machine will return our top-level SESSION_ESTABLISHED
event.  At this point, we complete the starting of the session by passing the SEND_MESSAGE event to the
sendMessage sub-machine.

.. code-block:: fsmlang

	event SESSION_ESTABLISHED, SEND_MESSAGE;
	state ESTABLISHING_SESSION;

	action completeSessionStart[SESSION_ESTABLISHED, ESTABLISHING_SESSION] transition IN_SESSION;

	completeSessionStart returns noEvent;

While IN_SESSION, the machine queues incomming messages and passes the ESTABLISHED_SESSION event to the sendMessage
machine.

.. code-block:: fsmlang

	event SEND_MESSAGE;
	state IN_SESSION;

	action requestMessageTransmission[SEND_MESSAGE, IN_SESSION];

	requestMessageTransmission returns noEvent;

Also while IN_SESSION, it may be possible for the machine to receive the TIMER_EXPIRED event.  This event requires no action,
but simply a transition to the IDLE state.

.. code-block:: fsmlang

	event TIMER_EXPIRED;
	state IN_SESSION, IDLE;

	transition [TIMER_EXPIRED, IN_SESSION] IDLE;

The full top-level machine looks like this:

.. code-block:: fsmlang

	machine hsmCommunicator
	{

	   event SEND_MESSAGE
	         , MESSAGE_RECEIVED
	         , TIMER_EXPIRED
	         ;

	   state IDLE
	         , ESTABLISHING_SESSION
	         , IN_SESSION
	         ;

	   machine establishSession
	   {
	      event START_SESSION_ESTABLISHMENT;
	   }

	   /** This action also adds the message to the queue. */
	   action startSessionEstablishment[SEND_MESSAGE, IDLE] transition ESTABLISHING_SESSION;

	   action queueMessage[SEND_MESSAGE, ESTABLISHING_SESSION];

	   action completeSessionStart[SESSION_ESTABLISHED, ESTABLISHING_SESSION] transition IN_SESSION;

	   action requestMessageTransmission[SEND_MESSAGE, IN_SESSION];

	   transition [TIMER_EXPIRED, IN_SESSION] IDLE;

	   startSessionEstablishment  returns establishSession::START_SESSION_ESTABLISHMENT;
	   queueMessage               returns noEvent;
	   completeSessionStart       returns noEvent;
	   requestMessageTransmission returns noEvent;

	}

------------------
The Generated Code
------------------

The command line, ``fsm -tc --generate-weak-fns=false hsmCommunicator.fsm``, produces the following files:

Source files:

* **hsmCommunicator.c**
* establishSession.c
* sendMessage.c 

Header files:

* **hsmCommunicator_priv.h**
* **hsmCommunicator.h**
* **hsmCommunicator_submach.h**
* **hsmCommunicator_events.h**
* establishSession_priv.h
* sendMessage_priv.h 

In this section, we look only at the top-level files, *i.e.* the ones beginning with *hsmCommunicator*.

The top-level header is hsmCommunicator.h.  It is the only file that should be included by code which uses
the machine.

The header, hsmCommunicator_priv.h, is for the action function files.  It contains all the definitions they need,
and itself includes the top-level header.

The source code for the top-level machine is in hsmCommunicator.c.

As with a flat machine, the top-level header file provides convenience macros and a function through
which the state machine may be run.  Though macros are provided to directly inject sub-machine events,
they should only be used if unavoidable, since this exposes the internals of the state machine, complicating
any machine re-design.

.. code-block:: c

	#ifndef NO_CONVENIENCE_MACROS
	#undef UFMN
	#define UFMN(A) hsmCommunicator_##A
	#undef THIS
	#define THIS(A) hsmCommunicator_##A
	#endif
	#undef STATE
	#define STATE(A) hsmCommunicator_##A
	#undef HSM_COMMUNICATOR
	#define HSM_COMMUNICATOR(A) hsmCommunicator_##A
	#undef ESTABLISH_SESSION
	#define ESTABLISH_SESSION(A) hsmCommunicator_establishSession_##A
	#undef SEND_MESSAGE
	#define SEND_MESSAGE(A) hsmCommunicator_sendMessage_##A
	
	#undef ACTION_RETURN_TYPE
	#define ACTION_RETURN_TYPE HSM_COMMUNICATOR_EVENT
	
	void run_hsmCommunicator(HSM_COMMUNICATOR_EVENT);
	
	typedef struct _hsmCommunicator_struct_ *pHSM_COMMUNICATOR;
	extern pHSM_COMMUNICATOR phsmCommunicator;

This file, as with flat machines, includes the header containing the events enumeration.  This enumeration is our first indication that we
are dealing with a hierarchical machine.

.. code-block:: c

	typedef enum HSM_COMMUNICATOR_EVENT {
	    hsmCommunicator_SEND_MESSAGE
	    , hsmCommunicator_SESSION_ESTABLISHED
	    , hsmCommunicator_SESSION_TIMEOUT
	    , hsmCommunicator_MESSAGE_RECEIVED
	    , hsmCommunicator_noEvent
	    , hsmCommunicator_numEvents
	    , hsmCommunicator_establishSession_firstEvent
	    , hsmCommunicator_establishSession_ESTABLISH_SESSION_REQUEST = hsmCommunicator_establishSession_firstEvent
	    , hsmCommunicator_establishSession_STEP0_RESPONSE
	    , hsmCommunicator_establishSession_STEP1_RESPONSE
	    , hsmCommunicator_establishSession_MESSAGE_RECEIVED
	    , hsmCommunicator_establishSession_noEvent
	    , hsmCommunicator_sendMessage_firstEvent
	    , hsmCommunicator_sendMessage_SEND_MESSAGE = hsmCommunicator_sendMessage_firstEvent
	    , hsmCommunicator_sendMessage_MESSAGE_RECEIVED
	    , hsmCommunicator_sendMessage_ACK
	    , hsmCommunicator_sendMessage_noEvent
	    , hsmCommunicator_numAllEvents
	}  HSM_COMMUNICATOR_EVENT;

All events for all machines appear in this one enumeration.  The enumeration has some structure, with the special "..._firstEvent" entries,
to allow easy descrimination, when used with "...noEvent" of which machine should handle each event.

Another important difference between flat and hierarchical FSMs is seen in hsmCommunicator_priv.h.

.. code-block:: c

	#include "hsmCommunicator_submach.h"
	
	struct _hsmCommunicator_struct_ {
		HSM_COMMUNICATOR_STATE              state;
		HSM_COMMUNICATOR_EVENT              event;
		HSM_COMMUNICATOR_STATE_FN      const (*statesArray)[hsmCommunicator_numStates];
		pHSM_COMMUNICATOR_SUB_FSM_IF   const (*subMachineArray)[hsmCommunicator_numSubMachines];
		HSM_COMMUNICATOR_FSM                fsm;
	};

The file inludes the hsmCommunicator_submach.h header which contains the material necessary for a parent
machine to interact with its sub-machines.  One of these items is the HSM_COMMUNICATOR_SUB_FSM_IF block which
is examined below.

The other difference from a flat machine is the presence of a pointer to an array of these blocks; the array
having one entry for each sub machine.

Looking into hsmCommunicator_submach.h, we find an enumeration of the sub-machines immediately before the sub-machine interface
block:

.. code-block:: c

	typedef enum {
		 establishSession_e
		, hsmCommunicator_firstSubMachine = establishSession_e
		,  sendMessage_e
		, hsmCommunicator_numSubMachines
	} HSM_COMMUNICATOR_SUB_MACHINES;
	
	typedef HSM_COMMUNICATOR_EVENT (*HSM_COMMUNICATOR_SUB_MACHINE_FN)(HSM_COMMUNICATOR_EVENT);
	typedef struct _hsmCommunicator_sub_fsm_if_ HSM_COMMUNICATOR_SUB_FSM_IF, *pHSM_COMMUNICATOR_SUB_FSM_IF;
	struct _hsmCommunicator_sub_fsm_if_
	{
		HSM_COMMUNICATOR_EVENT           first_event;
		HSM_COMMUNICATOR_EVENT           last_event;
		HSM_COMMUNICATOR_SUB_MACHINE_FN  subFSM;
	};

The typedef for the sub-machine function shows that sub-machines, unlike top-level and flat machines, return events for machines in which
actions return events (as the present example).  This is how sub-machines are able to act as "sub-routines."

.. admonition:: Sub-machines as Sub-routines
	:class: 'sidebar'

	It is expected that a sub-machine must process a sequence of events in order to accomplish its task.  As each intermediate event is handled,
	the sub-machine returns :fsmlang:`parent::noEvent` to its parent, to indicate that it is still working.  However, when the sub-machine is
	done, either through some happy path, or otherwise, the sub-machine will return some other event, to indicate the completion status.

Moving to the source file, we see how this structure is used by the top-level FSM function to select and execute appropriate sub-machines.

.. code-block:: c

	void hsmCommunicatorFSM(pHSM_COMMUNICATOR pfsm, HSM_COMMUNICATOR_EVENT event)
	{
	   HSM_COMMUNICATOR_EVENT e = event;
	
	   while (e != hsmCommunicator_noEvent) {
	
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
	         e = ((* (*pfsm->statesArray)[pfsm->state])(pfsm,e));
	      }
	      else
	      {
	         e = findAndRunSubMachine(pfsm, e);
	      }
	
	   }
	}

When the event being handled is less than the the machine's own *noEvent*, the machine goes to its own state function array, as would
a flat machine.  However, for events above that the machine finds, then runs, the approprate sub machine:

.. code-block:: c

	static HSM_COMMUNICATOR_EVENT findAndRunSubMachine(pHSM_COMMUNICATOR pfsm, HSM_COMMUNICATOR_EVENT e)
	{
	   for (HSM_COMMUNICATOR_SUB_MACHINES machineIterator = THIS(firstSubMachine);
	        machineIterator < THIS(numSubMachines);
	        machineIterator++
	       )
	   {
	      if (
	            ((*pfsm->subMachineArray)[machineIterator]->first_event <= e)
	            && ((*pfsm->subMachineArray)[machineIterator]->last_event > e)
	         )
	      {
	         return ((*(*pfsm->subMachineArray)[machineIterator]->subFSM)(e));
	      }
	   }
	
	   return THIS(noEvent);
	
	}

This function loops through the array of sub-machine blocks, looking for the one whose event range encompasses the event being handled.
Upon finding the right block, the function pointer is used to invoke the sub-machine's FSM function, passing the event.  The event
returned by that function is returned from *findAndRunSubMachine*.

The calling FSM function then loops, looking at the event returned from the sub-machine.  When that event is the top-level's own *noEvent*,
the FSM function exits.  Otherwise, it looks again for an action or machine to handle the new event.

As can be seen, a sub-machine can return an event that will be handled only by another sub-machine.  However, doing so can quickly result
in the kind of inter-weaving that FSMs are intended to prevent.  Best is to have sub-machines only return events belonging to their parent.
It would then be up to the parent to decide when that event should spark the running of another sub-machine.

This is illustrated by the interaction between the top-level machine and the *establishSession* sub-machine.  When the top-level machine
calls *startSessionEstablishment* to handle the *SEND_MESSAGE* event from the *IDLE* state, the function adds the message to the queue
and returns *establishSession::BEGIN_SESSION_ESTABLISHMENT*.  The top-level FSM function loops and quickly finds the *establishSession*
as the machine which should handle the event.  The receipt of two messages is required for the *establishSession* machine to complete its
work; after processing the first message, it returns *parent::noEvent*, causing the top level machine to also simply exit.  Upon the
receipt of the second message, however, *establishSession* returns *parent::SESSION_ESTABLISHED*.  The top-level machine processes this
event by asking the *sendMessage* machine to begin sending messages from the queue.

As stated, *startSessionEstablishment* returns a sub-machine event in order to get that machine to do some work:

.. code-block:: c

	HSM_COMMUNICATOR_EVENT  UFMN(startSessionEstablishment)(FSM_TYPE_PTR pfsm)
	{
	    DBG_PRINTF("%s", __func__);
	    (void) pfsm;
	
	    queue_count++;
	
	    return ESTABLISH_SESSION(ESTABLISH_SESSION_REQUEST);
	}

This is a good technique to use to start an idle machine.

For shared events, however, a different technique is used, as seen in *passMessageReceived*:

.. code-block:: c

	HSM_COMMUNICATOR_EVENT  UFMN(passMessageReceived)(FSM_TYPE_PTR pfsm)
	{
	    DBG_PRINTF("%s", __func__);
	    (void) pfsm;
	    return hsmCommunicator_pass_shared_event(sharing_hsmCommunicator_MESSAGE_RECEIVED);
	}

The *pass_shared_event* function is defined in each parent machine's source file:

.. code-block:: c

	HSM_COMMUNICATOR_EVENT hsmCommunicator_pass_shared_event(pHSM_COMMUNICATOR_SHARED_EVENT_STR sharer_list[])
	{
	    HSM_COMMUNICATOR_EVENT return_event = THIS(noEvent);
	    for (pHSM_COMMUNICATOR_SHARED_EVENT_STR *pcurrent_sharer = sharer_list;
	            *pcurrent_sharer && return_event == THIS(noEvent);
	            pcurrent_sharer++)
	    {
	        return_event = (*(*pcurrent_sharer)->psub_fsm_if->subFSM)((*pcurrent_sharer)->event);
	    }
	
	    return return_event;
	}

Sharer lists are constructed for each parent event shared down to any sub-machines.

.. code-block:: c

	pHSM_COMMUNICATOR_SHARED_EVENT_STR sharing_hsmCommunicator_SEND_MESSAGE[] =
	{
	    &sendMessage_share_hsmCommunicator_SEND_MESSAGE_str
	    , NULL
	};
	
	pHSM_COMMUNICATOR_SHARED_EVENT_STR sharing_hsmCommunicator_MESSAGE_RECEIVED[] =
	{
	    &establishSession_share_hsmCommunicator_MESSAGE_RECEIVED_str
	    , &sendMessage_share_hsmCommunicator_MESSAGE_RECEIVED_str
	    , NULL
	};

As seen, the *MESSAGE_RECEIVED* event is always shared to both of the sub-machines.  It is up to those sub-machines whether or not they
act on that event in their current state.  Though this can (and does) result in making a call to a sub-machine with an even that it
will simply ignore, to do otherwise would bring the sub-machine's state chart into the parent, reducing the value of the hierarchical
concept.


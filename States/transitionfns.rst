.. role:: fsmlang(code)
	:language: fsmlang

==============================
Transition Functions or Guards
==============================

All examples shown thus far have declared transitions in terms of the state to which the machine will move.  It is possible,
however to specify a function to be called, which will return the appropriate transition state.  This is dangerous,
because logic which should be in the main body of the machine design can be hidden away in transition functions.  But, there
are legitimate uses.  For example, consider the following:

.. code-block:: fsmlang

	event e1, e2;
	state s1, s2;

	action do_nothing[e1, s1];
	transition [e2, s1] s2;

	do_nothing returns e2, noEvent;

In this snippet we see that the machine should remain in state s1 when do_nothing returns noEvent, but should transition to s2 when
do_nothing instead returns e2.  *do_something* is acting as a predicate determining the end state of the machine, based on some
criteria.

The same effect can be accomplished, perhaps with better clarity, with this construction:

.. code-block:: fsmlang

	event e1;
	state s1, s2;

	transition [e1, s1] where_to_go;

	where_to_go  returns s2, noTransition
	
In this case, the criteria for the state change are coded in *where_to_go*.

.. admonition:: Terminology
	:class: sidebar

	In UML parlance, *where_to_go* is a *guard*.

	FSMLang offers :fsmlang:`guard` as an optional keyword usable when the transition is via a function.


In both constructions, when e1 occurs in s1, nothing is done and the machine ends up in either s1 or s2.

The following scenario can be used to illustrate this in our HSM Communicator:  Consider an "unhappy" path in the session setup
wherein the peer rejects the session.  The first steps toward handling this scenario are to add another element to our msg_e_t
enumeration, and to give the top-level machine a SESSION_REJECTED event, the receipt of which causes the machine to clear the queue
and return to the IDLE state (other strategies are imaginable, of course).  For this, we need:

.. code-block:: fsmlang
	:emphasize-lines: 13, 24, 25, 29, 30
	:caption: hsmCommunicator Top-Level

	native
	{
	#ifndef DBG_PRINTF
	#include <stdio.h>
	#define DBG_PRINTF(...) printf(__VA_ARGS__); printf("\n");
	#endif
	
	typedef enum
	{
		msg_none
		, msg_step0_response
		, msg_step1_response
		, msg_session_rejected
		, msg_ack
	} msg_e_t;
	
	typedef struct _queue_str_
	{
		unsigned queue_count;
	} queue_str_t;
	
	}

	    /** Our peer has rejected our attempt to establish a session. */
	    event SESSION_REJECTED;
	
	    state IDLE, ESTABLISHING_SESSION;

	    /** Clear the queue and return to the IDLE state */
	    action clearQueue[SESSION_REJECTED, ESTABLISHING_SESSION] transition IDLE;

What should be done in *establishSession*?

One approach woudld be to give *establishSession* its own REJECTION_RESPONSE event (returned by parseMessage) and have an action
function which would notifySessionRejected upon its receipt:

.. code-block:: fsmlang
    :caption: establishSession - with new action

    /* parent events */
    event MESSAGE_RECEIVED, SESSION_REJECTED;

    event ESTABLISH_SESSION_REQUEST, STEP0_RESPONSE, STEP1_RESPONSE, REJECTION_RESPONSE;
    event parent::MESSAGE_RECEIVED data translator copy_current_message;

    state IDLE, AWAITING_RESPONSE;

    action notifySessionRejected[REJECTION_RESPONSE, AWAITING_RESPONSE] transition IDLE;

    /** Parse the incoming message */
    action parseMessage[MESSAGE_RECEIVED, AWAITING_RESPONSE];

    parseMessage returns STEP0_RESPONSE, STEP1_RESPONSE, REJECTION_RESPONSE, noEvent;

    notifySessionRejected returns parent::SESSION_REJECTED;

Or, using a transition function, we could write:

.. code-block:: fsmlang
    :caption: establishSession - with transition function

    /* parent events */
    event MESSAGE_RECEIVED, SESSION_REJECTED;

    event ESTABLISH_SESSION_REQUEST, STEP0_RESPONSE, STEP1_RESPONSE;

    state IDLE, AWAITING_RESPONSE;

    /** Parse the incoming message */
    action parseMessage[MESSAGE_RECEIVED, AWAITING_RESPONSE] guard handle_rejection;

    parseMessage returns STEP0_RESPONSE, STEP1_RESPONSE, parent::SESSION_REJECTED, noEvent;

    handle_rejection returns IDLE, noTransition;

In this solution, parseMessage returns the parent SESSION_REJECTED event itself, and handle_rejection decides whether the machine
will remain in the AWAITING_RESPONSE state or transition to the IDLE state.  In this, we save an action function, but gain a
transition, or, *guard*, function.

But, now that we have the guard function, one further refinment can be made.  The action, *notifyParent* is no longer needed;
*parseMessage* can return parent::SESSION_ESTABLISHED just as well as it can return parent::SESSION_REJECTED. And, if we're going
to do that, then *handle_rejection* is no longer a good name for the guard function, so we'll go with the blander
*decide_parse_transition*.

So, we have this:

.. code-block:: fsmlang
    :caption: establishSession - refined

    /* parent events */
    event MESSAGE_RECEIVED, SESSION_ESTABLISHED, SESSION_REJECTED;

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

    event ESTABLISH_SESSION_REQUEST, STEP0_RESPONSE;
    event parent::MESSAGE_RECEIVED data translator copy_current_message;

    state IDLE, AWAITING_RESPONSE;

    /** Start the session establishment process. */
    action sendStep0Message[ESTABLISH_SESSION_REQUEST, IDLE]  transition AWAITING_RESPONSE;

    /** Continue session establisment */
    action sendStep1Message[STEP0_RESPONSE, AWAITING_RESPONSE];

    /** Parse the incoming message */
    action parseMessage[MESSAGE_RECEIVED, AWAITING_RESPONSE] guard decide_parse_transition;

    /* these lines are informational; they affect the html output, but do not affect any C code generated. */
    sendStep0Message returns noEvent;
        
    sendStep1Message returns noEvent;
        
    parseMessage returns STEP0_RESPONSE, parent::SESSION_ESTABLISHED, parent::SESSION_REJECTED, noEvent;

    decide_parse_transition returns IDLE, noTransition;

    }

Here is the implementation of the new parseMessage action:

.. code-block:: c

	ACTION_RETURN_TYPE UFMN(parseMessage)(FSM_TYPE_PTR pfsm)
	{
	    DBG_PRINTF("%s", __func__);
	    ACTION_RETURN_TYPE new_event = THIS(noEvent);
	
	    switch (pfsm->data.current_msg)
	    {
	    case msg_step0_response:
	        new_event = THIS(STEP0_RESPONSE);
	        break;
	    case msg_step1_response:
	        new_event = PARENT(SESSION_ESTABLISHED);
	        break;
	    case msg_session_rejected:
	        new_event = PARENT(SESSION_REJECTED);
	        break;
	    default:
	        break;
	    }
	
	    return new_event;
	}

The new *decide_parse_transition* looks like this:

.. code-block:: c

	ESTABLISH_SESSION_STATE UFMN(decide_parse_transition)(pESTABLISH_SESSION pfsm, HSM_COMMUNICATOR_EVENT_ENUM e)
	{
	    DBG_PRINTF("%s", __func__);
	
	    (void) e;
	    ESTABLISH_SESSION_STATE new_state = pfsm->state;
	
	    switch (pfsm->data.current_msg)
	    {
	    default:
	    case msg_step0_response:
	        break;
	    case msg_step1_response:
	    case msg_session_rejected:
	        new_state = STATE(IDLE);
	        break;
	    }
	
	    return new_state;
	}

This causes the machine to move to the IDLE state when the session is either established or rejected, but remain in the
AWAITING_RESPONSE state when the step0 response is received.  The key thing to notice here is that this function takes no
actions; it merely chooses a path.  This is a key to good state machine design using guard functions.

At the top-level, we have merely to add the *clearQueue* action:

.. code-block:: c

	ACTION_RETURN_TYPE  UFMN(clearQueue)(FSM_TYPE_PTR pfsm)
	{
	    DBG_PRINTF("%s", __func__);
	
	    pfsm->data.queue.queue_count = 0;
	
	    return THIS(noEvent);
	}


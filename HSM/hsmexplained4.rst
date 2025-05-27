.. role:: fsmlang(code)
	:language: fsmlang

========================
Sub-machine: sendMessage
========================

In this section, we look at adding actions and transitions for the sub-machine, *sendMessage*.

----------
The Design
----------

*sendMessage*, with events and states looks like this:

.. code-block:: fsmlang

	event SEND_MESSAGE, MESSAGE_RECEIVED;

	machine sendMessage
	{

	   event parent::SEND_MESSAGE
	         , parent::MESSAGE_RECEIVED
	         , ACK
	         ;

	   state IDLE
	         , AWAITING_ACK
	         ;

	}

This machine will be in its *IDLE* state when it receives the first *SEND_MESSAGE* event from the parent.  Thereafter, the machine
may be in the *AWAITING_ACK* state.  When recieved in the former state, the machine will pull the top message from the queue, send it, and
transition to the *AWAITING_ACK* state.  In that state, any *SEND_MESSAGE* event received will simply be ignored.

.. code-block:: fsmlang

	event SEND_MESSAGE;
	state IDLE, AWAITING_ACK;

	/** Dequeue and transmit message to the peer. */
	action sendMessage[SEND_MESSAGE,IDLE] transition AWAITING_ACK;

	sendMessage returns noEvent;
        

When the *MESSAGE_RECEIVED* event is shared to this machine in the *AWAITING_ACK* state, it must parse it.  Since the event is not
expected while *IDLE*, it will be ignored in that state.

.. code-block:: fsmlang

	event MESSAGE_RECEIVED;
	state AWAITING_ACK;

	action parseMessage[MESSAGE_RECEIVED, AWAITING_ACK];

	parseMessage returns ACK, noEvent;

When the *ACK* is received in the *AWAITING_ACK* state, the queue is checked for more messages, and the machine transitions to the *IDLE*
state.  This ensures that the machine will handle the *SEND_MESSAGE* event returned by the checkQueue action when the queue is not
empty. The transition cannot wait, since if *checkQueue* returns *noEvent*, the FSM function loop will exit, thus giving no futher
opportunity for the transition to be made.

.. code-block:: fsmlang

	/** Check queue for messages; if found return SEND_MESSAGE; otherwise, return noEvent. */
	action checkQueue[ACK,AWAITING_ACK] transition IDLE;

	checkQueue returns SEND_MESSAGE, noEvent;

The full sendMessage machine looks like this:

.. code-block:: fsmlang

	machine sendMessage
	{
	   event parent::SEND_MESSAGE
	         , parent::MESSAGE_RECEIVED
	         , ACK
	         ;

	   state IDLE
	         , AWAITING_ACK
	         ;

	   /** Dequeue and transmit message to the peer. */
	   action sendMessage[SEND_MESSAGE,IDLE] transition AWAITING_ACK;

	   /** Check queue for messages; if found return SEND_MESSAGE; otherwise, return noEvent. */
	   action checkQueue[ACK,AWAITING_ACK] transition IDLE;

	   action parseMessage[MESSAGE_RECEIVED, AWAITING_ACK];

	   sendMessage  returns noEvent;
	   checkQueue   returns SEND_MESSAGE, noEvent;
	   parseMessage returns ACK, noEvent;

	}

------------------
The Generated Code
------------------

As mentioned in a previous section, the command line, ``fsm -tc --generate-weak-fns=false hsmCommunicator.fsm``, produces the
following files:

Source files:

* hsmCommunicator.c
* establishSession.c
* **sendMessage.c**

Header files:

* hsmCommunicator_priv.h
* hsmCommunicator.h
* hsmCommunicator_submach.h
* hsmCommunicator_events.h
* establishSession_priv.h
* **sendMessage_priv.h** 

It would be tedious to examine the *sendMessage* files, since they mimic those generated for the *establishSession* machine.

.. role:: fsmlang(code)
	:language: fsmlang

.. _the-hsm-communicator:

====================
The HSM Communicator
====================

As stated in the hsmCommunicator.fsm document, the design goal is a machine that is not only able to send
messages to a peer using a stop and wait protocol, in which only one message may be outstanding, but which
also implements the additional constraint that no message may be sent until a session has been
established.  Session establishment requires two messages to be sent and two to be received.  Sessions expire after
some amount of inactivity.

The events such a machine would experience might be conceived in this way:

* A request to send a message.  This is an event from a local client program.
* The sending of the first session establishment message to the peer communicator.
* The receipt of the first session establishment message from the peer communicator.
* The receipt of the second session establishment message from the peer communicator.
* The receipt of the message acknowledgement from the peer communicator.
* The receipt of a timer message from the local OS indicating that the session timeout has elapsed.

Of course, being a simple example, a flat state machine may be conceived which would handle these events.
This machine would need only three states:

#. Nothing is happening.
#. Session establishment messages are outstanding.
#. A client message is outstanding.

The flat machine might start to look like this:

.. code-block:: fsmlang

	machine complexCommunicator
	{

	   event SEND_MESSAGE
	         , MESSAGE_RECEIVED
	         , STEP0_RESPONSE
	         , STEP1_RESPONSE
	         , ACK
	         , TIMER_EXPIRED
	         ;

	   state IDLE
	         , ESTABLISHING_SESSION
	         , IN_SESSION
	         , AWAITING_ACK
	         ;

	}


Note the addition of the MESSAGE_RECIEVED event.  In our scenario, the machine is responsible for parsing
the peer messages.

Since our goal is to illustrate a hierarchical machine, however, we'll leave the flat machine behind.

One conception would be to have a top-level machine with two sub-machines.  One sub-machine will handle the
session establishment chore, while the other will send the client messages.

.. note::
	FSMLang interchanges the use of
	terms parent/child with parent/sub-machine.

	The term *top-level* always refers to the machine of which all others
	are children (or grandchildren).

--------
Skeleton
--------

The skeleton looks like this:

.. code-block:: fsmlang

	machine hsmCommunicator
	{

	   machine establishSession
	   {

	   }

	   machine sendMessage
	   {

	   }

	}

------
Events
------

The next step is to allocate the events:

.. code-block:: fsmlang

	machine hsmCommunicator
	{

	   event SEND_MESSAGE
	         , MESSAGE_RECEIVED
	         , SESSION_ESTABLISHED
	         , TIMER_EXPIRED
	         ;

	   machine establishSession
	   {
	      event ESTABLISH_SESSION
	            , STEP0_RESPONSE
	            , STEP1_RESPONSE
	            , parent::MESSAGE_RECEIVED
	            ;

	   }

	   machine sendMessage
	   {

	      event parent::SEND_MESSAGE
	            , parent::MESSAGE_RECEIVED
	            , ACK
	            ;
	   }

	}

The alert reader notes the addition of several events. Namely, :fsmlang:`event ESTABLISH_SESSION` 
is added in the sub-machine 
establishSession; and :fsmlang:`event SESSION_ESTABLISHED` is added in the top-level machine. In addition,
the keyword and namespace indicator :fsmlang:`parent::` are used to add this event to the sub-machines. These
will be used to show the two ways that HSMs can communicate within the hierarchy.  The top-level machine will
send ESTABLISH_SESSION to the sub-machine establishSession only when a new session is needed.  The sub-machine,
establishSession will return SESSION_ESTABLISHED only upon session establishment. But,
:fsmlang:`event MESSAGE_RECEIVED`, on the other hand, will be shared down to the sub-machines whenever it occurs.

------
States
------

To finish our design, we add the states needed to track progress through the event stream.

All of the machines will start in the IDLE state.

When the top-level machine receives the SEND_MESSAGE event in the IDLE state, it must first establish a session
before it can send the message.  It begins the session establishment sequence by sending the ESTABLISH_SESSION
event to the establishSession machine.  The top-level machine also transitions to the ESTABLISHING_SESSION state
in order to not interrupt the session establishment sequence, should a new SEND_MESSAGE event be received before
the session is fully established.  Then, when the establishSession machine has completed its work and has
returned :fsmlang:`event SESSION_ESTABLISHED`, the top-level machine will transition to the IN_SESSION state.
This transition is again necessary to take the correct action on the next SEND_MESSAGE event received.  It is
in this state, too, that the TIMER_EXPIRED event might occur, which would cause the machine to return to the
IDLE state.

Our machine now looks like this:

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
			event ESTABLISH_SESSION
	            , STEP0_RESPONSE
	            , STEP1_RESPONSE
	            , parent::MESSAGE_RECEIVED
	            ;

	      state IDLE
	            , AWAITING_RESPONSE
	            ;

	   }

	   machine sendMessage
	   {

	      event parent::SEND_MESSAGE
	            , parent::MESSAGE_RECEIVED
	            , ACK
	            ;

	      state IDLE;

	   }

	}

Next, we take up the states for the establishSession machine.

The ESTABLISH_SESSION message is first received in the IDLE state.  The machine will begin its work and move
to the AWAITING_RESPONSE state.  While in this state, responses from the peer are received.  Since the
"outside world" knows only about the hsmCommunicator, the MESSAGE_RECEIVED events are first seen there, then
shared down to the children; each child machine parses the message, creating the approprate machine events from
the message received.  In the case of the establishSession machine, it is looking for the STEP0_RESPONSE and
STEP1_RESPONSE messages.  Assuming the peer will not send these responses out of order, establishSession may
remain in the single AWAITING_RESPONSE state until both messages are received. Upon this, establishSession
returns SESSION_ESTABLISHED to the parent and transitions to the IDLE state.

The final machine, sendMessage, manages the sending of the client messages.  Since the protocol is simple, the
machine needs only a second state, AWAITING_ACK, to ensure that no more than one message is outstanding.

Here, then, is the final skeleton:

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
	      event ESTABLISH_SESSION
	            , STEP0_RESPONSE
	            , STEP1_RESPONSE
	            , parent::MESSAGE_RECEIVED
	            ;

	      state IDLE
	            , AWAITING_RESPONSE
	            ;

	   }

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

	}

.. note::

	FSMLang is perfectly happy to have sub-machine source in the same file as the top-level machine, as seen in the skeleton.  However,
	this can make for very long files.  To aleviate this, the :code:`include` keyword can be used to insert external files into the source
	exactly as if their contents were present at the point of inclusion.

The next pages flesh out each machine with the actions and transitions which make them perform.  Each machine is treated on its own page,
reflecting the use of the :fsmlang:`include` keyword in the top-level source.


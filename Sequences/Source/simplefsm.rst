======================
simpleCommunicator.fsm
======================

:download:`Download <../Source/simpleCommunicator.fsm>`

The sequences are highlighted

.. code-block:: fsmlang
	:emphasize-lines: 62, 63

	native
	{
	#include <stdio.h>
	#define DBG_PRINTF(...) printf(__VA_ARGS__); printf("\n")
	}
	
	/**
	   This machine manages communications using a "stop and wait" protocol.
	      Only one message is allowed to be outstanding.
	*/
	machine simpleCommunicator {
	
		state	IDLE,
					AWAITING_ACK;
	
		event	SEND_MESSAGE,
	NEVER_SEEN,
					ACK;
	
		action neverExecuted[NEVER_SEEN, (IDLE, AWAITING_ACK)];
	
	  /**
	    Since we're idle, we can simply send the message.  Transitioning
	    to the AWAITING_ACK state ensures that any other messages
	    we're asked to send will be queued.
	  */
		action	sendMessage[SEND_MESSAGE,IDLE] transition AWAITING_ACK;
	
	  /**
	    Since we're busy, we must queue the message for later
	    sending.  The queue will be checked when the ACK
	    is received.
	  */
		action	queueMessage[SEND_MESSAGE,AWAITING_ACK];
	
	  /**
	    We've received our ACK for the previous message.  It is 
	    time to check for any others.
	  */
		action	checkQueue[ACK,AWAITING_ACK] transition IDLE;
	
	  /* these lines are informational; they affect the html output,
	      but do not affect any code generated.
	  */
	
	  /** queueMessage adds a message to the queue */
	  queueMessage returns noEvent;
	
	  /** sendMessage sends a message from the queue.  The
	      message is expected to be there, since
	      checkQueue will have been previously called.
	  */
	  sendMessage  returns noEvent;
	
	  /** checkQueue only checks; it does not dequeue; that
	      is done by sendMessage.
	
	      Return SEND_MESSAGE when the queue is not empty.
	  */
	  checkQueue   returns SEND_MESSAGE, noEvent;
	
	  sequence seq1 SEND_MESSAGE, ACK;
	  sequence seq2 SEND_MESSAGE, SEND_MESSAGE, ACK, SEND_MESSAGE, ACK;
	
	}
	


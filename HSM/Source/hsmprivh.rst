======================
hsmCommunicator_priv.h
======================

.. code-block:: c

	/**
		hsmCommunicator_priv.h
	
		This file automatically generated by FSMLang
	*/
	
	#ifndef _HSMCOMMUNICATOR_PRIV_H_
	#define _HSMCOMMUNICATOR_PRIV_H_
	
	#include "hsmCommunicator.h"
	#ifndef HSM_COMMUNICATOR_NATIVE_PROLOG
	#define HSM_COMMUNICATOR_NATIVE_PROLOG
	
	#ifndef DBG_PRINTF
	#include <stdio.h>
	#define DBG_PRINTF(...) printf(__VA_ARGS__); printf("\n")
	#endif
	
	extern unsigned queue_count;
	
	#endif
	
	#ifdef HSM_COMMUNICATOR_DEBUG
	#include <stdio.h>
	#include <stdlib.h>
	#endif
	
	
	#ifdef HSM_COMMUNICATOR_DEBUG
	extern char *HSM_COMMUNICATOR_EVENT_NAMES[];
	extern char *HSM_COMMUNICATOR_STATE_NAMES[];
	#endif
	
	typedef enum {
	    hsmCommunicator_IDLE
	    , hsmCommunicator_ESTABLISHING_SESSION
	    , hsmCommunicator_IN_SESSION
	    , hsmCommunicator_numStates
	}  HSM_COMMUNICATOR_STATE;
	
	typedef struct _hsmCommunicator_struct_ HSM_COMMUNICATOR;
	#undef FSM_TYPE_PTR
	#define FSM_TYPE_PTR pHSM_COMMUNICATOR
	extern HSM_COMMUNICATOR hsmCommunicator;
	
	typedef HSM_COMMUNICATOR_EVENT (*HSM_COMMUNICATOR_ACTION_FN)(FSM_TYPE_PTR);
	
	typedef void (*HSM_COMMUNICATOR_FSM)(FSM_TYPE_PTR,HSM_COMMUNICATOR_EVENT);
	
	void hsmCommunicatorFSM(FSM_TYPE_PTR,HSM_COMMUNICATOR_EVENT);
	
	#include "hsmCommunicator_submach.h"
	typedef ACTION_RETURN_TYPE (*HSM_COMMUNICATOR_STATE_FN)(pHSM_COMMUNICATOR,HSM_COMMUNICATOR_EVENT);
	
	static const HSM_COMMUNICATOR_STATE_FN hsmCommunicator_state_fn_array[hsmCommunicator_numStates];
	
	struct _hsmCommunicator_struct_ {
	    HSM_COMMUNICATOR_STATE              state;
	    HSM_COMMUNICATOR_EVENT              event;
	    HSM_COMMUNICATOR_STATE_FN     const (*statesArray)[hsmCommunicator_numStates];
	    pHSM_COMMUNICATOR_SUB_FSM_IF   const (*subMachineArray)[hsmCommunicator_numSubMachines];
	    HSM_COMMUNICATOR_FSM                fsm;
	};
	
	
	ACTION_RETURN_TYPE hsmCommunicator_startSessionEstablishment(FSM_TYPE_PTR);
	ACTION_RETURN_TYPE hsmCommunicator_completeSessionStart(FSM_TYPE_PTR);
	ACTION_RETURN_TYPE hsmCommunicator_passMessageReceived(FSM_TYPE_PTR);
	ACTION_RETURN_TYPE hsmCommunicator_queueMessage(FSM_TYPE_PTR);
	ACTION_RETURN_TYPE hsmCommunicator_requestMessageTransmission(FSM_TYPE_PTR);
	
	
	
	#endif


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
	    hsmCommunicator_UNINITIALIZED
	    , hsmCommunicator_IDLE
	    , hsmCommunicator_ESTABLISHING_SESSION
	    , hsmCommunicator_IN_SESSION
	    , hsmCommunicator_numStates
	}  HSM_COMMUNICATOR_STATE;
	
	typedef struct _hsmCommunicator_struct_ HSM_COMMUNICATOR;
	#undef FSM_TYPE_PTR
	#define FSM_TYPE_PTR pHSM_COMMUNICATOR
	extern HSM_COMMUNICATOR hsmCommunicator;
	
	typedef HSM_COMMUNICATOR_EVENT_ENUM (*HSM_COMMUNICATOR_ACTION_FN)(FSM_TYPE_PTR);
	
	typedef void (*HSM_COMMUNICATOR_FSM)(FSM_TYPE_PTR,pHSM_COMMUNICATOR_EVENT);
	
	void hsmCommunicatorFSM(FSM_TYPE_PTR,pHSM_COMMUNICATOR_EVENT);
	
	#include "hsmCommunicator_submach.h"
	typedef ACTION_RETURN_TYPE (*HSM_COMMUNICATOR_STATE_FN)(pHSM_COMMUNICATOR,HSM_COMMUNICATOR_EVENT_ENUM);
	
	static const HSM_COMMUNICATOR_STATE_FN hsmCommunicator_state_fn_array[hsmCommunicator_numStates];
	
	struct _hsmCommunicator_struct_ {
	    HSM_COMMUNICATOR_DATA               data;
	    HSM_COMMUNICATOR_STATE              state;
	    HSM_COMMUNICATOR_EVENT_ENUM         event;
	    HSM_COMMUNICATOR_STATE_FN     const (*statesArray)[hsmCommunicator_numStates];
	    pHSM_COMMUNICATOR_SUB_FSM_IF   const (*subMachineArray)[hsmCommunicator_numSubMachines];
	    HSM_COMMUNICATOR_FSM                fsm;
	};
	
	
	ACTION_RETURN_TYPE hsmCommunicator_initialize(FSM_TYPE_PTR);
	ACTION_RETURN_TYPE hsmCommunicator_startSessionEstablishment(FSM_TYPE_PTR);
	ACTION_RETURN_TYPE hsmCommunicator_passMessageReceived(FSM_TYPE_PTR);
	ACTION_RETURN_TYPE hsmCommunicator_clearQueue(FSM_TYPE_PTR);
	ACTION_RETURN_TYPE hsmCommunicator_completeSessionStart(FSM_TYPE_PTR);
	ACTION_RETURN_TYPE hsmCommunicator_requestMessageTransmission(FSM_TYPE_PTR);
	
	void UFMN(track_transitions)(pHSM_COMMUNICATOR,HSM_COMMUNICATOR_STATE);
	
	
	void hsmCommunicator_start_session_timer(pHSM_COMMUNICATOR_DATA);
	void hsmCommunicator_stop_session_timer(pHSM_COMMUNICATOR_DATA);
	
	void hsmCommunicator_store_message(pHSM_COMMUNICATOR_DATA,pHSM_COMMUNICATOR_MESSAGE_RECEIVED_DATA);
	
	
	#endif


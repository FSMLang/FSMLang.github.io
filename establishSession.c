/**
	establishSession.c

	This file automatically generated by FSMLang
*/

#include "establishSession.h"


const ESTABLISH_SESSION_ACTION_TRANS establishSession_action_array[establishSession_numEvents][establishSession_numStates] =
{
	{
		/* -- ESTABLISH_SESSION_REQUEST -- */

		/* -- IDLE -- */	{establishSession_sendStep0Message,establishSession_AWAITING_RESPONSE}
		/* -- AWAITING_RESPONSE -- */	, {establishSession_noAction, establishSession_AWAITING_RESPONSE}
	},
	{
		/* -- STEP0_RESPONSE -- */

		/* -- IDLE -- */	{establishSession_noAction, establishSession_IDLE}
		/* -- AWAITING_RESPONSE -- */	, {establishSession_sendStep1Message,establishSession_AWAITING_RESPONSE}
	},
	{
		/* -- STEP1_RESPONSE -- */

		/* -- IDLE -- */	{establishSession_noAction, establishSession_IDLE}
		/* -- AWAITING_RESPONSE -- */	, {establishSession_notifyParent,establishSession_IDLE}
	},
};
HSM_COMMUNICATOR_EVENT establishSession_sub_machine_fn(HSM_COMMUNICATOR_EVENT e)
{
	return establishSessionFSM(pestablishSession, e);
}


HSM_COMMUNICATOR_SUB_FSM_IF establishSession_sub_fsm_if =
{
		.subFSM = establishSession_sub_machine_fn
	, .first_event = hsmCommunicator_establishSession_ESTABLISH_SESSION_REQUEST
	, .last_event = hsmCommunicator_establishSession_STEP1_RESPONSE
};

ESTABLISH_SESSION establishSession = {
	establishSession_IDLE,
	&establishSession_action_array,
	establishSessionFSM
};

pESTABLISH_SESSION pestablishSession = &establishSession;

HSM_COMMUNICATOR_EVENT establishSessionFSM(pESTABLISH_SESSION pfsm, HSM_COMMUNICATOR_EVENT event)
{
/* writeOriginalSubFSM */
	HSM_COMMUNICATOR_EVENT new_e;

	HSM_COMMUNICATOR_EVENT e = event;

	while (
		(e != THIS(noEvent))
		&& (e >= THIS(ESTABLISH_SESSION_REQUEST))
	)
	{

#ifdef ESTABLISH_SESSION_DEBUG
DBG_PRINTF("event: %s; state: %s"
,ESTABLISH_SESSION_EVENT_NAMES[e - THIS(ESTABLISH_SESSION_REQUEST)]
,ESTABLISH_SESSION_STATE_NAMES[pfsm->state]
);
#endif

		new_e = ((* (*pfsm->actionArray)[e - THIS(ESTABLISH_SESSION_REQUEST)][pfsm->state].action)(pfsm));

		pfsm->state = (*pfsm->actionArray)[e - THIS(ESTABLISH_SESSION_REQUEST)][pfsm->state].transition;

		e = new_e;

	} 
	return e == THIS(noEvent) ? PARENT(noEvent) : e;

}

HSM_COMMUNICATOR_EVENT __attribute__((weak)) establishSession_sendStep0Message(pESTABLISH_SESSION pfsm)
{
	DBG_PRINTF("weak: establishSession_sendStep0Message");
	return THIS(noEvent);
}

HSM_COMMUNICATOR_EVENT __attribute__((weak)) establishSession_sendStep1Message(pESTABLISH_SESSION pfsm)
{
	DBG_PRINTF("weak: establishSession_sendStep1Message");
	return THIS(noEvent);
}

HSM_COMMUNICATOR_EVENT __attribute__((weak)) establishSession_notifyParent(pESTABLISH_SESSION pfsm)
{
	DBG_PRINTF("weak: establishSession_notifyParent");
	return THIS(noEvent);
}


#ifdef ESTABLISH_SESSION_DEBUG
char *ESTABLISH_SESSION_EVENT_NAMES[] = {
	 "establishSession_ESTABLISH_SESSION_REQUEST"
	, "establishSession_STEP0_RESPONSE"
	, "establishSession_STEP1_RESPONSE"
	, "establishSession_noEvent"
	, "establishSession_numEvents"
};

char *ESTABLISH_SESSION_STATE_NAMES[] = {
	 "establishSession_IDLE"
	,"establishSession_AWAITING_RESPONSE"
};

#endif

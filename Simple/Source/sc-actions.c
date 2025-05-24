
#include "simpleCommunicator_priv.h"

SIMPLE_COMMUNICATOR_EVENT UFMN(sendMessage)(FSM_TYPE_PTR pfsm)
{
	DBG_PRINTF("%s", __func__);
	(void) pfsm;
	return THIS(noEvent);
}

SIMPLE_COMMUNICATOR_EVENT UFMN(queueMessage)(FSM_TYPE_PTR pfsm)
{
	DBG_PRINTF("%s", __func__);
	(void) pfsm;
	return THIS(noEvent);
}

SIMPLE_COMMUNICATOR_EVENT UFMN(checkQueue)(FSM_TYPE_PTR pfsm)
{
	DBG_PRINTF("%s", __func__);
	(void) pfsm;
	return THIS(noEvent);
}

SIMPLE_COMMUNICATOR_EVENT UFMN(noAction)(FSM_TYPE_PTR pfsm)
{
	DBG_PRINTF("%s", __func__);
	(void) pfsm;
	return THIS(noEvent);
}


int main(void)
{

	run_simpleCommunicator(THIS(SEND_MESSAGE));
	run_simpleCommunicator(THIS(ACK));

	return 0;
}


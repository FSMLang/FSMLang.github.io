=================
hsmc-actions.c
=================

:download:`Download <../Source/hsmc-actions.c>`

.. code-block:: c

	/*
	  hsmc-actions.c
	
		Action functions for the hsmCommunicator top-level FSM.
	
	    FSMLang (fsm) - A Finite State Machine description language.
	    Copyright (C) 2024  Steven Stanton
	
	    This program is free software; you can redistribute it and/or modify
	    it under the terms of the GNU General Public License as published by
	    the Free Software Foundation; either version 2 of the License, or
	    (at your option) any later version.
	
	    This program is distributed in the hope that it will be useful,
	    but WITHOUT ANY WARRANTY; without even the implied warranty of
	    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	    GNU General Public License for more details.
	
	    You should have received a copy of the GNU General Public License
	    along with this program; if not, write to the Free Software
	    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
	
	    Steven Stanton
	    sstanton@pesticidesoftware.com
	
	    For the latest on FSMLang: https://fsmlang.github.io/
	
	    And, finally, your possession of this source code implies nothing.
	
	*/
	
	#include "hsmCommunicator_priv.h"
	
	ACTION_RETURN_TYPE  UFMN(startSessionEstablishment)(FSM_TYPE_PTR pfsm)
	{
	    DBG_PRINTF("%s", __func__);
	
	    pfsm->data.queue.queue_count++;
	
	    return ESTABLISH_SESSION(ESTABLISH_SESSION_REQUEST);
	}
	
	ACTION_RETURN_TYPE  UFMN(completeSessionStart)(FSM_TYPE_PTR pfsm)
	{
	    DBG_PRINTF("%s", __func__);
	    return SEND_MESSAGE(SEND_MESSAGE);
	}
	
	ACTION_RETURN_TYPE  UFMN(queueMessage)(FSM_TYPE_PTR pfsm)
	{
	    DBG_PRINTF("%s", __func__);
	
	    pfsm->data.queue.queue_count++;
	
	    return SEND_MESSAGE(SEND_MESSAGE);
	}
	
	ACTION_RETURN_TYPE  UFMN(clearQueue)(FSM_TYPE_PTR pfsm)
	{
	    DBG_PRINTF("%s", __func__);
	
	    pfsm->data.queue.queue_count = 0;
	
	    return THIS(noEvent);
	}
	
	ACTION_RETURN_TYPE  UFMN(requestMessageTransmission)(FSM_TYPE_PTR pfsm)
	{
	    DBG_PRINTF("%s", __func__);
	
	    pfsm->data.queue.queue_count++;
	
	    return SEND_MESSAGE(SEND_MESSAGE);
	}
	
	ACTION_RETURN_TYPE  UFMN(noAction)(FSM_TYPE_PTR pfsm)
	{
	    DBG_PRINTF("%s", __func__);
	    (void) pfsm;
	    return THIS(noEvent);
	}
	
	void hsmCommunicator_store_message(pHSM_COMMUNICATOR_DATA pfsm_data, pHSM_COMMUNICATOR_MESSAGE_RECEIVED_DATA pedata)
	{
	    DBG_PRINTF("%s", __func__);
	
	    pfsm_data->current_msg = pedata->message;
	}
	
	void UFMN(track_transitions)(pHSM_COMMUNICATOR pfsm, HSM_COMMUNICATOR_STATE s)
	{
	    DBG_PRINTF("%s", __func__);
	    (void) pfsm;
	}
	
	void hsmCommunicator_start_session_timer(pHSM_COMMUNICATOR_DATA pfsm)
	{
	    DBG_PRINTF("%s", __func__);
	    (void) pfsm;
	}
	
	void hsmCommunicator_stop_session_timer(pHSM_COMMUNICATOR_DATA pfsm)
	{
	    DBG_PRINTF("%s", __func__);
	    (void) pfsm;
	}
	
	int main(void)
	{
	    HSM_COMMUNICATOR_EVENT event;
	
	    event.event = THIS(INIT);
	    run_hsmCommunicator(&event);
	
	    event.event = THIS(SEND_MESSAGE);
	    run_hsmCommunicator(&event);
	
	    event.event = THIS(MESSAGE_RECEIVED);
	    event.event_data.MESSAGE_RECEIVED_data.message = msg_step0_response;
	    run_hsmCommunicator(&event);
	
	    event.event = THIS(SEND_MESSAGE);
	    run_hsmCommunicator(&event);
	
	    event.event = THIS(MESSAGE_RECEIVED);
	    event.event_data.MESSAGE_RECEIVED_data.message = msg_step1_response;
	    run_hsmCommunicator(&event);
	
	    event.event = THIS(MESSAGE_RECEIVED);
	    event.event_data.MESSAGE_RECEIVED_data.message = msg_ack;
	    run_hsmCommunicator(&event);
	
	    event.event = THIS(MESSAGE_RECEIVED);
	    event.event_data.MESSAGE_RECEIVED_data.message = msg_ack;
	    run_hsmCommunicator(&event);
	
	    return 0;
	}
	

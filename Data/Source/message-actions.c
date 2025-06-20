/*
  message_actions.c

	Action functions for the sendMessage FSM.

	FSMLang (fsm) - A Finite State Machine description language.
	Copyright (C) 4/20/2025  Steven Stanton

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

	For the latest on FSMLang: http://fsmlang.github.io

	And, finally, your possession of this source code implies nothing.

*/

#include "sendMessage_priv.h"

ACTION_RETURN_TYPE UFMN(sendMessage)(FSM_TYPE_PTR pfsm)
{
    DBG_PRINTF("%s", __func__);

    pfsm->data.pqueue->queue_count--;

    return THIS(noEvent);
}

ACTION_RETURN_TYPE UFMN(parseMessage)(FSM_TYPE_PTR pfsm)
{
    DBG_PRINTF("%s", __func__);
    (void) pfsm;
    return THIS(ACK);
}

ACTION_RETURN_TYPE UFMN(checkQueue)(FSM_TYPE_PTR pfsm)
{
    DBG_PRINTF("%s", __func__);

    return (pfsm->data.pqueue->queue_count > 0) ? THIS(SEND_MESSAGE) : THIS(noEvent);
}

ACTION_RETURN_TYPE UFMN(noAction)(FSM_TYPE_PTR pfsm)
{
    DBG_PRINTF("%s", __func__);
    (void) pfsm;
    return THIS(noEvent);
}

void UFMN(copy_current_message)(pHSM_COMMUNICATOR_DATA pfsm_data)
{
    DBG_PRINTF("%s", __func__);

	psendMessage->data.current_msg = pfsm_data->current_msg;

}

void UFMN(init_data)(pHSM_COMMUNICATOR_DATA pfsm_data)
{
    DBG_PRINTF("%s", __func__);

	psendMessage->data.pqueue = &pfsm_data->queue;

}


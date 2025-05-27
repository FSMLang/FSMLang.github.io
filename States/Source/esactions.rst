=================
session-actions.c
=================

:download:`Download <../Source/session-actions.c>`

.. code-block:: c

	/*
	  session_actions.c
	
		Action functions for the establishSession FSM.
	
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
	
	#include <stdbool.h>
	#include "establishSession_priv.h"
	
	ACTION_RETURN_TYPE UFMN(sendStep0Message)(FSM_TYPE_PTR pfsm)
	{
	    DBG_PRINTF("%s", __func__);
	    (void) pfsm;
	    return THIS(noEvent);
	}
	
	ACTION_RETURN_TYPE UFMN(sendStep1Message)(FSM_TYPE_PTR pfsm)
	{
	    DBG_PRINTF("%s", __func__);
	    (void) pfsm;
	    return THIS(noEvent);
	}
	
	ACTION_RETURN_TYPE UFMN(notifyParent)(FSM_TYPE_PTR pfsm)
	{
	    DBG_PRINTF("%s", __func__);
	    (void) pfsm;
	    return PARENT(SESSION_ESTABLISHED);
	}
	
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
	
	ACTION_RETURN_TYPE UFMN(noAction)(FSM_TYPE_PTR pfsm)
	{
	    DBG_PRINTF("%s", __func__);
	    (void) pfsm;
	    return THIS(noEvent);
	}
	
	void UFMN(copy_current_message)(pHSM_COMMUNICATOR_DATA pfsm_data)
	{
	    DBG_PRINTF("%s", __func__);
	
	    pestablishSession->data.current_msg = pfsm_data->current_msg;
	}
	
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
	

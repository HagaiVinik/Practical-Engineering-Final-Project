#include "Board.h"
#include "logger.h"


#define _CRT_SECURE_NO_WARNINGS


std::map<long,std::function<void(METAEVENT)>> Board::states_map;

/*con/destructors*/
Board :: Board()
{
	D_Channel_one_state		=	DATA_LINK_DOWN	;
	D_Channel_two_state		=	DATA_LINK_DOWN	;

	states_map[GCEV_UNBLOCKED]	=	std::bind(&Board::lineUnblocked, this, std::placeholders::_1)	 ;
	states_map[GCEV_BLOCKED]		=	std::bind(&Board::lineBlocked, this, std::placeholders::_1)	 ;
	states_map[GCEV_OFFERED]		=	std::bind(&Board::callOffered, this, std::placeholders::_1)      ;
	states_map[GCEV_CONNECTED]		=	std::bind(&Board::callConnected, this, std::placeholders::_1)    ;
	states_map[GCEV_DISCONNECTED]	=	std::bind(&Board::callDisconnected, this, std::placeholders::_1) ;
	states_map[GCEV_D_CHAN_STATUS]	=	std::bind(&Board::getTrunksState, this, std::placeholders::_1)	 ;   //Problem
	states_map[GCEV_ANSWERED]		=	std::bind(&Board::callAnswered, this, std::placeholders::_1);
}

Board :: ~Board()
{
}

/* Global call methods */
long Board ::  eventHandler(unsigned long param)
{
	int static counter    =      1 ;
	int error             =     -1 ; /* reason for failure of function */
	struct channel *pline          ;
	METAEVENT metaevent            ; /* event data block */
	GC_TRACEDATA* trace_data       ;

	if (gc_GetMetaEvent(&metaevent) != GC_SUCCESS)
	{    
		/* process error return as shown */
		std::cout <<"gc_metaevent failed.....\n" << std::endl;
		return(error);
	}    
	if( metaevent.flags & GCME_GC_EVENT) // check that its a global call event.
	{
		if (gc_GetUsrAttr(metaevent.linedev, (void **)&pline) != GC_SUCCESS)
		{
			/* process error return as shown */
			std::cout <<"gc_GetUsrAttr failed.....\n"<< std::endl;
			return error;
		}
		std::cout <<"\n============================================================== " << std::endl;
		std::cout <<"event number: " << counter++ <<" Event id: " << metaevent.evttype << "  " ;
		switch (metaevent.evttype)
		{
		case GCEV_UNBLOCKED:
			std::cout <<"GCEV_UNBLOCKED - call can be execute now.\n" << std::endl;
			break;
		case GCEV_BLOCKED:
			std::cout <<"GCEV_BLOCKED - lines blocked.\n" << std::endl;             
			break;
		case GCEV_D_CHAN_STATUS:
			std::cout <<"GCEV_D_CHAN_STATUS - GCEV_D_CHAN_STATUS CHANGED.\n" << std::endl;
			break;
		case GCEV_CONNECTED:
			//std::cout <<"GCEV_CONNECTED - Remote end connected.\n" << std::endl;
			break;
		case GCEV_DISCONNECTED:
			//std::cout <<" GCEV_DISCONNECTED - Remote end disconnected " << std::endl;
			break;
		case GCEV_ERROR:
			std::cout <<"GCEV_ERROR - error occurred\n" << std::endl;
			break;
		case GCEV_OPENEX:
			std:: cout <<"GCEV_OPENEX - device opened successfully." << endl;
			break;
		case GCEV_OPENEX_FAIL:
			std::cout <<"GCEV_OPENEX_FAIL - failed to open lines\n" << std::endl;
			break;
		case GCEV_OFFERED:
			std::cout <<"GCEV_OFFERED - a connection request has been made!\n" << std::endl;	
			break;
		case GCEV_TASKFAIL:
			std::cout <<"GCEV_TASKFAIL - a task failed.\n" << std::endl;
			break;
		case GCEV_DROPCALL:
			std::cout <<"GCEV_DROPCALL - gc_dropcall() completed.\n" << std::endl;
			break;
		case GCEV_CALLINFO:
			std::cout <<"GCEV_CALLINFO -dont know what to do\n" << std::endl;
			break;
		case GCEV_PROCEEDING:
			std::cout << "GCEV_PROCEEDING - The call state has been changed to the proceeding state\n"<< std::endl ;
			break;
		case GCEV_ALERTING:
			std::cout << "GCEV_ALERTING - alerting activated\n" << std::endl ;
			break;
		case GCEV_EXTENSION:
			std::cout <<"GCEV_EXTENSION - Unsolicited extension event\n" << std::endl;
			trace_data = (GC_TRACEDATA*)metaevent.extevtdatap;
			printf("sequence number = %d\n",trace_data->seq_no);
			printf("size of data = %d\n",strlen((char*)trace_data->data_buf));
			printf("data is - %s\n",trace_data->data_buf);
			break;
		case GCEV_ANSWERED:
			std::cout << "GCEV_ANSWERED - answered call.\n" << std::endl ;
			break;
		case GCEV_RELEASECALL:
			std::cout << "GCEV_RELEASECALL - call released.\n" << std::endl ;
			break;
		case GCEV_TRACEDATA:
			std::cout << "GCEV_TRACEDATA - tracing data!!."<< std::endl ;
			trace_data = (GC_TRACEDATA*)metaevent.extevtdatap;
			break;
		case GCEV_HOLDACK:
			std::cout << "GCEV_HOLDACK - holding call!!."<< std::endl ;
			break;
		case GCEV_HOLDREJ:
			std::cout << "GCEV_HOLDREJ - holding call."<< std::endl ;
			int retCode;
			GC_INFO t_Info;  //struct of details about the event.
			// printf("trying to identify result.....\n");
			retCode = gc_ResultInfo(&metaevent, &t_Info);
			break;
		default:
			std::cout << "Unexpected GlobalCall event received\n"<< std::endl ;
			break;
		}
		auto value = states_map[metaevent.evttype];
		if(value != NULL)
		{
			value(metaevent);
		}

		std :: cout << "event occured on deviceline number - " << metaevent.linedev << std::endl;		  
		//Logger :: PrintResultInfo(&metaevent);
	}
	else
	{
		std::cout << "non-global event occured.\n" << std::endl ;
	}
	std::cout <<"============================================================== " << std::endl;
	return 0;
}

void Board :: startGClib()
{
	/* Start the library */
	if (gc_Start(NULL) != GC_SUCCESS)
	{
		/* process error return as shown */
		std::cout << "error: gc_Start failed." << std::endl;
	} 
	else
	{
		std::cout << "gc_start success." << std::endl;
	}
}

void Board :: initHandler()  
{
	int mode  ;
	/* Set SRL mode */
	mode    =    SR_STASYNC;
	if (sr_setparm(SRL_DEVICE, SR_MODEID, &mode) == -1) 
	{
		std::cout << "unable to set to polled Mode" << std::endl;
		gc_Stop();
		exit(EXIT_CODE_ERROR);
	}
	/* Enable the event handler */
	if (sr_enbhdlr(EV_ANYDEV, EV_ANYEVT, &Board :: eventHandler) == -1) 	
	{
		std::cout << "sr_enbhdlr failed" << std::endl;
		gc_Stop();
		exit(EXIT_CODE_ERROR);
	}
}

void Board :: openTrunksDchannel()
{
	for(int i = 0; i < MAX_NUM_TRUNKS; i++)
	{
		m_trunksArr[i].openD_Channel(i+1);  /* argument is index for name of board */
	}
}

void Board :: openTrunks()
{
	for(int i = 0; i < MAX_NUM_TRUNKS; i++)
	{
		m_trunksArr[i].openLineBags(i+1);
	}
}

void Board :: closeTrunks()
{
	for(int i = 0; i < MAX_NUM_TRUNKS; i++)
	{
		m_trunksArr[i].closeAllLineBags();
	}
}

void Board :: stopGC()
{
	if (gc_Stop() != GC_SUCCESS )
		log.PrintErrorInfo();
	else
		printf("gc_stop success\n");

}

void Board :: waitCallInTrunks()
{
	for(int i = 0; i < MAX_NUM_TRUNKS; i++)
	{
		m_trunksArr[i].waitForCall();
	}
}

/* Handle Flow: */
void Board :: getTrunksState(METAEVENT metaevent)
{
	D_Channel_one_state = m_trunksArr[0].getDchannelState();
	D_Channel_two_state = m_trunksArr[1].getDchannelState();
	Board::UpdateMessage(metaevent);
}

void Board :: callOffered(METAEVENT metaevent)
{
	int ts = -1;
	char adress[256] = {0};
	//offeredCall = metaevent.crn;
	std::cout << __FUNCTION__ << std::endl;
	ts = Board::findTimeSlot(metaevent);
	offeredCallsArr[ts] = metaevent.crn;
	std::cout << "entered callOFFERED state" << std::endl ;
	
	/* answer call was here. */
	

	if(gc_GetCallInfo(metaevent.crn, DESTINATION_ADDRESS , adress) != GC_SUCCESS) 
	{
	/* process error return as shown */
	}
	
	
	m_trunksArr[0].makeCall(ts,adress);
	Board::UpdateMessage(metaevent);
}

void Board :: callConnected(METAEVENT metaevent)
{
	int ts = Board::findTimeSlot(metaevent);
	if (gc_AnswerCall(offeredCallsArr[ts], 0, EV_ASYNC) != GC_SUCCESS) 			
		log.PrintErrorInfo();
	else
		std::cout <<"destination answered successfully.\n" << std::endl;
	Board::UpdateMessage(metaevent);
}

void Board :: callDisconnected(METAEVENT metaevent)
{
	std::cout << __FUNCTION__ << std::endl;
	std::cout << "GCEV_DISCONNECTED - remote end disconnected\n" << std::endl;
	if (gc_DropCall(metaevent.crn, GC_NORMAL_CLEARING, EV_SYNC) != GC_SUCCESS)
	{
		std::cout << "ERROR: gc_dropcall Failed" << std::endl;
		log.PrintErrorInfo();
	}
	else
		std::cout << "SUCCESS: CALL DROPPED" << std::endl;

	if (gc_ReleaseCallEx(metaevent.crn, EV_SYNC) != GC_SUCCESS)
	{
		std::cout << "ERROR: release call failed.....\n" << std::endl;
		log.PrintErrorInfo();
	}
	else
		std::cout << "SUCCESS: CALL RELAESED" << std::endl;

	Board::UpdateMessage(metaevent);
}

void Board :: lineUnblocked(METAEVENT metaevent)
{
	Board::UpdateMessage(metaevent);
}

void Board :: lineBlocked(METAEVENT metaevent)
{
	Board::UpdateMessage(metaevent);
}

void Board :: callAnswered(METAEVENT metaevent)
{
	int network_handler_Channel1 , network_handler_channel15; 
	std::cout << __FUNCTION__ << std::endl;
	std::cout <<"GCEV_CONNECTED - Destination answered the request\n" << std::endl;
	int time_slot = Board::findTimeSlot(metaevent);
	if(gc_GetResourceH(m_trunksArr[0].m_lineBagArr[time_slot].m_ldev, &network_handler_Channel1, GC_NETWORKDEVICE) != GC_SUCCESS)
	{
		std::cout << "error in gc_RetResource" << std::endl;
	}
	if(gc_GetResourceH(m_trunksArr[1].m_lineBagArr[time_slot].m_ldev, &network_handler_channel15, GC_NETWORKDEVICE) != GC_SUCCESS)
	{
		std::cout << "error in gc_RetResource" << std::endl;
	}

	if(nr_scroute(network_handler_Channel1, SC_DTI , network_handler_channel15, SC_DTI , SC_FULLDUP) < 0)
		std::cout << "Sound couldn't pass" << std::endl;
	Board::UpdateMessage(metaevent);
}

/* methods */
void Board :: startTrace()
{
	int rc;
	char *filename;
	filename="C:\\Users\\OhadKoren\\Documents\\visual studio 2012\\Projects\\PSTNDiagTemp\\PSTNDiagTemp\\trace2.log";
	rc = gc_StartTrace(m_trunksArr[0].D_channel, filename);
	if (rc != GC_SUCCESS) 
	{
		std::cout <<"Error in gc_StartTrace, rc = "<< rc << std::endl;
	}
	else
	{
		/* continue */
	}
}

int Board :: findTimeSlot(METAEVENT metaevent)
{
	// for d channel.
	int ts = -1;
	
	if( metaevent.linedev == m_trunksArr[0].D_channel )
		if (D_Channel_one_state == DATA_LINK_UP)
			return DATA_LINK_UP;	
		else
			return DATA_LINK_DOWN;

	if( metaevent.linedev == m_trunksArr[1].D_channel  )
		if (D_Channel_two_state == DATA_LINK_UP)
			return DATA_LINK_UP;	
		else
			return DATA_LINK_DOWN;
	
	for (int j = 0 ; j < MAX_NUM_TRUNKS ; j++)  //for time slot.
	{
		for (int i = 1 ; i <= MAXCHAN ; i++)
		{
			if(m_trunksArr[j].m_lineBagArr[i].m_ldev == metaevent.linedev)
				ts = i;
		}
	}
	return ts;

}

int Board :: findTrunk(METAEVENT metaevent)
{
	int trunk = -1;
	if( metaevent.linedev == m_trunksArr[0].D_channel )
		return 1;
	if( metaevent.linedev == m_trunksArr[1].D_channel )
		return 2;

	for (int j = 0 ; j < MAX_NUM_TRUNKS ; j++)
	{
		for (int i = 1 ; i <= MAXCHAN ; i++)
		{
			if(m_trunksArr[j].m_lineBagArr[i].m_ldev == metaevent.linedev)
				trunk = j+1;
		}
	}
	return trunk;
}

void Board :: UpdateMessage(METAEVENT metaevent)
{
	string buffer= "";
	
	switch (metaevent.evttype)
	{
		case GCEV_D_CHAN_STATUS:
			buffer.append("GCEV_D_CHAN_STATUS:");
			break;
		case GCEV_OFFERED:
			buffer.append("GCEV_OFFERED:");
			break;
		case GCEV_CONNECTED:
			buffer.append("GCEV_CONNECTED:");
			break;
		case GCEV_DISCONNECTED:
			buffer.append("GCEV_DISCONNECTED:");
			break;
		case GCEV_UNBLOCKED:
			buffer.append("GCEV_UNBLOCKED:");
			break;
		case GCEV_BLOCKED:
			buffer.append("GCEV_BLOCKED:");
			break;
		case GCEV_ANSWERED:
			buffer.append("GCEV_ANSWERED:");
		default:
			break;
	}

	buffer.append(std::to_string(metaevent.linedev).data());
	buffer.append(":");
	int trunk = Board::findTrunk(metaevent);
	int time_slot = Board::findTimeSlot(metaevent);

	buffer.append(std::to_string(trunk).data());
	buffer.append(":");
	buffer.append(std::to_string(time_slot).data());

	log.writeToLog("Event  " + buffer + "  sent to client.");

	mtx_message.lock();
	vec_msgs.push(buffer);
	mtx_message.unlock();

}

/* handle client requests */

void Board :: handle_client_message(std::string msg)
{

}

void Board :: closeOneTrunk( int trunk )
{
	m_trunksArr[trunk].closeAllLineBags();
}

void Board :: closeSpecificTimeSlot(int trunk , int ts)
{
	m_trunksArr[trunk-1].closeOneLineBag(ts);
}

void Board :: openSpecificTimeSlot(int trunk , int ts)
{
	m_trunksArr[trunk-1].openOneLineBag(trunk,ts);
}

void Board :: forceCloseCall(int trunk, int ts)
{
	if(trunk == 2)
	{
		//std::cout << "trunk is = " << trunk -1 << endl; 
		if (gc_DropCall(offeredCallsArr[ts], GC_NORMAL_CLEARING, EV_SYNC) != GC_SUCCESS)
		{
			std::cout << "ERROR: gc_dropcall Failed" << std::endl;
			log.PrintErrorInfo();
		}
		else
			std::cout << "SUCCESS: CALL DROPPED" << std::endl;

		if (gc_ReleaseCallEx(offeredCallsArr[ts], EV_SYNC) != GC_SUCCESS)
		{
			std::cout << "ERROR: release call failed.....\n" << std::endl;
			log.PrintErrorInfo();
		}
		else
			std::cout << "SUCCESS: CALL RELAESED" << std::endl;
	}
	
	else
	{
		if (gc_DropCall(m_trunksArr[0].m_lineBagArr[ts].m_crn, GC_NORMAL_CLEARING, EV_SYNC) != GC_SUCCESS)
		{
			std::cout << "ERROR: gc_dropcall Failed" << std::endl;
			log.PrintErrorInfo();
		}
		else
			std::cout << "SUCCESS: CALL DROPPED" << std::endl;

		if (gc_ReleaseCallEx(m_trunksArr[0].m_lineBagArr[ts].m_crn, EV_SYNC) != GC_SUCCESS)
		{
			std::cout << "ERROR: release call failed.....\n" << std::endl;
			log.PrintErrorInfo();
		}
		else
			std::cout << "SUCCESS: CALL RELAESED" << std::endl;
	}
}

void Board :: holdCall(int trunk , int ts)
{
	int network_handler_Channel1 , network_handler_channel15; 
	
	if(gc_GetResourceH(m_trunksArr[0].m_lineBagArr[ts].m_ldev, &network_handler_Channel1, GC_NETWORKDEVICE) != GC_SUCCESS)
	{
		std::cout << "error in gc_RetResource" << std::endl;
	}
	if(gc_GetResourceH(m_trunksArr[1].m_lineBagArr[ts].m_ldev, &network_handler_channel15, GC_NETWORKDEVICE) != GC_SUCCESS)
	{
		std::cout << "error in gc_RetResource" << std::endl;
	}
	if(nr_scunroute(network_handler_Channel1, SC_DTI , network_handler_channel15, SC_DTI , SC_FULLDUP) < 0)
		std::cout << "Blocking Sound Faild" << std::endl;
	else
		std::cout << "NOTE: SOUND BLOCKED." << std::endl;
}

void Board :: resumeCall(int trunk, int ts)
{
	//if(gc_RetrieveCall(offeredCallsArr[ts], EV_ASYNC) != GC_SUCCESS) 
	//{
		/* Process error */
	//	std::cout << "ERROR: gc_RetrieveCall Failed" << std::endl;
		//log.PrintErrorInfo();
	//}
	//return;

	int network_handler_Channel1 , network_handler_channel15; 
	
	if(gc_GetResourceH(m_trunksArr[0].m_lineBagArr[ts].m_ldev, &network_handler_Channel1, GC_NETWORKDEVICE) != GC_SUCCESS)
	{
		std::cout << "error in gc_RetResource" << std::endl;
	}
	if(gc_GetResourceH(m_trunksArr[1].m_lineBagArr[ts].m_ldev, &network_handler_channel15, GC_NETWORKDEVICE) != GC_SUCCESS)
	{
		std::cout << "error in gc_RetResource" << std::endl;
	}
	if(nr_scroute(network_handler_Channel1, SC_DTI , network_handler_channel15, SC_DTI , SC_FULLDUP) < 0)
		std::cout << "returning Sound Faild" << std::endl;
	else
		std::cout << "NOTE: SOUND RETURNED." << std::endl;
}

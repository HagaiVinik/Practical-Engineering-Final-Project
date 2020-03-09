/*====================================================================
* AUTHOR: Hagai Vinik	.	
* DATE: 16/05/19		.
/* copyright (c) 2018 Hagai Vinik , all rights reserved. */
/*====================================================================*/


#pragma once
#include "Communication.h"
#include "Trunk.h"
#include "sctools.h"
#include "logger.h"



#define MAX_NUM_TRUNKS 2


class Board
{
public:	
	/* Local Variables: */
	int D_Channel_one_state               ;     /* values available: DATA_LINK_DOWN , DATA_LINK_UP */
	int D_Channel_two_state               ;		/* values available: DATA_LINK_DOWN , DATA_LINK_UP */
	std::queue <std::string> vec_msgs	  ;
	
	Logger log                            ;					  ;
	CRN offeredCallsArr[MAXCHAN+1]		  ;
	Trunk m_trunksArr[MAX_NUM_TRUNKS]     ;

	// Map of Functions to handle call states. //
	static std::map<long,std::function<void(METAEVENT)>> Board::states_map   ;
	
	/* Constructor/Destructor: */
	Board();
	~Board(void);
	
	/* GC Methods: */
	static long eventHandler(unsigned long)  ;		/* event handler */
	void startGClib()                        ;		/* start global call application */
	void initHandler()                       ;		/* initialize event caller */
	void openTrunksDchannel()                ;		/* open d channels for all availiable trunks */		
	void openTrunks()                        ;		/* opens linebags for all availiable trunks */
	void closeTrunks()                       ;		/* close all trunks */
	void stopGC()                            ;		/* stop golbal call application */
	 
	/* application methods*/
	void startTrace()                           ;       /* start tracing. */
	int findTimeSlot(METAEVENT metaevent)		;
	int findTrunk(METAEVENT metaevent)			;
	void waitCallInTrunks()						;		/* set linebags to wait for calls */
	void getTrunksState(METAEVENT metaevent)    ;		/* get state of d channel for each trunk */
	void callOffered(METAEVENT metaevent)	    ;       /* handles call offerring */
	void callAnswered(METAEVENT metaevent)	    ;       /* handles call answering */
	void callConnected(METAEVENT metaevent)     ;		/* handles call connecting */
	void callDisconnected(METAEVENT metaevent)  ;		/* handles call disconnecting */
	void lineUnblocked(METAEVENT metaevent)		;		/* alert line unblocked */
	void lineBlocked(METAEVENT metaevent)		;		/* alert line blocked */
	/* Socket */
	void UpdateMessage(METAEVENT metaevent)		;		/* make message and update the public message variable */
	void handle_client_message(std::string msg)	;
	void closeOneTrunk( int trunk)				;
	void closeSpecificTimeSlot( int trunk, int ts)	;
	void openSpecificTimeSlot(int trunk , int ts)	;
	void forceCloseCall(int trunk ,int ts)			;
	void holdCall(int trunk , int ts)				;
	void resumeCall(int trunk, int ts)				;
};
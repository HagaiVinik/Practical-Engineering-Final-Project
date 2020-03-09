/*====================================================================
* AUTHOR: Hagai Vinik	.	
* DATE: 16/05/19		.
/* copyright (c) 2018 Hagai Vinik , all rights reserved. */
/*====================================================================*/


#pragma once
#include "linebag.h"


class Trunk

{
public:
	/* Local Variables: */
	LineBag m_lineBagArr[MAXCHAN+1];
	LINEDEV D_channel; /* Board level device */
	/* Constructor/Destructor: */
	Trunk();
	~Trunk();

	/* Methods: */
	int getStatusOfBchannel(LINEDEV ldev);
	int getDchannelState();
	int openD_Channel(int index);  /* open d channel  */
	int openLineBags(int index);
	int closeAllLineBags();
	int waitForCall();
	int makeCall(int LineBagIndex, char *adress);
	int closeOneLineBag(int timeSlot);
	int openOneLineBag(int trunk ,int ts);
	int WaitForCallOnSpecificTS(int ts);
	//TODO: Close D Channel.

};


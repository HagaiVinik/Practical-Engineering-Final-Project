/*====================================================================
* AUTHOR: Hagai Vinik	.	
* DATE: 16/05/19		.
/* copyright (c) 2018 Hagai Vinik , all rights reserved. */
/*====================================================================*/

/* include libraries */

#pragma once
#include <gclib.h>
#include <gcerr.h>
#include <dtilib.h>
#include <cclib.h>
#include <gcisdn.h>
#include <thread>
#include <cclib.h>
#include <gcpdkrt.h>
#include <ctime>
#include <dm3dtilib.h>
#include <dm3cc_parm.h>
#include <iostream>
#include <map>
#include <string>



/* define values */
#define T_MAX_NAMESTRING 50
#define MAXCHAN 30				/* max. number of channels in system. */
#define EXIT_CODE_SUCCESS 0		/* Exit Codes. */
#define EXIT_CODE_ERROR 1
#define LINES_CLOSED 0			/* flag for open/ close lines. */
#define LINES_OPEN 1


using namespace std;


/* Data structure which stores all information for each line. */
class LineBag
{
public:
	/* Local Variables: */
	LINEDEV m_ldev;			/* GlobalCall API line device handle */
	CRN m_crn;  
	int m_state;				/* GlobalCall API call handle */
	int m_blocked;			/* channel blocked/unblocked */
	int m_networkh;			/* network handle */
	int m_voiceh;				/* voice handle */
	
	/* Constructor/Destructor: */
	//LineBag();
	//~LineBag();
};
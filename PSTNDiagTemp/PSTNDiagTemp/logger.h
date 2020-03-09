/**************************************************************************/
/*   D I A G N O S E    E R R O R / R E S U L T    F U N C T I O N S  .

-- ResultInfo can be called anytime after a GlobalCall event has occurred
-- ErrorInfo prints the result information to the console with no other side effects
-- CreateFile and WriteToLog for saving events and errors
-- copyright (c) 2018 Hagai Vinik , all rights reserved.				*/
/**************************************************************************/


#pragma once
#include "linebag.h"
#include <time.h>
#include <fstream>
#include <cstdio>
#include "msg_mutex.h"


class Logger
{
public:
	Logger();
	~Logger();

	string nameOfFile;
	ofstream myLogFile ;

	/*  diagnose functions */
	string  datetime();
	string currenttime();
	void createFile();													/* create file to write log into it. */
	void  PrintResultInfo(METAEVENT *a_metaeventp);						/* print result of global call event. */
	void  PrintErrorInfo();												/* print value and detail of global call error event. */
	void  print_linebag(LineBag *pline);								/* print linebag values. */
	void  writeToLog(string log_msg);									/* write string to file. */
};
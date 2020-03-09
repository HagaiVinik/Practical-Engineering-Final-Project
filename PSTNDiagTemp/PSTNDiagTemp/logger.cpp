#include "Logger.h"


Logger :: Logger()
{
}

Logger :: ~Logger()
{
}

using namespace std;

void Logger:: PrintResultInfo(METAEVENT *a_metaeventp)
{
	int retCode;
    GC_INFO t_Info;  //struct of details about the event.
    // printf("trying to identify result.....\n");
    retCode = gc_ResultInfo(a_metaeventp, &t_Info);
    if (retCode == GC_SUCCESS)
    {
    
        printf("gc_ResultInfo() successfully called\n");  
        printf("event_Info: gcValue = 0x%x\n", t_Info.gcValue);
        printf("event_Info: gcMsg = %s\n", t_Info.gcMsg);
        printf("event_Info: ccLibId = %d\n", t_Info.ccLibId);
        printf("event_Info: ccLibName = %s\n", t_Info.ccLibName);
        printf("event_Info: ccValue = 0x%x\n", t_Info.ccValue);
        printf("event_Info: ccMsg = %s\n", t_Info.ccMsg);
        printf("event_Info: additionalInfo = %s\n", t_Info.additionalInfo);    
    }
    else 
    {
        printf("gc_ResultInfo() call failed\n");
        return;
    }
    printf("*** NOTICE: ");
    switch(t_Info.gcValue)
    {
		case GCRV_TIMEOUT:
			printf("GCRV_TIMEOUT - event caused by timeout.\n");
            break;
        case GCRV_RYEL:
            printf("GCRV_RYEL - Received yellow alarm \n");
            break;       
        case GCRV_RED:
            printf("GCRV_RED - Got a read alarm \n");
            break;
        case GCRV_ALARM:
            printf("GCRV_ALARM - event caused by alarm \n");         
            break;
        case GCRV_REDOK:
            printf("GCRV_REDOK -  Got a read alarm condition recovered \n");
            break;        
        case GCRV_RYELOK:
            printf("GCRV_RYELOK -  Got a yellow alarm recovered \n");
            break;
        /* didnt achived the results below. */
        case GCRV_NORMAL:
            printf("GCRV_NORMAL -  normal state . \n");
            break;
        case GCRV_SIGNALLING:
            printf("GCRV_SIGNALLING - Signaling change\n");
            break;
        case GCRV_DCHAN_CFAOK:
            printf("GCRV_DCHAN_CFAOK - D Channel out of srvice recovered.\n");
            break;
        case GCRV_CRC_CFAOK:
            printf("GCRV_CRC_CFAOK - time slot 16 is crc failure recovered\n");
            break;
        case GCRV_CCLIBSPECIFIC:
            printf("GCRV_CCLIBSPECIFIC - event caused by cclib specific failure\n");
            break;
        default:
           printf ("Unknown GlobalCall result code received, event_Info: gcValue = 0x%x\n",t_Info.gcValue);
		   break;
    }
    return;
}

void Logger :: PrintErrorInfo()
{
	printf("trying to identify error.....\n");
    int retCode;
    GC_INFO t_Info;
    retCode = gc_ErrorInfo(&t_Info);
    if (retCode == GC_SUCCESS)
    {
        printf("gc_ErrorInfo() successfully called, error identified .\n");
        printf("event_error: gcValue = 0x%x\n", t_Info.gcValue);
        printf("event_error: gcMsg = %s\n", t_Info.gcMsg);
        printf("event_error: ccLibId = %d\n", t_Info.ccLibId);
        printf("event_error: ccLibName = %s\n", t_Info.ccLibName);
        printf("event_error: ccValue = 0x%x\n", t_Info.ccValue);
        printf("event_error: ccMsg = %s\n", t_Info.ccMsg);
        printf("event_error: additionalInfo = %s\n", t_Info.additionalInfo);    
    } 
    else 
    {
        printf("gc_ErrorInfo() call failed\n");
        return;
    }
    printf("*** NOTICE: ");
    switch(t_Info.gcValue)
    {
        case EGC_ALARM:
            printf("EGC_ALARM - error alarm received.\n");
            break;       
        case EGC_NOT_ENABLED:
            printf("EGC_NOT_ENABLED - not enabled.\n");
            break;
        case EGC_INVLINEDEV:
            printf("EGC_INVLINEDEV - Invalid line device passed.\n");
            break;
        case EGC_INVCRN:
            printf("EGC_INVCRN - Invalid call reference number.\n");
            break;
        case EGC_INVPARM:
            printf("EGC_INVPARM - Invalid parameter(argument).\n");
            break;
        case EGC_SYSTEM:
            printf("EGC_SYSTEM -  System error.\n");
            break;
		case EGC_INVSTATE:
			printf("EGC_INVSTATE -  invalid state.\n");
			break;
		case EGC_ILLSTATE:
			printf("EGC_ILLSTATE -  function not suppurted in current state.\n");
			break;
		case EGC_CCLIBSPECIFIC:
			printf("EGC_CCLIBSPECIFIC -  cclib catchhall.\n");
			break;

		default:
            printf("Unknown GlobalCall error code received.\n");
		    break;
    }
    return;
}

void Logger :: print_linebag(LineBag *pline)
{
	    printf("linebag at %d values:\nlinedev = %d\ncrn = %d\nstate = %d\nblocked = %d\networkh = %d\nvoiceh = %d\n" ,
    pline, pline->m_ldev, pline->m_crn, pline->m_state, pline->m_blocked, pline->m_networkh, pline->m_voiceh );
}

string Logger :: datetime()
{
	
	char buffer[50];

	time_t curr_time ;
	tm * curr_time_ptr;
	
	time(&curr_time);
	curr_time_ptr = localtime(&curr_time);
	
	strftime(buffer, sizeof(buffer), "%d_%m_%y____%H_%M_%S", curr_time_ptr);
	return std::string(buffer);
}

string Logger :: currenttime()
{
	
	char buffer[50];

	time_t curr_time ;
	tm * curr_time_ptr;
	
	time(&curr_time);
	curr_time_ptr = localtime(&curr_time);
	
	strftime(buffer, sizeof(buffer), "%H:%M:%S", curr_time_ptr);
	return std::string(buffer);
}

void Logger :: createFile()
{
	nameOfFile = datetime() + ".txt";
	mtx_file.lock();
	FILE *f_ptr = fopen(nameOfFile.data(), "w");
	fclose(f_ptr);
	mtx_file.unlock();
}

void Logger :: writeToLog(string log_msg)
{
	string msg_for_write = currenttime() + ":: " + log_msg +"\n";
	std::cout << "Writing to File: '" + msg_for_write + "'";
	mtx_file.lock();
	FILE *f_ptr = fopen(nameOfFile.data(),"a");
	fputs(msg_for_write.data(), f_ptr);
	fclose(f_ptr);
	mtx_file.unlock();
}


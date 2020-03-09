#include "trunk.h"


Trunk :: Trunk()
{

}

Trunk :: ~Trunk()
{

}

int Trunk :: openD_Channel(int index)
{
	
    char bdevname[40]; /* Board device name */
    //int type; /* Type of line device */
    int statebuf = 0; /* Buffer to store line device state */
	sprintf(bdevname, ":N_dtiB%d:P_isdn",index);

	if(gc_OpenEx(&D_channel, bdevname, EV_ASYNC, &D_channel) != GC_SUCCESS)
	{
		//Logger :: PrintErrorInfo();
	} 

	printf("success ** opened borard\n");
	return EXIT_CODE_SUCCESS;
}

int Trunk :: openLineBags(int index)
{
	int counter = 0;
    int mode = EV_ASYNC;   // async mode!
    char devname[64];     //1 - 16 slots.
    int ts= 1;            //time slot.
    int vch = 0;          
    int vbnum = 0; 
    int linebag_index = 0;   // linebag num.
    for (ts = 1,linebag_index = 1; ts <= MAXCHAN ; ts++,linebag_index++)
    {
		vbnum = (ts - 1) / 4 + 1;
		vch = ((ts - 1) % 4) + 1;
		sprintf (devname, ":N_dtiB%dT%d:P_isdn:V_dxxxB%dC%d", index, ts, vbnum, vch);          
        if (gc_OpenEx(&m_lineBagArr[linebag_index].m_ldev, devname, EV_SYNC,(void *)&m_lineBagArr[linebag_index]) != GC_SUCCESS) 
        {
			std::cout << "*****ERROR1: error opening - couldnt open line devices.*****" << std::endl;
//			Logger :: PrintErrorInfo();
			return EXIT_CODE_ERROR;
		}
		counter++;
	}
	std::cout << "counter = " << counter << std::endl;
	std::cout << "open_line_devices completed." << std::endl;
	return EXIT_CODE_SUCCESS;
}

int Trunk :: closeAllLineBags()
{
	int port_index; /* port index */
	int error = -1; /* reason for failure of function */
	for (port_index = 1; port_index <= MAXCHAN; port_index++) 
    { 
		if (gc_Close(m_lineBagArr[port_index].m_ldev) != GC_SUCCESS) 
        {
			std::cout << "*** ERROR2.1: error closing lines in port 1 couldnt close lines. ***" << std::endl;
            /* process error return as shown */
//            Logger :: PrintErrorInfo();
            break;
		}
	}
	std::cout << "\n\n--- closed line devices completed... ---\n" << std::endl;
	return EXIT_CODE_SUCCESS;
}

int Trunk :: waitForCall()
{
	for(int i = 1; i <= 30; i++)
	{
		if (gc_WaitCall(m_lineBagArr[i].m_ldev, NULL, NULL, 0, EV_ASYNC) != GC_SUCCESS)
		{
			std::cout << "ERROR: unable to set Line "<< i << "to wait for call " << std::endl;
		}
	}
	std::cout << "SUCCESS: wait call for port 1 and 2 is initailized.\n" << std::endl;
	return EXIT_CODE_SUCCESS;
}

int Trunk :: makeCall(int LineBagIndex, char *adress)
{
    /* Make a call to the number 993-3000. */
	if (gc_MakeCall(m_lineBagArr[LineBagIndex].m_ldev,&m_lineBagArr[LineBagIndex].m_crn, adress, NULL, 0, EV_ASYNC) == GC_SUCCESS) 
    {
        /* Call successfully connected; continue processing */
		printf("\n\n=====  SUCCESS: make call succeed.  =======\n\n");
    }
    else 
    {
    /* process error return as shown */
		printf("\n\n=====  FAILED: make call Failed.  =======\n\n");
//        Logger :: PrintErrorInfo();
        return EXIT_CODE_ERROR;
    }
    /*
    * Application may now wait for an event to indicate call
    * completion.
    */
    return EXIT_CODE_SUCCESS;
}

int Trunk :: getDchannelState()  /* Find the status of the line. */
{
    int type; /* Type of line device */
    int statebuf = 0; /* Buffer to store line device state */
    type = GCGLS_DCHANNEL;
	if(gc_GetLinedevState(D_channel, type, &statebuf) != GC_SUCCESS)
    {
            cout << "\n\nget_line_dev_state error 3: gc_GetLinedevState failed.\n\n" << endl;
    }
    else
    {
        switch (statebuf)
        {
            case DATA_LINK_DOWN:
				cout << "D Channel Status: DATA_LINK_DOWN" << endl;
                break;
            case DATA_LINK_UP:
                cout << "D Channel Status: DATA_LINK_UP" << endl;
                break;
            default:
                cout<< "D Channel Status: " << statebuf << endl;
                break;
        }         
    }
	return statebuf;

}

int Trunk :: getStatusOfBchannel(LINEDEV ldev)
{
	int type; /* Type of line device */
    int statebuf = 0; /* Buffer to store line device state */
    type = GCGLS_BCHANNEL;
    if(gc_GetLinedevState(ldev, type, &statebuf) != GC_SUCCESS)
    {
            printf("\n\nget_line_dev_state error 4: gc_GetLinedevState failed.\n\n");
    }
    else
    {
    switch (statebuf)
    {
        case GCLS_INSERVICE:
            printf("B Channel Status: GCLS_INSERVICE\n");
            break;
        case GCLS_MAINTENANCE:
            printf("B Channel Status: GCLS_MAINTENANCE\n");
            break;
        case GCLS_OUT_OF_SERVICE:
            printf("B Channel Status: GCLS_OUT_OF_SERVICE\n");
            break;
        default:
            printf("B Channel Status: %d\n", statebuf);
            break;
     }          
    }
    //gc_Close(bdd);
    //gc_Close(ldev);
    return EXIT_CODE_SUCCESS;
}

int Trunk :: closeOneLineBag(int timeSlot)
{
	if (gc_Close(m_lineBagArr[timeSlot].m_ldev) != GC_SUCCESS) 
        {
			std::cout << "*** ERROR2.1: error closing lines in port 1 couldnt close lines. ***" << std::endl;
            /* process error return as shown */
//            Logger :: PrintErrorInfo();
			return EXIT_CODE_ERROR;
		}
	std::cout << "NOTE: LINDEV TIMESLOT:" << timeSlot << " Closed" << std::endl;
	return EXIT_CODE_SUCCESS;
}

int Trunk :: openOneLineBag(int trunk ,int ts)
{
	char devname[64];     //1 - 16 slots.
	int vch = 0;          
    int vbnum = 0;
	vbnum = (ts - 1) / 4 + 1;
	vch = ((ts - 1) % 4) + 1;
	sprintf (devname, ":N_dtiB%dT%d:P_isdn:V_dxxxB%dC%d", trunk, ts, vbnum, vch);
	if (gc_OpenEx(&m_lineBagArr[ts].m_ldev, devname, EV_SYNC,(void *)&m_lineBagArr[ts]) != GC_SUCCESS) 
        {
			std::cout << "*****ERROR1: error opening - couldnt open line devices.*****" << std::endl;
//			Logger :: PrintErrorInfo();
			return EXIT_CODE_ERROR;
		}
	std::cout << "NOTE: LINDEV TIMESLOT:" << ts << " Opened" << std::endl;
	WaitForCallOnSpecificTS(ts);
	return EXIT_CODE_SUCCESS;
}

int Trunk :: WaitForCallOnSpecificTS(int ts)
{
	if (gc_WaitCall(m_lineBagArr[ts].m_ldev, NULL, NULL, 0, EV_ASYNC) != GC_SUCCESS)
	{
		std::cout << "ERROR: unable to set Line "<< ts << "to wait for call " << std::endl;
		return EXIT_CODE_ERROR;
	}
	std::cout << "waiting for call on specific ts(" << ts << ")....." << std::endl;
	return EXIT_CODE_SUCCESS;
}
/********************************************************************
*
*			D I A G N O S I S	A N D	A N A L Y S I S
*				T E L E P H O N Y	S Y S T E N
*
*********************************************************************
*
*====================================================================
*
* AUTHOR: Hagai Vinik	.	
* DATE: 16/05/19		.	
* DESCRIPTION: This software is used to analyze the DNI board system,
*              user can control the global call commands and services.
*
* SOURCE: copyright (c) 2018 Hagai Vinik , all rights reserved.
*
====================================================================*/


#pragma once
#include "board.h"
#include <string>



void check_message(Board *b, Communication *myServer)
{
	
	std::string current_str = "";
	while(true)
	{
		if(!b->vec_msgs.empty())
		{
			mtx_message.lock();	
			Comm_mtx_msg.lock();
			myServer->server_msgs.push( b->vec_msgs.front());
			b->vec_msgs.pop();
			Comm_mtx_msg.unlock();
			mtx_message.unlock();
		}
	}
}

void check_message_from_client(Board *b, Communication *serverRecieveMsg)
{
	std::string current_msg;
	size_t pos = 0;
	while(true)
	{
		if (serverRecieveMsg->msg_from_client != "")
		{
			current_msg = serverRecieveMsg->msg_from_client;
			b->log.writeToLog("===== Command from client:   " + current_msg + "   ====="); // Mutex inside.
			pos = current_msg.find(":");
			std::string f_event = current_msg.substr(0,pos);
			current_msg.erase(0,pos + 1);
			pos = current_msg.find(":");
			std::string  f_trunk = current_msg.substr(0,pos);
			current_msg.erase(0,pos + 1);
			pos = current_msg.find(":");
			std::string  f_timeSlot = current_msg.substr(0,pos);
			
			std::cout << "f_event =" <<f_event << std::endl;
			std::cout << "f_trunk =" <<f_trunk << std::endl;
			std::cout << "f_timeSlot =" <<f_timeSlot << std::endl;
			//std::cout << "event" <<f_event
			if(f_event == "Unblock")
			{
				b->openSpecificTimeSlot(std::stoi(f_trunk),std::stoi(f_timeSlot));
			}
		
			else if(f_event == "Block")
			{				
				b->closeSpecificTimeSlot(std::stoi(f_trunk),std::stoi(f_timeSlot));
			}

			else if(f_event == "Resume")
			{
				b->resumeCall(std::stoi(f_trunk),std::stoi(f_timeSlot));
			}

			else if(f_event == "Hold")
			{
				b->holdCall(std::stoi(f_trunk),std::stoi(f_timeSlot));
			}

			else if(f_event == "CloseAll")
			{
				b->closeTrunks();
			}

			else if(f_event == "OpenAll")
			{
				b->openTrunks();
				b->waitCallInTrunks();
			}

			else if(f_event == "Disconnect")
			{
				b->forceCloseCall(std::stoi(f_trunk),std::stoi(f_timeSlot));
			}

			else if(f_event == "Reset")
			{
				b->closeTrunks();
				b->openTrunks();
				b->waitCallInTrunks();
			}

			else
				std::cout<< "Warning: msg does not match protocol! " << std::endl;

			Comm_mtx_msg_to_server.lock();
			serverRecieveMsg->msg_from_client = "";
			Comm_mtx_msg_to_server.unlock();

		}
	}
}

int main()
{
	/* Variables: */
	Board b;
	Communication myServer;
	Communication serverRecieveMsg;

	/* Code: */
	std::cout << "Starting program......" << std::endl;	
	myServer.start_server("5000");  /*	NOTE: BLOCKING	*/	
	
	b.log.createFile();			// create a file if client connected.
	
	thread t1(check_message,&b,&myServer);
	t1.detach();
	
	b.startGClib();		// init: start global call library, init handler and open trunks..
	b.initHandler();	
	b.openTrunksDchannel();
	b.openTrunks();
	b.waitCallInTrunks();	// wait for call in all timeslots.

	thread t2(check_message_from_client, &b,&serverRecieveMsg);
	t2.detach();
	
	serverRecieveMsg.start_server("6000"); /* NOTE: BLOCKING , Keep Program alive.*/
}


/*====================================================================
* AUTHOR: Hagai Vinik	.	
* DATE: 16/05/19		.	
* DESCRIPTION: This program creates instance of server for sending or  
*			   recieving data from client that connects	. 
* SOURCE: copyright (c) 2018 Hagai Vinik , all rights reserved.
======================================================================*/

#pragma once 
#include <WS2tcpip.h>
#include <WinSock2.h>
#include <Windows.h>
#include <iostream>
#include <thread>
#include <string>
#pragma comment(lib, "ws2_32.lib")
#include "msg_mutex.h"
#include <queue>


class Communication
{
public:
	Communication()							;
	~Communication()						;

	SOCKET clientSocket                     ;
	sockaddr_in client                      ;
	int clientSize                          ;

	std::string msg_from_client				;
	std::queue <std::string> server_msgs    ;

	void handle_Msg_client()				;
	void start_server(char *port)			;
	void handle_socket()					;
	void send_data(char *message)			;
private:

};


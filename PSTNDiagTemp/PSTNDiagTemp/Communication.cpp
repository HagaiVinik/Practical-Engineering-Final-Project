#include "Communication.h"


Communication:: Communication()
{	
	msg_from_client = "";
}

Communication:: ~Communication()
{
}

void Communication:: start_server(char *port)
{
	
	/* Variables: */
	int result            =               0 ;
	char buffer[1024]     =             {0} ;
	sockaddr_in address                     ;
	sockaddr_in *response                   ;
	addrinfo *result_data =            NULL ;
	addrinfo hints                          ;
	int addrlen           = sizeof(address) ;
	SOCKET listening                        ;
	WSADATA wasData                         ;
	WORD ver              =   MAKEWORD(2,2) ;
	int wSok    = WSAStartup(ver, &wasData) ;	

	/* Code: */
	std::cout << "starting SOCKET SERVER......" << std::endl;
	if(wSok != 0)
	{
		std::cerr << "cant Initialize socket, exiting....." << std::endl;
		return;
	}
	// Setting server: //
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
	result = getaddrinfo(NULL, port , &hints, &result_data);
	if ( result != 0 )
	{
		std::cerr << "getaddrinfo failed..." << std::endl;
	}
	listening = socket(result_data->ai_family, result_data->ai_socktype, result_data->ai_protocol);
	if(listening == INVALID_SOCKET)
	{
		std::cerr << "cant create a socket, exiting....." << std::endl;
		return;
	}
	// Binding: //
	result = bind(listening, result_data->ai_addr, (int)result_data->ai_addrlen);
	if ( result == -1 )
	{
		std::cerr << "failed binding, couldn't listen....." << std::endl;
		return;
	}
	freeaddrinfo(result_data);
	result = listen(listening, SOMAXCONN);
	if ( result == -1 )
	{
		std::cerr << "couldn't listen....." << std::endl;
		return;
	}	
	// Waiting for client to connect - CRITICAL CODE - BLOCKING
	clientSize = sizeof(client);
	while (true)
	{
		clientSocket = accept(listening,NULL,NULL);
		if(port == "6000" && clientSocket != SOCKET_ERROR)
		{
			handle_Msg_client();
		}
		if( port == "5000" && clientSocket != SOCKET_ERROR )
		{
			break;
		}
		Sleep(1000);
	}
	
	std::cout << "Client connected" << std::endl;
	if(port == "5000")
	{
		closesocket(listening);
		std::thread handleThread(&Communication::handle_socket, this);
		handleThread.detach();
	}
	//if(port == "6000")
	//{
	//	std::thread handleThread(&Communication::handle_Msg_client, this);
	//	handleThread.detach();
	//}
	
}

void Communication:: handle_socket()
{
	std::cout << "Handling socket" << std::endl;
	std::string current_msg = "";
	while(true)
	{
		if (!server_msgs.empty())
		{
			Comm_mtx_msg.lock();
			std::cout << "------ message to client is : " << server_msgs.front() <<" ---------" << std::endl;
			send(clientSocket,server_msgs.front().data(),strlen(server_msgs.front().data()),0);
			server_msgs.pop();
			Comm_mtx_msg.unlock();
		}
	}
}

void Communication:: handle_Msg_client()
{
	char buffer[128] = {0};
	std::cout << "Handling socket msg:" << std::endl;
	int result = recv(clientSocket,buffer,sizeof(buffer),0);
	if( result > 0 )
	{
		std::cout << "message from client is - " << buffer << std::endl;
		Comm_mtx_msg_to_server.lock();
		msg_from_client = buffer;
		Comm_mtx_msg_to_server.unlock();
	}
	std::cout << "Client Disconnected... "  << std::endl;
	closesocket(clientSocket);
	//WSACleanup();
}
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#include <string>

#include "../tppSocketPOSIX.h"

#define DEFAULT_BUFLEN 512

int main(void)
{
	WSADATA wsaData;

	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	// Initialize Winsock
	{
		int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

		if (iResult == 0)
		{
			printf("WSAStartup initialized correctly\n");
		}
		else
		{
			printf("WSAStartup failed with error: %d\n", iResult);
			return 1;
		}
	}

	tpp::NetworkAddress address("127.0.0.1", 27001);
	tpp::SocketPOSIX serverSocket;

	serverSocket.Create();
	serverSocket.Listen(address.port);
	tpp::ISocket* clientSocket = serverSocket.Accept(address);
	serverSocket.Close();

	clientSocket->SetTimeout(tpp::Channel::Both, 1000);

	bool shutdown = false;

	while(!shutdown)
	{
		tpp::SocketReturn::T receiveResult = clientSocket->Receive(recvbuf, recvbuflen);

		if (receiveResult > 0)
		{
			std::string message(recvbuf, receiveResult);

			tpp::SocketReturn::T sendResult;

			if (message.compare("vec3") == 0)
			{
				std::string vec3str = "{ 0.0, 1.0, 2.0 }";
				sendResult = clientSocket->Send(vec3str.c_str(), vec3str.length());
			}
			else if (message.compare("bool") == 0)
			{
				std::string boolstr = "true";
				sendResult = clientSocket->Send(boolstr.c_str(), boolstr.length());
			}
			else if (message.compare("int") == 0)
			{
				std::string intstr = "2";
				sendResult = clientSocket->Send(intstr.c_str(), intstr.length());
			}
			else
			{
				std::string unrecognizedMsg = "Unrecognized message";
				sendResult = clientSocket->Send(unrecognizedMsg.c_str(), unrecognizedMsg.length());
			}
		}
		else if (receiveResult == tpp::SocketReturn::Timeout)
		{

		}
		else
		{
			shutdown = true;
		}
	}

	clientSocket->Close();

	WSACleanup();

	printf("Server closed succesfully\n");

	getchar();

	return 0;
}
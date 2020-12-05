#define WIN32_LEAN_AND_MEAN

#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <string>

#include "tppNetwork.h"
#include "tppISocket.h"

int main(int argc, char **argv)
{
	static const int DEFAULT_BUFLEN = 512;

	char recvbuf[DEFAULT_BUFLEN] = {};
	int recvbuflen = DEFAULT_BUFLEN;

	tpp::Network::Initialize();

	tpp::NetworkAddress address("127.0.0.1", 27001);
	tpp::ISocket* clientSocket = tpp::Network::CreateSocket();
	clientSocket->Create();
	clientSocket->SetBlocking(false);

	bool shutdown = false;

	while (!shutdown)
	{
		if (clientSocket->IsConnected())
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
			else if (receiveResult == 0)
			{
				printf("Connection closed\n");
			}
			else if (receiveResult == tpp::SocketReturn::WouldBlock)
			{
			
			}
			else if (receiveResult < 0)
			{
				//wchar_t errorString[256] = {};
				//int size = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, WSAGetLastError(), 0, (LPWSTR)&errorString, sizeof(errorString), NULL);
				//
				//printf("recv failed with error: %S\n", errorString);
			}
		}
		else
		{
			tpp::SocketReturn::T connectReturn = clientSocket->Connect(address);
			//if (connectReturn == tpp::SocketReturn::Ok)
			//{
			//	clientSocket->SetBlocking(false);
			//}
		}
	}

	tpp::Network::Deinitialize();

	printf("Client closed successfully\n");

	getchar();

	return 0;
}
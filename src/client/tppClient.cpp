#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#include <vector>
#include <string>

#include "../tppSocketPOSIX.h"

#define DEFAULT_BUFLEN 512

int main(int argc, char **argv)
{
	WSADATA wsaData;
	char *sendbuf = "this is a test";
	char recvbuf[DEFAULT_BUFLEN] = {};
	int recvbuflen = DEFAULT_BUFLEN;

	const char* serverName = "192.168.0.11";

	// Initialize Winsock
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

	tpp::NetworkAddress address("127.0.0.1", 27001);
	tpp::SocketPOSIX* clientSocket = new tpp::SocketPOSIX();
	clientSocket->Create();
	clientSocket->SetBlocking(false);

	std::vector<std::string> messages;

	messages.push_back("vec3");
	messages.push_back("bool");
	messages.push_back("int");

	bool shutdown = false;

	while (!shutdown)
	{
		if (clientSocket->IsConnected())
		{
			if (!messages.empty())
			{
				const std::string& message = messages.back();
				tpp::SocketReturn::T sendResult = clientSocket->Send(message.c_str(), message.length());
				// TODO Handle send issues
				messages.pop_back();
			}

			tpp::SocketReturn::T receiveResult = clientSocket->Receive(recvbuf, recvbuflen);

			if (receiveResult == 0)
			{
				printf("Connection closed\n");
			}
			else if (receiveResult == tpp::SocketReturn::WouldBlock)
			{
			
			}
			else if (receiveResult < 0)
			{
				wchar_t errorString[256] = {};
				int size = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, WSAGetLastError(), 0, (LPWSTR)&errorString, sizeof(errorString), NULL);

				printf("recv failed with error: %S\n", errorString);
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

	WSACleanup();

	printf("Client closed successfully\n");

	getchar();

	return 0;
}
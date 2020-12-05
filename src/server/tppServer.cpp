#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <vector>
#include <chrono>
#include <thread>

#include <string>

#include "tppNetwork.h"
#include "tppISocket.h"

int main(void)
{
	static const int DEFAULT_BUFLEN = 512;

	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	tpp::Network::Initialize();

	tpp::NetworkAddress address("127.0.0.1", 27001);
	tpp::ISocket* serverSocket = tpp::Network::CreateSocket();
	tpp::ISocket* clientSocket = tpp::Network::CreateSocket();

	serverSocket->Create();
	serverSocket->SetBlocking(false);
	serverSocket->Listen(address.port);

	std::vector<std::string> messages;

	bool shutdown = false;

	while(!shutdown)
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

			if (receiveResult > 0)
			{
				
			}
			else if (receiveResult == tpp::SocketReturn::Timeout || receiveResult == tpp::SocketReturn::WouldBlock)
			{
				
			}
			else
			{
				clientSocket->Close();
			}
		}
		else
		{
			time_t currentTime = time(NULL);

			printf("Listening for connections... (%lli)\n", currentTime);
			tpp::SocketReturn::T acceptReturn = serverSocket->Accept(address, clientSocket);

			if (acceptReturn != tpp::SocketReturn::Ok)
			{
				serverSocket->Close();
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			}
			else
			{
				messages.push_back("vec3");
				messages.push_back("bool");
				messages.push_back("int");
			}
		}
	}

	clientSocket->Close();

	tpp::Network::Deinitialize();

	printf("Server closed successfully\n");

	getchar();

	return 0;
}
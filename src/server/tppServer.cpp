#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <vector>
#include <chrono>
#include <thread>

#include <string>

#include "tppNetwork.h"
#include "tppISocket.h"

// Example
// tpp33path20Rendering/SSR/Number of Raystypefloatvalue[f]

std::vector<char> PrepareMessage1()
{
	std::string messageHeader = "tpp";
	std::string pathString = u8"path";
	std::string path = u8"Rendering/SSR/Number of Rays";
	std::string typeString = "type";
	std::string type = "float";
	std::string valueString = "value";

	std::vector<char> fullPacket;
	
	// 1 Reserve space in the packet for tpp and the size of the data
	const char dummySize[4] = {};
	fullPacket.insert(fullPacket.end(), messageHeader.begin(), messageHeader.end());
	fullPacket.insert(fullPacket.end(), dummySize, dummySize + 4);

	// 2 Populate all the packet data after that
	fullPacket.insert(fullPacket.end(), pathString.begin(), pathString.end()); // Add path
	fullPacket.insert(fullPacket.end(), path.begin(), path.end() + 1);

	fullPacket.insert(fullPacket.end(), typeString.begin(), typeString.end()); // Add type
	fullPacket.insert(fullPacket.end(), type.begin(), type.end() + 1);

	float value = 16.0;
	fullPacket.insert(fullPacket.end(), valueString.begin(), valueString.end()); // Add value
	fullPacket.insert(fullPacket.end(), reinterpret_cast<char*>(&value), reinterpret_cast<char*>(&value) + 4);

	// 3 Calculate size as size() - (length(tpp) + 4)
	// 4 Fill in data size for the entire packet
	size_t totalDataSize = fullPacket.size() - messageHeader.size() - 4;
	const char* totalDataSizeChar = reinterpret_cast<const char*>(&totalDataSize);
	std::copy(totalDataSizeChar, totalDataSizeChar + 4, fullPacket.data() + messageHeader.size());

	return fullPacket;
}

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

	std::vector<std::vector<char>> messages;

	bool shutdown = false;

	while(!shutdown)
	{
		if (clientSocket->IsConnected())
		{
			if (!messages.empty())
			{
				const std::vector<char>& message = messages.back();
				tpp::SocketReturn::T sendResult = clientSocket->Send(message.data(), message.size());
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
				messages.push_back(PrepareMessage1());
			}
		}
	}

	clientSocket->Close();

	tpp::Network::Shutdown();

	printf("Server closed successfully\n");

	getchar();

	return 0;
}
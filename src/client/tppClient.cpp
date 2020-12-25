#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <algorithm>

#include "tppNetwork.h"
#include "tppISocket.h"
#include "tppTypes.h"

tpp::Float myFloat("Rendering/SSR/Number of Rays", 8.0f, 1.0f, 64.0f, 1.0f);

bool IsCopyable(tpp::VariableType type)
{
	switch (type)
	{
		case tpp::VariableType::Function:
			return false;
	}

	return true;
}

int main(int argc, char **argv)
{
	static const int DEFAULT_BUFLEN = 512;

	char receiveBuffer[DEFAULT_BUFLEN] = {};
	int receiveBufferLength = DEFAULT_BUFLEN;

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
			tpp::SocketReturn::T receiveResult = clientSocket->Receive(receiveBuffer, receiveBufferLength);

			// If we have received valid data
			if (receiveResult > 0)
			{
				// Copy all the data into a vector
				// TODO Use the original buffer instead
				std::vector<char> receivedData(receiveBuffer, receiveBuffer + receiveResult);

				// Search for the first appearance of the header
				auto headerPosition = std::search(receivedData.begin(), receivedData.end(), tpp::HeaderString, tpp::HeaderString + strlen(tpp::HeaderString));

				// While we have headers (there could be multiple messages in the data) process them
				while (headerPosition != receivedData.end())
				{
					// Cast the start of the message to the header, and extract relevant information
					tpp::MessageHeader* header = reinterpret_cast<tpp::MessageHeader*>(receivedData.data());
					tpp::MessageType messageType = header->messageType;
					tpp::Version version = header->version;
					uint32_t packetSize = header->messageSize;

					// Find where the header starts, and copy the data onward
					auto index = headerPosition + sizeof(tpp::MessageHeader) - receivedData.begin();

					std::vector<char> packetData;
					packetData.reserve(packetSize);
					packetData.insert(packetData.end(), &receivedData[index], &receivedData[index] + packetSize);

					// Search for path in packet and read path
					std::string path;
					{
						auto pathPosition = std::search(packetData.begin(), packetData.end(), tpp::PathString, tpp::PathString + strlen(tpp::PathString));
						auto nullTerminator = std::find(pathPosition, packetData.end(), '\0');
						path = std::string(pathPosition + strlen(tpp::PathString), nullTerminator);
					}

					// Use the type to read in the value
					{
						tpp::Data variableData = tpp::GlobalVariableManager->Find(path);

						auto variablePosition = std::search(packetData.begin(), packetData.end(), tpp::VariableString, tpp::VariableString + strlen(tpp::VariableString));

						if (variablePosition != packetData.end() && variableData.memory != nullptr)
						{
							auto valueIndex = variablePosition - packetData.begin();
							tpp::VariableHeader* variablePacket = reinterpret_cast<tpp::VariableHeader*>(&packetData[valueIndex]);

							auto variableIndex = valueIndex + sizeof(tpp::VariableHeader);

							if (IsCopyable(variablePacket->type))
							{
								memcpy(variableData.memory, &packetData[variableIndex], variablePacket->size);
							}
						}
					}

					if (myFloat == 16.0f)
					{
						printf("Wow!\n");
					}

					auto nextHeaderPosition = std::search(headerPosition + 1, receivedData.end(), tpp::HeaderString, tpp::HeaderString + strlen(tpp::HeaderString));

					headerPosition = nextHeaderPosition;
				}

				tpp::SocketReturn::T sendResult;
				std::string receivedMsg = "Received a message from server";
				sendResult = clientSocket->Send(receivedMsg.c_str(), receivedMsg.length());
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

	tpp::Network::Shutdown();

	printf("Client closed successfully\n");

	return 0;
}
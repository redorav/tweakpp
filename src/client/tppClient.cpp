#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <algorithm>

#include "tppNetwork.h"
#include "tppISocket.h"
#include "tppTypes.h"

tpp::Float myFloat("Rendering/SSR/Number of Rays", 8.0f, 1.0f, 64.0f, 1.0f);

enum class VariableType
{
	Float,
	UnsignedInteger,
	Integer,
	Color3,
	Color4,
	Bool,
	Invalid
};

const char* GetTypeString(VariableType type)
{
	switch (type)
	{
		case VariableType::Float: return "float32";
		case VariableType::UnsignedInteger: return "uint32";
		case VariableType::Integer: return "int32";
		case VariableType::Color3: return "color3";
	}

	return "";
}

VariableType GetType(const std::string& type)
{
	if (type == "float")
	{
		return VariableType::Float;
	}
	else if (type == "uint32")
	{
		return VariableType::UnsignedInteger;
	}
	else if (type == "int32")
	{
		return VariableType::Integer;
	}
	else if (type == "color3")
	{
		return VariableType::Color3;
	}
	else
	{
		return VariableType::Invalid;
	}
}

// Command encodes all the necessary information to update a variable on the client's side
struct UpdateCommand
{
	std::string path;
	VariableType type = VariableType::Float;
	union
	{
		float f32 = 0.0f;
		uint32_t u32;
		int32_t i32;
	};
};

void PrintCommandDescription(const UpdateCommand& updateCommand)
{
	printf("Path: %s\n", updateCommand.path.c_str());
	printf("Type: %s\n", GetTypeString(updateCommand.type));
	printf("Value: %f\n", updateCommand.f32);
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
					// Compute the offset into the raw packet data to get the size of the packet
					// From there we can load the entire packet in to get the next one
					// Once we've got all relevant packets, we'll start parsing them for the interesting bits
					auto index = headerPosition + strlen(tpp::HeaderString) - receivedData.begin();
					uint32_t packetSize = *(uint32_t*)(&receivedData[index]);

					std::vector<char> packetData;
					packetData.reserve(packetSize);
					packetData.insert(packetData.end(), &receivedData[index] + 4, &receivedData[index] + 4 + packetSize);

					UpdateCommand command;

					// Search for path in packet and read path
					{
						auto pathPosition = std::search(packetData.begin(), packetData.end(), tpp::PathString, tpp::PathString + strlen(tpp::PathString));
						auto nullTerminator = std::find(pathPosition, packetData.end(), '\0');
						command.path = std::string(pathPosition + strlen(tpp::PathString), nullTerminator);
					}

					// Search for type in packet and read type
					// TODO Make this an enum instead of parse
					{
						auto typePosition = std::search(packetData.begin(), packetData.end(), tpp::TypeString, tpp::TypeString + strlen(tpp::TypeString));
						auto nullTerminator = std::find(typePosition, packetData.end(), '\0');
						std::string typeString = std::string(typePosition + strlen(tpp::TypeString), nullTerminator);
						command.type = GetType(typeString);
					}

					// Use the type to read in the value
					{
						tpp::Data variableData = tpp::GlobalVariableManager->Find(command.path);

						auto valuePosition = std::search(packetData.begin(), packetData.end(), tpp::ValueString, tpp::ValueString + strlen(tpp::ValueString));

						if (valuePosition != packetData.end() && variableData.memory != nullptr)
						{
							auto valueIndex = valuePosition + strlen(tpp::ValueString) - packetData.begin();
							char* data = &packetData[valueIndex];

							if (command.type == VariableType::Float ||
								command.type == VariableType::Integer ||
								command.type == VariableType::UnsignedInteger)
							{
								memcpy(variableData.memory, data, variableData.size);
							}
						}
					}

					if (myFloat == 16.0f)
					{
						printf("Wow!\n");
					}

					PrintCommandDescription(command);

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
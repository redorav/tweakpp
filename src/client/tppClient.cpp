#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <algorithm>

#include "tppNetwork.h"
#include "tppISocket.h"
#include "tppTypes.h"
#include "tppSerialize.h"

//---------
// EXAMPLES
//---------

// SSR
tpp::Float SSRNumberOfRays("Rendering/Post Effects/SSR1/Number of Rays", 8.0f, 1.0f, 64.0f, 1.0f);
tpp::Float SSRThicknessMultiplier("Rendering/Post Effects/SSR2/Thickness Multiplier", 1.0f, 1.0f, 2.0f, 0.001f);

// Depth of Field
tpp::Float DepthOfFieldAperture("Rendering/Post Effects/Depth of Field1/Aperture", 8.0f, 1.0f, 64.0f, 1.0f);
tpp::Float DepthOfFieldBokehSize("Rendering/Post Effects/Depth of Field2/Bokeh Size", 8.0f, 1.0f, 64.0f, 1.0f);

// TAA
tpp::Float TAAJitterX("Rendering/Post Effects/TAA1/TAA Jitter X", 8.0f, 1.0f, 64.0f, 1.0f);
tpp::Float TAAJitterY("Rendering/Post Effects/TAA2/TAA Jitter Y", 8.0f, 1.0f, 64.0f, 1.0f);

tpp::Float PerformanceGraphScaleX("Rendering/Performance1/Graph Scale X", 8.0f, 1.0f, 64.0f, 1.0f);
tpp::Float PerformanceGraphScaleY("Rendering/Performance2/Graph Scale Y", 8.0f, 1.0f, 64.0f, 1.0f);

tpp::Float CoreGraphScaleX("Core/Performance1/Graph Scale X", 8.0f, 1.0f, 64.0f, 1.0f);
tpp::Float CoreGraphScaleY("Core/Performance2/Graph Scale Y", 8.0f, 1.0f, 64.0f, 1.0f);

tpp::Float AnimationTimeScale("Animation1/Time Scale", 8.0f, 1.0f, 64.0f, 1.0f);
tpp::Float AnimationThreshold("Animation2/Threshold", 8.0f, 1.0f, 64.0f, 1.0f);

tpp::Float PhysicsTargetFPS("Physics1/Target FPS", 8.0f, 1.0f, 64.0f, 1.0f);
tpp::Float PhysicsFPSLimit("Physics2/Performance/FPS Limit", 8.0f, 1.0f, 64.0f, 1.0f);

tpp::Float DebugDisplayDeferredNormals("Rendering/Debug Display1/Deferred/Normals", 8.0f, 1.0f, 64.0f, 1.0f);
tpp::Float DebugDisplayForwardAlbedo("Rendering/Debug Display2/Forward/Albedo", 8.0f, 1.0f, 64.0f, 1.0f);


bool IsCopyable(tpp::VariableType type)
{
	switch (type)
	{
		case tpp::VariableType::Function:
			return false;
	}

	return true;
}

// HEADER
// tpp MessageType::Declaration 33 2 - Update message that is 33 bytes, version 2
//
// MESSAGE
// [Header][path]Rendering/SSR/Number of Rays\0 [var][floatType][initialValue][minValue][maxValue][step]

// TODO Take as input parameter
std::vector<char> PrepareVariableDescriptionTable()
{
	std::vector<char> fullPacket;

	tpp::GlobalClientVariableManager->ForEachVariable([&fullPacket](const std::string& path, const tpp::Variable& variable)
	{
		std::vector<char> localPacket;

		SerializeCommandHeader(localPacket, tpp::MessageType::Declaration);

		SerializePath(localPacket, path.c_str());

		SerializeFloat(localPacket, 16.0f);

		SerializeFloatProperty(localPacket, tpp::PropertyType::Min, 1.0f);

		SerializeFloatProperty(localPacket, tpp::PropertyType::Max, 64.0f);

		SerializeFloatProperty(localPacket, tpp::PropertyType::Step, 1.0f);

		// 3 Calculate message size and update packet
		size_t totalDataSize = localPacket.size() - sizeof(tpp::MessageHeader);
		tpp::MessageHeader* header = reinterpret_cast<tpp::MessageHeader*>(localPacket.data());
		header->messageSize = totalDataSize;

		// TODO Do this on the actual full packet
		fullPacket.insert(fullPacket.end(), localPacket.begin(), localPacket.end());
	});

	return fullPacket;
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

	bool sentVariableTable = false;

	while (!shutdown)
	{
		if (clientSocket->IsConnected())
		{
			if (!sentVariableTable)
			{
				std::vector<char> variableDescriptionTable = PrepareVariableDescriptionTable();
				clientSocket->Send(variableDescriptionTable.data(), variableDescriptionTable.size());
				sentVariableTable = true;
			}

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
						tpp::Variable variable = tpp::GlobalClientVariableManager->Find(path);

						auto variablePosition = std::search(packetData.begin(), packetData.end(), tpp::VariableString, tpp::VariableString + strlen(tpp::VariableString));

						if (variablePosition != packetData.end() && variable.memory != nullptr)
						{
							auto valueIndex = variablePosition - packetData.begin();
							tpp::VariableHeader* variablePacket = reinterpret_cast<tpp::VariableHeader*>(&packetData[valueIndex]);

							auto variableIndex = valueIndex + sizeof(tpp::VariableHeader);

							if (IsCopyable(variablePacket->type))
							{
								memcpy(variable.memory, &packetData[variableIndex], variablePacket->size);
							}
						}
					}

					if (SSRNumberOfRays == 16.0f)
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

			if (connectReturn == tpp::SocketReturn::Ok)
			{
				sentVariableTable = false;
			}
		}
	}

	tpp::Network::Shutdown();

	printf("Client closed successfully\n");

	return 0;
}
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <algorithm>

#include "tppNetwork.h"
#include "tppISocket.h"
#include "tppTypes.h"
#include "tppSerialize.h"
#include "client/tppClientVariableManager.h"

//---------
// EXAMPLES
//---------

// SSR
tpp::Float SSRNumberOfRays("Rendering/Post Effects/SSR/Number of Rays", 8.0f, 1.0f, 64.0f, 1.0f);
tpp::UInt SSRThicknessMultiplier("Rendering/Post Effects/SSR/Thickness Multiplier", 1, 1, 8, 1);
tpp::Int SSRThicknessBias("Rendering/Post Effects/SSR/Thickness Bias", -1, -10, 10, 2);
tpp::Color3 SSRClearColor("Rendering/Post Effects/SSR/Clear Color", 1.0f, 0.5f, 0.3f);
tpp::Color4 SSRRayColor("Rendering/Post Effects/SSR/Ray Color", 1.0f, 0.5f, 0.3f, 0.2f);
tpp::Bool SSREnabled("Rendering/Post Effects/SSR/Enabled", false);

// Depth of Field
tpp::Float DepthOfFieldAperture("Rendering/Post Effects/Depth of Field/Aperture", 2.0f, 0.001f, 8.0f, 1.0f);
tpp::Float DepthOfFieldBokehSize("Rendering/Post Effects/Depth of Field/Bokeh Size", 2.5f, 1.0f, 32.0f, 1.0f);

// TAA
tpp::Float TAAJitterX("Rendering/Post Effects/TAA/TAA Jitter X", 0.5f, 0.0f, 128.0f, 1.0f);
tpp::Float TAAJitterY("Rendering/Post Effects/TAA/TAA Jitter Y", 0.5f, 0.0f, 128.0f, 1.0f);

tpp::Float PerformanceGraphScaleX("Rendering/Performance/Graph Scale X", 1.5f, 0.1f, 4.0f, 1.0f);
tpp::Float PerformanceGraphScaleY("Rendering/Performance/Graph Scale Y", 1.5f, 0.1f, 4.0f, 1.0f);

tpp::Float CoreGraphScaleX("Core/Performance/Graph Scale X", 1.3f, 0.1f, 4.0f, 1.0f);
tpp::Float CoreGraphScaleY("Core/Performance/Graph Scale Y", 1.3f, 0.1f, 4.0f, 1.0f);

tpp::Float AnimationTimeScale("Animation/Time Scale", 1.0f, 0.0f, 2.0f, 1.0f);
tpp::Float AnimationThreshold("Animation/Threshold", 1.0f, 1.0f, 3.0f, 1.0f);

tpp::Float PhysicsTargetFPS("Physics/Target FPS", 60.0f, 1.0f, 120.0f, 1.0f);
tpp::Float PhysicsFPSLimit("Physics/Performance/FPS Limit", 120.0f, 0.0f, 120.0f, 1.0f);

tpp::Float DebugDisplayDeferredNormals("Rendering/Debug Display/Deferred/Normals", 0.77f, 0.0f, 1.0f, 1.0f);
tpp::Float DebugDisplayForwardAlbedo("Rendering/Debug Display/Forward/Albedo", 1.0f, 0.0f, 1.0f, 1.0f);

bool IsCopyable(tpp::VariableType type)
{
	switch (type)
	{
		case tpp::VariableType::Function:
			return false;
	}

	return true;
}

void PrepareVariableDescriptionTable(tpp::Archive<tpp::SerializationStreamType::RawStreamWrite>& variableDescriptionTable)
{
	tpp::GetClientVariableManager()->ForEachVariable([&variableDescriptionTable](const std::string& path, const tpp::Variable& variable)
	{
		variableDescriptionTable.SerializeVariableDescription(path, variable);
	});
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

	tpp::SerializationStream<tpp::SerializationStreamType::RawStreamWrite> writerStream(DEFAULT_BUFLEN);
	tpp::Archive<tpp::SerializationStreamType::RawStreamWrite> variableDescriptionTable(writerStream);

	while (!shutdown)
	{
		if (clientSocket->IsConnected())
		{
			if (!sentVariableTable)
			{
				variableDescriptionTable.Clear();
				PrepareVariableDescriptionTable(variableDescriptionTable);
				clientSocket->Send(variableDescriptionTable.GetStream().Data(), variableDescriptionTable.GetStream().Size());
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
					tpp::MessageHeader* header = reinterpret_cast<tpp::MessageHeader*>(&*headerPosition);
					tpp::MessageType messageType = header->type;
					tpp::Version version = header->version;
					uint32_t packetSize = header->size;

					// Find where the header starts, and copy the data onward
					auto index = headerPosition + sizeof(tpp::MessageHeader) - receivedData.begin();

					std::vector<char> packetData;
					packetData.reserve(packetSize);
					packetData.insert(packetData.end(), &receivedData[index], &receivedData[index] + packetSize);

					auto currentPosition = packetData.begin();

					// Search for path in packet and read path
					std::string path;
					{
						auto nullTerminator = std::find(currentPosition, packetData.end(), '\0');
						path = std::string(packetData.begin(), nullTerminator);
						currentPosition = nullTerminator + 1;
					}

					// Use the type to read in the value
					{
						const tpp::Variable& variable = tpp::GetClientVariableManager()->Find(path);

						if (variable.memory != nullptr)
						{
							auto valueIndex = currentPosition - packetData.begin();
							tpp::VariableHeader* variablePacket = reinterpret_cast<tpp::VariableHeader*>(&packetData[valueIndex]);

							auto variableIndex = valueIndex + sizeof(tpp::VariableHeader);

							if (IsCopyable(variablePacket->type))
							{
								memcpy(variable.memory, &packetData[variableIndex], variablePacket->size);
							}
						}
					}

					auto nextHeaderPosition = std::search(headerPosition + 1, receivedData.end(), tpp::HeaderString, tpp::HeaderString + strlen(tpp::HeaderString));

					headerPosition = nextHeaderPosition;
				}
			}
			else if (receiveResult == tpp::SocketReturn::Ok || receiveResult == tpp::SocketReturn::WouldBlock)
			{
				// Ignore
			}
			else if (receiveResult == tpp::SocketReturn::ConnectionClosed)
			{
				// Close the socket. This means we've closed the server				
				clientSocket->Close();
				printf("Connection closed\n");

				// Reopen and leave in a good state
				clientSocket->Create();
				clientSocket->SetBlocking(false);
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
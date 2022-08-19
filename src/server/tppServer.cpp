#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <algorithm>

#include "tppNetwork.h"
#include "tppISocket.h"
#include "tppTypes.h"
#include "tppSerialize.h"
#include "server/tppServerVariableManager.h"

//---------
// EXAMPLES
//---------

// SSR
tpp::Float SSRNumberOfRays("Rendering/Post Effects/SSR/Number of Rays", 8.0f, 1.0f, 64.0f, 1.0f);
tpp::UInt SSRThicknessMultiplier("Rendering/Post Effects/SSR/Thickness Multiplier", 1, 1, 8, 1);
tpp::Int SSRThicknessBias("Rendering/Post Effects/SSR/Thickness Bias", -1, -10, 10, 2);
tpp::Bool SSREnabled("Rendering/Post Effects/SSR/Enabled", false);
tpp::Vector2 SSRDirection2("Rendering/Post Effects/SSR/Direction 2", 1.0f, 0.7f);
tpp::Vector3 SSRDirection3("Rendering/Post Effects/SSR/Direction 3", 0.4f, 0.2f, 0.3f);
tpp::Vector4 SSRDirection4("Rendering/Post Effects/SSR/Direction 4", 0.34f, 0.5f, 0.6f, 0.1f);
tpp::Color3 SSRClearColor("Rendering/Post Effects/SSR/Clear Color", 1.0f, 0.5f, 0.3f);
tpp::Color4 SSRRayColor("Rendering/Post Effects/SSR/Ray Color", 0.7f, 0.4f, 0.2f, 0.2f);

void RecompileShadersCallback()
{
	printf("I recompiled shaders\n");
}

tpp::Callback RecompileShaders("Rendering/Post Effects/SSR/Recompile Shaders", RecompileShadersCallback);

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

template<typename TppVariable>
void SerializeTppVariableDeclaration(TppVariable& variable, const tpp::Hash& hash, tpp::SerializationStream<tpp::SerializationStreamType::BinaryWrite>& stream)
{
	// Send the entire variable as it contains the description
	stream << tpp::VariableHeader(variable.type, sizeof(variable), hash);
	stream << variable;
}

template<>
void SerializeTppVariableDeclaration<tpp::Callback>(tpp::Callback& variable, const tpp::Hash& hash, tpp::SerializationStream<tpp::SerializationStreamType::BinaryWrite>& stream)
{
	stream << tpp::VariableHeader(variable.type, 0, hash);
}

void SerializeVariableDescription(const tpp::Variable& inVariable, const std::string& inPath, const tpp::Hash& hash, tpp::SerializationStream<tpp::SerializationStreamType::BinaryWrite>& stream)
{
	tpp::Variable& variable = const_cast<tpp::Variable&>(inVariable);
	std::string& path = const_cast<std::string&>(inPath);

	size_t startSize = stream.Size();

	// We don't know what the size is going to be yet so we put 0. The final step will patch the header with the
	// size that we know once the whole message has been constructed
	stream << tpp::MessageHeader(0, tpp::MessageType::Declaration);

	// Serialize the path of the variable so the server can display it
	stream << path;

	if (variable.type == tpp::VariableType::Float)
	{
		SerializeTppVariableDeclaration(variable.vdFloat, hash, stream);
	}
	else if (variable.type == tpp::VariableType::UnsignedInteger)
	{
		SerializeTppVariableDeclaration(variable.vdUInt, hash, stream);
	}
	else if (variable.type == tpp::VariableType::Integer)
	{
		SerializeTppVariableDeclaration(variable.vdInt, hash, stream);
	}
	else if (variable.type == tpp::VariableType::Bool)
	{
		SerializeTppVariableDeclaration(variable.vdBool, hash, stream);
	}
	else if (variable.type == tpp::VariableType::Color3)
	{
		SerializeTppVariableDeclaration(variable.vdColor3, hash, stream);
	}
	else if (variable.type == tpp::VariableType::Color4)
	{
		SerializeTppVariableDeclaration(variable.vdColor4, hash, stream);
	}
	else if (variable.type == tpp::VariableType::Vector2)
	{
		SerializeTppVariableDeclaration(variable.vdVector2, hash, stream);
	}
	else if (variable.type == tpp::VariableType::Vector3)
	{
		SerializeTppVariableDeclaration(variable.vdVector3, hash, stream);
	}
	else if (variable.type == tpp::VariableType::Vector4)
	{
		SerializeTppVariableDeclaration(variable.vdVector4, hash, stream);
	}
	else if (variable.type == tpp::VariableType::Callback)
	{
		SerializeTppVariableDeclaration(variable.vdCallback, hash, stream);
	}
	else
	{
		printf("Variable %s not serialized correctly\n", path.c_str());
	}

	size_t totalSize = stream.Size() - startSize;
	size_t packetSize = totalSize - sizeof(tpp::MessageHeader);

	tpp::MessageHeader* header = reinterpret_cast<tpp::MessageHeader*>(stream.Back() - totalSize + 1);
	header->size = (decltype(header->size))packetSize;
}

void PrepareVariableDescriptionTable(tpp::SerializationStream<tpp::SerializationStreamType::BinaryWrite>& variableDescriptionTable)
{
	tpp::GetServerVariableManager()->ForEachVariable([&variableDescriptionTable](const tpp::Variable& variable, const std::string& path, const tpp::Hash& hash)
	{
		SerializeVariableDescription(variable, path, hash, variableDescriptionTable);
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

	tpp::SerializationStream<tpp::SerializationStreamType::BinaryWrite> variableDescriptionTable(DEFAULT_BUFLEN);

	while (!shutdown)
	{
		if (clientSocket->IsConnected())
		{
			if (!sentVariableTable)
			{
				variableDescriptionTable.Clear();
				PrepareVariableDescriptionTable(variableDescriptionTable);
				clientSocket->Send(variableDescriptionTable.Data(), variableDescriptionTable.Size());
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

					// Use the type to read in the value
					{
						auto valueIndex = currentPosition - packetData.begin();
						tpp::VariableHeader* variablePacket = reinterpret_cast<tpp::VariableHeader*>(&packetData[valueIndex]);

						const tpp::Variable& variable = tpp::GetServerVariableManager()->Find(variablePacket->hash);

						if (variable.type != tpp::VariableType::Invalid)
						{
							auto variableIndex = valueIndex + sizeof(tpp::VariableHeader);

							if (variablePacket->type == tpp::VariableType::Callback)
							{
								// Invoke the callback
								variable.vdCallback.currentValue();
							}
							else if (variablePacket->size > 0)
							{
								// Copy the memory over as-is, we assume the format is correct
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
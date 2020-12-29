#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <vector>
#include <chrono>
#include <thread>
#include <algorithm>
#include <string>
#include <memory>
#include <unordered_map>

#include "tppNetwork.h"
#include "tppISocket.h"

#include "tppServerVariableManager.h"
#include "tppUIBackend.h"
#include "tppUILog.h"
#include "tppUIVariableTree.h"
#include "tppUIConnectionsWindow.h"
#include "tppSerialize.h"
#include "tppTypes.h"

#include "imgui.h"

std::vector<char> PrepareMessage1()
{
	std::string path = u8"Rendering/SSR/Number of Rays";

	std::vector<char> fullPacket;

	SerializeCommandHeader(fullPacket, tpp::MessageType::Update);

	SerializePath(fullPacket, path);

	SerializeFloat(fullPacket, 16.0);

	// 3 Calculate message size and update packet
	size_t totalDataSize = fullPacket.size() - sizeof(tpp::MessageHeader);
	tpp::MessageHeader* header = reinterpret_cast<tpp::MessageHeader*>(fullPacket.data());
	header->messageSize = totalDataSize;

	return fullPacket;
}

// TODO There should be one per connection
tpp::ServerVariableManager GlobalServerVariableManager;

void ProcessPacket(const std::vector<char>& currentPacketData)
{
	// Search for path in packet and read path
	std::string path;
	{
		auto pathPosition = std::search(currentPacketData.begin(), currentPacketData.end(), tpp::PathString, tpp::PathString + strlen(tpp::PathString));
		auto nullTerminator = std::find(pathPosition, currentPacketData.end(), '\0');
		path = std::string(pathPosition + strlen(tpp::PathString), nullTerminator);
	}

	auto variablePosition = std::search(currentPacketData.begin(), currentPacketData.end(), tpp::VariableString, tpp::VariableString + strlen(tpp::VariableString));

	if (variablePosition != currentPacketData.end())
	{
		auto valueIndex = variablePosition - currentPacketData.begin();
		const tpp::VariableHeader* variablePacket = reinterpret_cast<const tpp::VariableHeader*>(&currentPacketData[valueIndex]);

		auto variableIndex = valueIndex + sizeof(tpp::VariableHeader);

		if (variablePacket->type == tpp::VariableType::Float)
		{
			float initialValue = *reinterpret_cast<const float*>(&currentPacketData[variableIndex]);

			tpp::Variable floatVariableDescription;
			floatVariableDescription.vdFloat = tpp::Float("", initialValue, 0.0f, 0.0f, 0.0f);

			// Put in global registry
			GlobalServerVariableManager.AddVariable(path, floatVariableDescription);
		}
	}
}

int main(void)
{
	tpp::UIInitializeParams params;
	params.windowPositionX = 100;
	params.windowPositionY = 100;
	params.windowWidth = 1280;
	params.windowHeight = 800;

	tpp::UIBackend::Initialize(params);

	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	tpp::UILog uiLog;
	tpp::UIVariableTree variableTree;
	tpp::UIConnectionsWindow connectionsWindow;

	static const int DEFAULT_BUFLEN = 512;

	char receiveBuffer[DEFAULT_BUFLEN];
	int receiveBufferLength = DEFAULT_BUFLEN;

	tpp::Network::Initialize();

	tpp::NetworkAddress address("127.0.0.1", 27001);
	tpp::ISocket* serverSocket = tpp::Network::CreateSocket();
	tpp::ISocket* clientSocket = tpp::Network::CreateSocket();

	serverSocket->Create();
	serverSocket->SetBlocking(false);
	serverSocket->Listen(address.port);

	std::vector<std::vector<char>> messages;

	uint64_t lastAttemptedConnection = 0;

	bool packetOpen = false;

	std::vector<char> receivedData;
	receivedData.reserve(DEFAULT_BUFLEN);

	std::vector<char> currentPacketData;
	currentPacketData.reserve(DEFAULT_BUFLEN);

	while (tpp::UIBackend::PrepareNewFrame() != tpp::UIBackendResult::Quit)
	{
		if (clientSocket->IsConnected())
		{
			tpp::SocketReturn::T receiveResult = clientSocket->Receive(receiveBuffer, receiveBufferLength);

			if (receiveResult > 0)
			{
				// Parse variable description message

				// Copy all the data into a vector
				// TODO Use the original buffer instead
				receivedData.clear();
				receivedData.insert(receivedData.begin(), receiveBuffer, receiveBuffer + receiveResult);

				auto headerPosition = receivedData.begin();
				size_t offset = 0;

				// If we have a pending packet, assume the rest of the content is coming next
				// We search for another header in the incoming data. If we find one, we only
				// copy up to that point. Otherwise, copy all the data. If the incoming data
				// fills the buffer we need to wait 
				if (currentPacketData.size() > 0)
				{
					headerPosition = std::search(receivedData.begin() + offset, receivedData.end(), tpp::HeaderString, tpp::HeaderString + strlen(tpp::HeaderString));

					// If we actually manage to find a new header, we know where the existing message ends
					if (headerPosition != receivedData.end())
					{
						currentPacketData.insert(currentPacketData.end(), receivedData.begin(), headerPosition);
						ProcessPacket(currentPacketData);
					}
				}

				while (true)
				{
					headerPosition = std::search(receivedData.begin() + offset, receivedData.end(), tpp::HeaderString, tpp::HeaderString + strlen(tpp::HeaderString));

					size_t headerIndex = headerPosition - receivedData.begin();

					if (headerPosition != receivedData.end())
					{
						// If we found the header, there are several possible cases
						size_t remainingDataInBuffer = (size_t)receiveResult - (headerPosition - receivedData.begin());

						if (remainingDataInBuffer >= sizeof(tpp::MessageHeader))
						{
							tpp::MessageHeader* header = reinterpret_cast<tpp::MessageHeader*>(&*headerPosition);
							tpp::MessageType messageType = header->messageType;
							tpp::Version version = header->version;
							uint32_t packetSize = header->messageSize;

							if (remainingDataInBuffer >= sizeof(tpp::MessageHeader) + packetSize)
							{
								currentPacketData.clear();
								currentPacketData.insert(currentPacketData.end(), headerPosition, headerPosition + packetSize);
								ProcessPacket(currentPacketData);
								offset = (headerPosition - receivedData.begin()) + sizeof(tpp::MessageHeader) + packetSize;
							}
							else
							{
								// Insert into the current packet, but don't process it yet as it's incomplete
								currentPacketData.clear();
								currentPacketData.insert(currentPacketData.end(), headerPosition, receivedData.end());
								break;
							}
						}
						else
						{
							// Insert into the current packet, but don't process it yet as it's incomplete
							currentPacketData.clear();
							currentPacketData.insert(currentPacketData.end(), headerPosition, receivedData.end());
							break;
						}
					}
					else
					{
						// If we didn't find the header, there are two possible reasons.
						// 1) We actually parsed all messages that arrived
						// 2) The message was split and the pattern 'tpp' has been split as well (bad luck!) so we need to store
						// whatever part is there and try again with more data

						// Insert into the current packet, but don't process it yet as it's incomplete
						currentPacketData.clear();
						currentPacketData.insert(currentPacketData.end(), headerPosition, receivedData.end());
						break;
					}
				}
			}
			else if (receiveResult == tpp::SocketReturn::Timeout || receiveResult == tpp::SocketReturn::WouldBlock)
			{

			}
			else
			{
				clientSocket->Close();

				// TODO Clear everything related to a connection
				GlobalServerVariableManager.Clear();
			}

			// TODO Check if it's still connected as we might have dropped the connection here!
			if (!messages.empty())
			{
				const std::vector<char>& message = messages.back();

				uiLog.Log(message.data());

				tpp::SocketReturn::T sendResult = clientSocket->Send(message.data(), message.size());
				// TODO Handle send issues
				messages.pop_back();
			}
		}
		else
		{
			time_t currentTime = time(NULL);

			// TODO Use better time
			uint64_t timeDelta = currentTime - lastAttemptedConnection;

			if (timeDelta > 0)
			{
				uiLog.Log("Listening for connections... (%lli)\n", currentTime);
				printf("Listening for connections... (%lli)\n", currentTime);
				tpp::SocketReturn::T acceptReturn = serverSocket->Accept(address, clientSocket);

				if (acceptReturn != tpp::SocketReturn::Ok)
				{
					serverSocket->Close();
				}
				else
				{
					messages.push_back(PrepareMessage1());
				}

				lastAttemptedConnection = currentTime;
			}
		}

		// Prepare the UI elements
		{
			ImGuiDockNodeFlags dockNodeFlags = 0;
			ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), dockNodeFlags);

			ImGuiWindowFlags windowFlags = 0;

			// Variable Tree
			ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Appearing);
			ImGui::SetNextWindowSize(ImVec2((float)tpp::UIBackend::GetWindowWidth() / 2, (float)(tpp::UIBackend::GetWindowHeight() - 200)), ImGuiCond_Appearing);

			variableTree.Draw(GlobalServerVariableManager, "Variable Tree", nullptr);

			// Connection Window (with variables)
			ImGui::SetNextWindowPos(ImVec2((float)tpp::UIBackend::GetWindowWidth() / 2, 0), ImGuiCond_Appearing);
			ImGui::SetNextWindowSize(ImVec2((float)tpp::UIBackend::GetWindowWidth() / 2, (float)(tpp::UIBackend::GetWindowHeight() - 200)), ImGuiCond_Appearing);

			connectionsWindow.Draw("Connections");

			// Log
			ImGui::SetNextWindowPos(ImVec2(0, (float)(tpp::UIBackend::GetWindowHeight() - 200)), ImGuiCond_Appearing);
			ImGui::SetNextWindowSize(ImVec2((float)tpp::UIBackend::GetWindowWidth(), 200), ImGuiCond_Appearing);

			uiLog.Draw("Log", nullptr);

			// Console
		}

		tpp::UIBackend::Render();
	}

	clientSocket->Close();

	tpp::Network::Shutdown();

	tpp::UIBackend::Shutdown();

	return 0;
}
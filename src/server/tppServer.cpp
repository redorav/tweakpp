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
#include "tppUIConnectionWindow.h"
#include "tppSerialize.h"
#include "tppTypes.h"

#include "imgui.h"

// TODO There should be one per connection
std::unique_ptr<tpp::ServerVariableManager> GlobalServerVariableManager;

void ProcessDeclarationPacket(tpp::ServerVariableManager* serverVariableManager, const std::vector<char>& currentPacketData)
{
	auto currentPosition = currentPacketData.begin();

	// Search for path in packet and read path
	std::string path;
	{
		auto nullTerminator = std::find(currentPosition + sizeof(tpp::MessageHeader), currentPacketData.end(), '\0');
		path = std::string(currentPacketData.begin() + sizeof(tpp::MessageHeader), nullTerminator);
		currentPosition = nullTerminator + 1;
	}

	auto valueIndex = currentPosition - currentPacketData.begin();
	const tpp::VariableHeader* variablePacket = reinterpret_cast<const tpp::VariableHeader*>(&currentPacketData[valueIndex]);

	auto variableIndex = valueIndex + sizeof(tpp::VariableHeader);

	bool validVariable = true;

	tpp::Variable variable(variablePacket->type, path, variablePacket->hash);

	switch (variablePacket->type)
	{
		case tpp::VariableType::Float:           variable.vdFloat   = *reinterpret_cast<const tpp::Float*>(&currentPacketData[variableIndex]); break;
		case tpp::VariableType::UnsignedInteger: variable.vdUInt    = *reinterpret_cast<const tpp::UInt*>(&currentPacketData[variableIndex]); break;
		case tpp::VariableType::Integer:         variable.vdInt     = *reinterpret_cast<const tpp::Int*>(&currentPacketData[variableIndex]); break;
		case tpp::VariableType::Bool:            variable.vdBool    = *reinterpret_cast<const tpp::Bool*>(&currentPacketData[variableIndex]); break;
		case tpp::VariableType::Vector2:         variable.vdVector2 = *reinterpret_cast<const tpp::Vector2*>(&currentPacketData[variableIndex]); break;
		case tpp::VariableType::Vector3:         variable.vdVector3 = *reinterpret_cast<const tpp::Vector3*>(&currentPacketData[variableIndex]); break;
		case tpp::VariableType::Vector4:         variable.vdVector4 = *reinterpret_cast<const tpp::Vector4*>(&currentPacketData[variableIndex]); break;
		case tpp::VariableType::Color3:          variable.vdColor3  = *reinterpret_cast<const tpp::Color3*>(&currentPacketData[variableIndex]); break;
		case tpp::VariableType::Color4:          variable.vdColor4  = *reinterpret_cast<const tpp::Color4*>(&currentPacketData[variableIndex]); break;
		case tpp::VariableType::Callback: break;
		default: validVariable = false;
	}

	if (validVariable)
	{
		serverVariableManager->AddVariable(variable);
	}
	else
	{
		printf("Unrecognized variable found while processing packet\n");
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
	tpp::UIConnectionWindow uiConnectionWindow;

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

	GlobalServerVariableManager = std::unique_ptr<tpp::ServerVariableManager>(new tpp::ServerVariableManager());

	std::vector<std::vector<char>> messages;

	uint64_t lastAttemptedConnection = 0;

	bool packetOpen = false;

	std::vector<char> receivedData;
	receivedData.reserve(DEFAULT_BUFLEN);

	std::vector<char> currentPacketData;
	currentPacketData.reserve(DEFAULT_BUFLEN);

	tpp::SerializationStream<tpp::SerializationStreamType::RawStreamWrite> writerStream(DEFAULT_BUFLEN);
	tpp::Archive<tpp::SerializationStreamType::RawStreamWrite> serializationWriter(writerStream);

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
						ProcessDeclarationPacket(GlobalServerVariableManager.get(), currentPacketData);
					}
				}

				while (true)
				{
					headerPosition = std::search(receivedData.begin() + offset, receivedData.end(), tpp::HeaderString, tpp::HeaderString + strlen(tpp::HeaderString));

					if (headerPosition != receivedData.end())
					{
						// If we found the header, there are several possible cases
						size_t remainingDataInBuffer = (size_t)receiveResult - (headerPosition - receivedData.begin());

						if (remainingDataInBuffer >= sizeof(tpp::MessageHeader))
						{
							tpp::MessageHeader* header = reinterpret_cast<tpp::MessageHeader*>(&*headerPosition);
							tpp::MessageType messageType = header->type;
							tpp::Version version = header->version;
							uint32_t packetSize = header->size;

							if (remainingDataInBuffer >= sizeof(tpp::MessageHeader) + packetSize)
							{
								currentPacketData.clear();
								currentPacketData.insert(currentPacketData.end(), headerPosition, headerPosition + sizeof(tpp::MessageHeader) + packetSize);
								ProcessDeclarationPacket(GlobalServerVariableManager.get(), currentPacketData);
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
				GlobalServerVariableManager->Clear();
			}

			// TODO Check if it's still connected as we might have dropped the connection here!

			if(!serializationWriter.GetStream().Empty())
			{
				// TODO Handle send issues
				tpp::SocketReturn::T sendResult = clientSocket->Send(serializationWriter.GetStream().Data(), serializationWriter.GetStream().Size());
				serializationWriter.Clear();
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

				lastAttemptedConnection = currentTime;
			}
		}

		// Prepare the UI elements
		{
			const tpp::Variable* modifiedVariable = nullptr;

			if (ImGui::BeginMainMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("New Connection", "Ctrl + N"))
					{

					}

					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Help"))
				{
					if (ImGui::MenuItem("About"))
					{

					}
					ImGui::EndMenu();
				}
				ImGui::EndMainMenuBar();
			}

			ImGuiWindowFlags windowFlags = 0;
			windowFlags |= ImGuiWindowFlags_NoCollapse;
			//windowFlags |= ImGuiWindowFlags_NoResize;
			//windowFlags |= ImGuiWindowFlags_AlwaysAutoResize;
			ImGui::Begin("Connections", nullptr, windowFlags);
			{
				ImGui::BeginTabBar("Tab Bar");
				{
					uiConnectionWindow.Draw(GlobalServerVariableManager.get(), "Local : 192.168.0.1", modifiedVariable);
				
					if (ImGui::BeginTabItem("Xbox One : 192.168.0.32"))
					{
						ImGui::Text("This is the Xbox tab!");
						ImGui::EndTabItem();
					}
				}
				ImGui::EndTabBar();
			}
			ImGui::End();

			// Log
			ImGui::SetNextWindowPos(ImVec2(0, (float)(tpp::UIBackend::GetWindowHeight() - 200)), ImGuiCond_Appearing);
			ImGui::SetNextWindowSize(ImVec2((float)tpp::UIBackend::GetWindowWidth(), 200), ImGuiCond_Appearing);

			uiLog.Draw("Log", nullptr);

			// Console

			if (modifiedVariable)
			{
				serializationWriter.SerializeTppVariableUpdatePacket(*modifiedVariable);
			}
		}

		tpp::UIBackend::Render();
	}

	clientSocket->Close();

	tpp::Network::Shutdown();

	tpp::UIBackend::Shutdown();

	return 0;
}
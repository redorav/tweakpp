#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <vector>
#include <chrono>
#include <thread>

#include <string>

#include "tppNetwork.h"
#include "tppISocket.h"

#include "tppUIBackend.h"
#include "tppUILog.h"
#include "tppUIVariableTree.h"
#include "tppUIConnectionsWindow.h"

#include "imgui.h"

// Example
//
// HEADER
// tpp MessageType::Update 33 2 - Update message that is 33 bytes, version 2
// 
// MESSAGE
// [Header]pathRendering/SSR/Number of Rays\0[floatType][floatValue]
// [Header]pathRendering/SSR/Optimized\0[boolType][boolValue]
// [Header]pathRendering/SSR/Clear\0[functionType]

void SerializeFloat(std::vector<char>& updateCommandStream, float value)
{
	tpp::VariableHeader floatPacket;
	floatPacket.type = tpp::VariableType::Float;
	floatPacket.size = 4;
	updateCommandStream.insert(updateCommandStream.end(), reinterpret_cast<char*>(&floatPacket), reinterpret_cast<char*>(&floatPacket) + sizeof(floatPacket));
	updateCommandStream.insert(updateCommandStream.end(), reinterpret_cast<char*>(&value), reinterpret_cast<char*>(&value) + sizeof(value));
}

void SerializePath(std::vector<char>& updateCommandStream, const std::string& path)
{
	updateCommandStream.insert(updateCommandStream.end(), tpp::PathString, tpp::PathString + strlen(tpp::PathString));
	updateCommandStream.insert(updateCommandStream.end(), path.begin(), path.end() + 1);
}

void SerializeCommandHeader(std::vector<char>& updateCommandStream, tpp::MessageType messageType)
{
	// We don't know what the size is going to be yet so we put 0. The final step will patch the header with the
	// size that we know once the whole message has been constructed
	tpp::MessageHeader header;
	header.messageType = messageType;
	header.messageSize = 0;

	updateCommandStream.insert(updateCommandStream.end(), (const char*)&header, (const char*)&header + sizeof(header));
}

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

	uint64_t lastAttemptedConnection = 0;

	while (tpp::UIBackend::PrepareNewFrame() != tpp::UIBackendResult::Quit)
	{
		if (clientSocket->IsConnected())
		{
			if (!messages.empty())
			{
				const std::vector<char>& message = messages.back();

				uiLog.Log(message.data());

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

			variableTree.Draw("Variable Tree", nullptr);

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
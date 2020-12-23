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
// tpp33path20Rendering/SSR/Number of Raystypefloatvalue[f]

std::vector<char> PrepareMessage1()
{
	std::string path = u8"Rendering/SSR/Number of Rays";
	std::string type = "float";

	std::vector<char> fullPacket;
	
	// 1 Reserve space in the packet for tpp and the size of the data
	const char dummySize[4] = {};
	fullPacket.insert(fullPacket.end(), tpp::HeaderString, tpp::HeaderString + strlen(tpp::HeaderString));
	fullPacket.insert(fullPacket.end(), dummySize, dummySize + 4);

	// 2 Populate all the packet data after that
	fullPacket.insert(fullPacket.end(), tpp::PathString, tpp::PathString + strlen(tpp::PathString));
	fullPacket.insert(fullPacket.end(), path.begin(), path.end() + 1);

	fullPacket.insert(fullPacket.end(), tpp::TypeString, tpp::TypeString + strlen(tpp::TypeString));
	fullPacket.insert(fullPacket.end(), type.begin(), type.end() + 1);

	float value = 16.0;
	fullPacket.insert(fullPacket.end(), tpp::ValueString, tpp::ValueString + strlen(tpp::ValueString));
	fullPacket.insert(fullPacket.end(), reinterpret_cast<char*>(&value), reinterpret_cast<char*>(&value) + 4);

	// 3 Calculate size as size() - (length(tpp) + 4)
	// 4 Fill in data size for the entire packet
	size_t totalDataSize = fullPacket.size() - strlen(tpp::HeaderString) - 4;
	const char* totalDataSizeChar = reinterpret_cast<const char*>(&totalDataSize);
	std::copy(totalDataSizeChar, totalDataSizeChar + 4, fullPacket.data() + strlen(tpp::HeaderString));

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
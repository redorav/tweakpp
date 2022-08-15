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

std::vector<std::unique_ptr<tpp::ServerVariableManager>> ServerVariableManagers;

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

	tpp::Network::Initialize();

	ServerVariableManagers.push_back(std::unique_ptr<tpp::ServerVariableManager>(new tpp::ServerVariableManager("localhost", 27001)));
	ServerVariableManagers.push_back(std::unique_ptr<tpp::ServerVariableManager>(new tpp::ServerVariableManager("127.0.0.1", 27002)));

	std::vector<std::vector<char>> messages;

	while (tpp::UIBackend::PrepareNewFrame() != tpp::UIBackendResult::Quit)
	{
		for (auto& serverVariableManager : ServerVariableManagers)
		{
			serverVariableManager->UpdateConnection();
		}

		bool popupOpen = false;

		// Prepare the UI elements
		{
			const tpp::Variable* modifiedVariable = nullptr;

			if (ImGui::BeginMainMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("New Connection", "Ctrl + N"))
					{
						popupOpen = true;
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

			if (popupOpen)
			{
				ImGui::OpenPopup("New Connection");
			}

			// Make dimensions adequate
			{
				ImVec2 center = ImGui::GetMainViewport()->GetCenter();
				ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

				if (ImGui::BeginPopupModal("New Connection", NULL, ImGuiWindowFlags_AlwaysAutoResize))
				{
					ImGui::Text("Enter IP Address and port\n\n");
					ImGui::Separator();

					static char IPBuffer[16] = "localhost";
					ImGui::Text("IP Address");
					ImGui::InputText("##IP Address", IPBuffer, sizeof(IPBuffer));
					ImGui::SameLine();

					static char portBuffer[16] = "27001";
					ImGui::InputText("##Port", portBuffer, sizeof(portBuffer));

					if (ImGui::Button("OK", ImVec2(60, 0)))
					{
						// TODO std::stoi crashes if input is malformed. Create a simple function to parse a port
						uint32_t port = std::stoi(portBuffer);
						ServerVariableManagers.push_back(std::unique_ptr<tpp::ServerVariableManager>(new tpp::ServerVariableManager(IPBuffer, port)));
						ImGui::CloseCurrentPopup();
					}

					ImGui::SetItemDefaultFocus();
					ImGui::SameLine();
					if (ImGui::Button("Cancel", ImVec2(60, 0)))
					{
						ImGui::CloseCurrentPopup();
					}

					ImGui::EndPopup();
				}
				else
				{
					popupOpen = false;
				}
			}

			ImGuiID mainDockspaceID = ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

			for (auto& iter = ServerVariableManagers.begin(); iter != ServerVariableManagers.end();)
			{
				const auto& serverVariableManager = *iter;

				ImGui::SetNextWindowDockID(mainDockspaceID, ImGuiCond_Once);
				serverVariableManager->DrawConnectionWindow();

				if (serverVariableManager->MarkedAsClosed())
				{
					iter = ServerVariableManagers.erase(iter);
				}
				else
				{
					++iter;
				}
			}

			// Log
			ImGui::SetNextWindowPos(ImVec2(0, (float)(tpp::UIBackend::GetWindowHeight() - 200)), ImGuiCond_Appearing);
			ImGui::SetNextWindowSize(ImVec2((float)tpp::UIBackend::GetWindowWidth(), 200), ImGuiCond_Appearing);

			uiLog.Draw("Log", nullptr);
		}

		ImGui::ShowDemoWindow(nullptr);

		tpp::UIBackend::Render();
	}

	ServerVariableManagers.clear();

	tpp::Network::Shutdown();

	tpp::UIBackend::Shutdown();

	return 0;
}
#include "tppNetwork.h"
#include "tppISocket.h"

#include "tppClientVariableManager.h"
#include "tppUIBackend.h"
#include "tppUILog.h"
#include "tppUIConnectionWindow.h"
#include "tppSerialize.h"
#include "tppTypes.h"
#include "tppUITextIcons.h"

#include "imgui.h"
#include "imgui_internal.h"

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

std::vector<std::unique_ptr<tpp::ClientVariableManager>> ClientVariableManagers;

std::string scratchMemoryString;

int main(void)
{
	tpp::UIInitializeParams params;
	params.windowPositionX = 100;
	params.windowPositionY = 100;
	params.windowWidth = 1280;
	params.windowHeight = 800;

	tpp::UIBackend::Initialize(params);

	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	tpp::Network::Initialize();

	ClientVariableManagers.push_back(std::unique_ptr<tpp::ClientVariableManager>(new tpp::ClientVariableManager("localhost", 27001)));
	ClientVariableManagers.push_back(std::unique_ptr<tpp::ClientVariableManager>(new tpp::ClientVariableManager("127.0.0.1", 27002)));

	std::vector<std::vector<char>> messages;

	while (tpp::UIBackend::PrepareNewFrame() != tpp::UIBackendResult::Quit)
	{
		for (auto& clientVariableManager : ClientVariableManagers)
		{
			clientVariableManager->UpdateConnection();
		}

		bool popupOpen = false;

		// Prepare the UI elements
		{
			if (ImGui::BeginMainMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItemEx("New Connection", tpp::icons::ElectricPlug, "Ctrl + N"))
					{
						popupOpen = true;
					}

					if (ImGui::MenuItem("Exit"))
					{

					}

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Edit"))
				{
					if (ImGui::MenuItemEx("Options...", tpp::icons::SettingsCog, "Ctrl + O"))
					{

					}

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Help"))
				{
					if (ImGui::MenuItemEx("About", tpp::icons::Info))
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
						ClientVariableManagers.push_back(std::unique_ptr<tpp::ClientVariableManager>(new tpp::ClientVariableManager(IPBuffer, port)));
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

			for (auto iter = ClientVariableManagers.begin(); iter != ClientVariableManagers.end();)
			{
				const auto& clientVariableManager = *iter;

				ImGui::SetNextWindowDockID(mainDockspaceID, ImGuiCond_Once);
				clientVariableManager->DrawConnectionWindow();

				if (clientVariableManager->MarkedAsClosed())
				{
					iter = ClientVariableManagers.erase(iter);
				}
				else
				{
					++iter;
				}
			}
		}

		ImGui::ShowDemoWindow(nullptr);

		tpp::UIBackend::Render();
	}

	ClientVariableManagers.clear();

	tpp::Network::Shutdown();

	tpp::UIBackend::Shutdown();

	return 0;
}
﻿#include <stdlib.h>
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

	ServerVariableManagers.push_back(std::unique_ptr<tpp::ServerVariableManager>(new tpp::ServerVariableManager("127.0.0.1", 27001)));
	ServerVariableManagers.push_back(std::unique_ptr<tpp::ServerVariableManager>(new tpp::ServerVariableManager("127.0.0.1", 27002)));

	std::vector<std::vector<char>> messages;

	while (tpp::UIBackend::PrepareNewFrame() != tpp::UIBackendResult::Quit)
	{
		for (auto& serverVariableManager : ServerVariableManagers)
		{
			serverVariableManager->UpdateConnection();
		}

		// Prepare the UI elements
		{
			const tpp::Variable* modifiedVariable = nullptr;

			if (ImGui::BeginMainMenuBar())
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
				//ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(255.0f, 255.0f, 255.0f, 255.0f));

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

				ImGui::PopStyleColor(1);

				ImGui::EndMainMenuBar();
			}

			ImGuiWindowFlags windowFlags = 0;
			windowFlags |= ImGuiWindowFlags_NoCollapse;
			//windowFlags |= ImGuiWindowFlags_NoResize;
			//windowFlags |= ImGuiWindowFlags_AlwaysAutoResize;
			ImGui::Begin("Connections", nullptr, windowFlags);
			{
				ImGuiTabBarFlags tabBarFlags = 0;
				tabBarFlags |= ImGuiTabBarFlags_Reorderable;
				tabBarFlags |= ImGuiTabBarFlags_FittingPolicyResizeDown;

				ImGui::BeginTabBar("Tab Bar", tabBarFlags);
				{
					for (auto& serverVariableManager : ServerVariableManagers)
					{
						uiConnectionWindow.Draw(serverVariableManager.get(), "Local : 192.168.0.1", modifiedVariable);

						if (modifiedVariable)
						{
							serverVariableManager->NotifyVariableModified(*modifiedVariable);
						}
					}

					if (ImGui::BeginTabItem("Xbox One : 192.168.0.32", nullptr))
					{
						ImGui::Text("This is the Xbox tab!");
						ImGui::EndTabItem();
					}

					static bool leOpen = false;

					ImGuiTabItemFlags tabButtonFlags = 0;
					tabButtonFlags |= ImGuiTabItemFlags_Trailing;
					if (ImGui::TabItemButton("+", tabButtonFlags))
					{
						// TODO Create new tab
						leOpen = true;
					}

					if (ImGui::BeginTabItem("Xbox Scarlett : 192.168.0.354", &leOpen))
					{
						ImGui::Text("This is the Xbox Scarlett tab!");
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
		}

		tpp::UIBackend::Render();
	}

	// TODO Destroy all server variable managers

	tpp::Network::Shutdown();

	tpp::UIBackend::Shutdown();

	return 0;
}
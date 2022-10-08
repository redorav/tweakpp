#include "tppNetwork.h"
#include "tppISocket.h"

#include "platform/tppPlatform.h"

#include "tppClientVariableManager.h"
#include "tppUIBackend.h"
#include "tppUILog.h"
#include "tppUIConnectionWindow.h"
#include "tppSerialize.h"
#include "tppTypes.h"
#include "tppUITextIcons.h"
#include "tppSaveData.h"

#include "imgui.h"
#include "imgui_internal.h"

#include <vector>
#include <string>
#include <memory>

std::vector<std::unique_ptr<tpp::ClientVariableManager>> ClientVariableManagers;

// We use these to temporarily modify settings. If we then cancel, they are discarded
tpp::ApplicationSettings TemporarySettings;

int main(void)
{
	tpp::Platform::Initialize();

	std::string TweakppDirectory = tpp::Platform::GetUserDirectory() + "Tweak++/";

	tpp::SaveData::LoadSettingsFromFile(tpp::SaveData::GlobalSettings, TweakppDirectory + "Settings.xml"); 

	tpp::UIInitializeParams params;
	params.windowPositionX = 100;
	params.windowPositionY = 100;
	params.windowWidth = 1280;
	params.windowHeight = 800;

	tpp::UIBackend::Initialize(params);

	tpp::Network::Initialize();

	ClientVariableManagers.push_back(std::unique_ptr<tpp::ClientVariableManager>(new tpp::ClientVariableManager("localhost", 27001)));
	ClientVariableManagers.push_back(std::unique_ptr<tpp::ClientVariableManager>(new tpp::ClientVariableManager("127.0.0.1", 27002)));

	while (tpp::UIBackend::PrepareNewFrame() != tpp::UIBackendResult::Quit)
	{
		for (auto& clientVariableManager : ClientVariableManagers)
		{
			clientVariableManager->UpdateConnection();
		}

		bool newConnectionOpen = false;
		bool optionsMenuOpen = false;

		// Prepare the UI elements
		{
			if (ImGui::BeginMainMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItemEx("New Connection", tpp::icons::ElectricPlug, "Ctrl + N"))
					{
						newConnectionOpen = true;
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
						TemporarySettings = tpp::SaveData::GlobalSettings;
						optionsMenuOpen = true;
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

			if (newConnectionOpen)
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
					newConnectionOpen = false;
				}
			}

			if (optionsMenuOpen)
			{
				ImGui::OpenPopup("Settings");
			}

			{
				ImVec2 center = ImGui::GetMainViewport()->GetCenter();
				ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

				if (ImGui::BeginPopupModal("Settings", NULL, ImGuiWindowFlags_AlwaysAutoResize))
				{
					ImGui::Text("General");
					ImGui::Separator();

					static const char* SortSettings[] =
					{
						"None",
						"Descending",
						"Ascending"
					};

					if (ImGui::BeginCombo("Variable Sort Order", SortSettings[TemporarySettings.defaultSortOrder], 0))
					{
						for (int n = 0; n < tpp::SortOrder::Count; n++)
						{
							bool isSelected = TemporarySettings.defaultSortOrder == n;
							if (ImGui::Selectable(SortSettings[n], isSelected))
							{
								TemporarySettings.defaultSortOrder = (tpp::SortOrder::T)n;
							}

							if (isSelected)
							{
								ImGui::SetItemDefaultFocus();
							}
						}

						ImGui::EndCombo();
					}

					if (ImGui::Button("OK", ImVec2(60, 0)))
					{
						tpp::SaveData::GlobalSettings = TemporarySettings;

						tpp::Platform::CreateDirectories(TweakppDirectory); // Create the AppData folder
						tpp::SaveData::SaveSettingsToFile(tpp::SaveData::GlobalSettings, TweakppDirectory + "Settings.xml");

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
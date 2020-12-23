#include "tppUIConnectionsWindow.h"

void tpp::UIConnectionsWindow::Draw(const char* title, bool* open)
{
	ImGuiWindowFlags windowFlags = 0;
	windowFlags |= ImGuiWindowFlags_NoCollapse;

	ImGui::Begin(title, nullptr, windowFlags);
	{
		ImGui::BeginTabBar("Tab Bar");
		{
			if (ImGui::BeginTabItem("Local : 192.168.0.1"))
			{
				ImGuiSliderFlags sliderFlags = 0;
				sliderFlags |= ImGuiSliderFlags_AlwaysClamp;
				ImGui::SliderFloat("Number of Rays", &m_dummyFloat, 1.0f, 64.0f, "%.0f", sliderFlags);
				ImGui::Text("This is the Local tab!");
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Xbox One : 192.168.0.32"))
			{
				ImGui::Text("This is the Xbox tab!");
				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
	}
	ImGui::End();
}

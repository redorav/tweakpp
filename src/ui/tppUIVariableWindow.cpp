#include "tppUIVariableWindow.h"

#include "server/tppServerVariableManager.h"

#include "imgui.h"

void tpp::UIVariableWindow::Draw(const tpp::ServerVariableManager& variableManager, const tpp::VariableGroupNode* variableGroupNode)
{
	if (variableGroupNode)
	{
		ImGuiTableFlags tableFlags = 0;
		tableFlags |= ImGuiTableFlags_Resizable;
		tableFlags |= ImGuiTableFlags_PadOuterX; // Because we've not enabled ImGuiTableFlags_BordersV
		tableFlags |= ImGuiTableFlags_ContextMenuInBody;
		tableFlags |= ImGuiTableFlags_ScrollY;

		if (ImGui::BeginTable("##UIVariableWindow", 2, tableFlags))
		{
			// Set up header rows
			ImGui::TableSetupColumn("Name");
			ImGui::TableSetupColumn("Value");
			ImGui::TableHeadersRow();

			// Exit header row
			ImGui::TableNextRow();

			variableManager.ForEachVariableInGroup(variableGroupNode->path, [this](const tpp::Variable* variable)
			{
				ImGui::TableNextRow();

				std::string mangledName = "##" + variable->GetName();

				if (variable->type == tpp::VariableType::Float)
				{
					ImGui::TableSetColumnIndex(0);
					ImGui::Text(variable->GetName().c_str());
					
					ImGui::TableSetColumnIndex(1);
					ImGui::SliderFloat(mangledName.c_str(), &variable->vdFloat.m_currentValue, 0.0f, 10.0f);
				}
			});

			ImGui::EndTable();
		}
	}
}
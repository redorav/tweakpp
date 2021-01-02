#include "tppUIVariableWindow.h"

#include "server/tppServerVariableManager.h"

#include "imgui.h"

void tpp::UIVariableWindow::Draw(const tpp::ServerVariableManager& variableManager, const tpp::VariableGroupNode* variableGroupNode, const tpp::Variable*& modifiedVariable)
{
	if (variableGroupNode)
	{
		ImGuiTableFlags tableFlags = 0;
		tableFlags |= ImGuiTableFlags_Resizable;
		tableFlags |= ImGuiTableFlags_PadOuterX; // Because we've not enabled ImGuiTableFlags_BordersV
		tableFlags |= ImGuiTableFlags_ScrollY;

		if (ImGui::BeginTable("##UIVariableWindow", 2, tableFlags))
		{
			// Set up header rows
			ImGui::TableSetupColumn("Name");
			ImGui::TableSetupColumn("Value");
			ImGui::TableHeadersRow();

			// Exit header row
			ImGui::TableNextRow();

			variableManager.ForEachVariableInGroup(variableGroupNode->path, [this, &modifiedVariable](const tpp::Variable* variable)
			{
				ImGui::TableNextRow();

				std::string mangledName = "##" + variable->GetName();

				if (variable->type == tpp::VariableType::Float)
				{
					ImGui::TableSetColumnIndex(0);
					ImGui::Text(variable->GetName().c_str());
					
					ImGui::TableSetColumnIndex(1);
					ImGui::Text("%.3f", variable->vdFloat.minValue);
					ImGui::SameLine();
					bool wasModified = ImGui::SliderFloat(mangledName.c_str(), &variable->vdFloat.currentValue, variable->vdFloat.minValue, variable->vdFloat.maxValue);
					ImGui::SameLine();
					ImGui::Text("%f", variable->vdFloat.maxValue);

					if (wasModified)
					{
						modifiedVariable = variable;
					}
				}
			});

			ImGui::EndTable();
		}
	}
}
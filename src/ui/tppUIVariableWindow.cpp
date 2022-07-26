#include "tppUIVariableWindow.h"

#include "server/tppServerVariableManager.h"

#include "imgui.h"

bool DrawVariableWidget(const std::string& mangledName, tpp::Variable* variable)
{
	bool wasModified = false;

	if (variable->type == tpp::VariableType::Float)
	{
		ImGui::Text("%.3f", variable->vdFloat.minValue);
		ImGui::SameLine();
		wasModified = ImGui::SliderFloat(mangledName.c_str(), &variable->vdFloat.currentValue, variable->vdFloat.minValue, variable->vdFloat.maxValue);
		ImGui::SameLine();
		ImGui::Text("%.3f", variable->vdFloat.maxValue);
	}
	else if (variable->type == tpp::VariableType::UnsignedInteger)
	{
		ImGui::Text("%i", variable->vdUInt.minValue);
		ImGui::SameLine();
		wasModified = ImGui::SliderScalar(mangledName.c_str(), ImGuiDataType_U32, &variable->vdInt.currentValue, &variable->vdUInt.minValue, &variable->vdUInt.maxValue);
		ImGui::SameLine();
		ImGui::Text("%i", variable->vdUInt.maxValue);
	}
	else if (variable->type == tpp::VariableType::Integer)
	{
		ImGui::Text("%i", variable->vdInt.minValue);
		ImGui::SameLine();
		wasModified = ImGui::SliderScalar(mangledName.c_str(), ImGuiDataType_S32, &variable->vdInt.currentValue, &variable->vdInt.minValue, &variable->vdInt.maxValue);
		ImGui::SameLine();
		ImGui::Text("%i", variable->vdInt.maxValue);
	}
	else if (variable->type == tpp::VariableType::Bool)
	{
		wasModified = ImGui::Checkbox(mangledName.c_str(), &variable->vdBool.currentValue);
	}
	else if (variable->type == tpp::VariableType::Color3)
	{
		wasModified = ImGui::ColorEdit3(mangledName.c_str(), &variable->vdColor3.r);
	}
	else if (variable->type == tpp::VariableType::Color4)
	{
		wasModified = ImGui::ColorEdit4(mangledName.c_str(), &variable->vdColor4.r);
	}
	else if (variable->type == tpp::VariableType::Vector2)
	{
		wasModified = ImGui::InputFloat2(mangledName.c_str(), &variable->vdVector2.x, "%.3f");
	}
	else if (variable->type == tpp::VariableType::Vector3)
	{
		wasModified = ImGui::InputFloat3(mangledName.c_str(), &variable->vdVector3.x, "%.3f");
	}
	else if (variable->type == tpp::VariableType::Vector4)
	{
		wasModified = ImGui::InputFloat4(mangledName.c_str(), &variable->vdVector4.x, "%.3f");
	}
	else
	{
		printf("Variable type not implemented\n");
	}

	return wasModified;
}

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

			variableManager.ForEachVariableInGroup(variableGroupNode->path, [this, &modifiedVariable](tpp::Variable* variable)
			{
				ImGui::TableNextRow();

				std::string mangledName = "##" + variable->GetName();

				ImGui::TableSetColumnIndex(0);
				ImGui::Text(variable->GetName().c_str());

				ImGui::TableSetColumnIndex(1);

				bool wasModified = DrawVariableWidget(mangledName, variable);

				if (wasModified)
				{
					modifiedVariable = variable;
				}
			});

			ImGui::EndTable();
		}
	}
}
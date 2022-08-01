#include "tppUIConnectionWindow.h"

#include "server/tppServerVariableManager.h"

#include "imgui.h"

namespace tpp
{
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
		else if (variable->type == tpp::VariableType::Callback)
		{
			// Use the original name here as we want to display it on top of the button
			wasModified = ImGui::Button(variable->GetName().c_str());
		}
		else
		{
			printf("Variable type not implemented\n");
		}

		return wasModified;
	}

	int MyCallback(ImGuiInputTextCallbackData* data)
	{
		return 0;
	}

	void UIConnectionWindow::Draw(const tpp::ServerVariableManager* variableManager, const char* title, const tpp::Variable*& modifiedVariable)
	{
		if (ImGui::BeginTabItem(variableManager->GetDisplayString(), nullptr))
		{
			// Address. We can pick up this address and modify it based on the selection, or parse it and navigate through it
			ImGui::Text("Address:");
			ImGui::SameLine();
			ImGuiInputTextFlags addressFlags = 0;
			//addressFlags |= ImGuiInputTextFlags_CharsDecimal;
			bool modifiedAddress = ImGui::InputText("##Address:", m_currentAddress, 512, addressFlags);

			if (modifiedAddress)
			{
				// Navigate to the appropriate place
				const VariableGroupNode* candidateGroupNode = variableManager->GetVariableGroupNode(std::string(m_currentAddress));

				if (candidateGroupNode)
				{
					m_selectedGroup = candidateGroupNode;
				}
			}

			ImGuiTableFlags tableFlags = 0;
			tableFlags |= ImGuiTableFlags_Resizable;
			tableFlags |= ImGuiTableFlags_BordersOuter;
			tableFlags |= ImGuiTableFlags_BordersV;
			tableFlags |= ImGuiTableFlags_ScrollY;

			if (ImGui::BeginTable("##table1", 2, tableFlags))
			{
				// Set up header rows
				ImGui::TableSetupColumn("Variable Groups");
				ImGui::TableHeadersRow();

				// Exit header row
				ImGui::TableNextRow();

				// Show variable groups
				ImGui::TableSetColumnIndex(0);

				variableManager->ForEachVariableGroup
				(
					[this](const std::string& nodeName, const VariableGroupNode& variableGroupNode)
					{
						ImGuiTreeNodeFlags nodeFlags = 0;
						nodeFlags |= ImGuiTreeNodeFlags_OpenOnArrow; // We want to be able to select it without opening
						nodeFlags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
						nodeFlags |= ImGuiTreeNodeFlags_SpanAvailWidth;

						if (variableGroupNode.nodes.empty())
						{
							nodeFlags |= ImGuiTreeNodeFlags_Leaf;
						}

						if (&variableGroupNode == m_selectedGroup)
						{
							nodeFlags |= ImGuiTreeNodeFlags_Selected;
						}

						// TODO More efficient than copy
						std::string patchedName = u8"\uf1c9 " + nodeName;

						// Take the address of the variable as the unique id so it remains consistent across frames
						bool nodeOpen = ImGui::TreeNodeEx((void*)&variableGroupNode, nodeFlags, patchedName.c_str());

						if (ImGui::IsItemClicked())
						{
							m_selectedGroup = &variableGroupNode;
						}

						return nodeOpen;
					},
					[](const std::string& nodeName, const VariableGroupNode& variableGroupNode, bool isOpen)
					{
						if (isOpen)
						{
							ImGui::TreePop();
						}
					}
				);

				// Show Variables and record which variable was modified through the UI
				ImGui::TableSetColumnIndex(1);

				if (m_selectedGroup)
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

						variableManager->ForEachVariableInGroup(m_selectedGroup->path, [this, &modifiedVariable](tpp::Variable* variable)
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

					// Copy path into the address bar
					strcpy(m_currentAddress, m_selectedGroup->path.c_str());
				}

				ImGui::EndTable();
			}

			ImGui::EndTabItem();
		}
	}
}
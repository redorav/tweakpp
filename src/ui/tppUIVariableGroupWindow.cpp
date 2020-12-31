#include "tppUIVariableGroupWindow.h"

#include "server/tppServerVariableManager.h"

void tpp::UIVariableGroupWindow::Draw(const tpp::ServerVariableManager& variableManager, const char* title, bool* open)
{
	variableManager.ForEachNode(
	[this](const std::string& nodeName, const VariableGroupNode& variable)
	{
		ImGuiTreeNodeFlags nodeFlags = 0;
		nodeFlags |= ImGuiTreeNodeFlags_OpenOnArrow; // We want to be able to select it without opening
		
		if (variable.nodes.empty())
		{
			nodeFlags |= ImGuiTreeNodeFlags_Leaf;
		}

		if (&variable == m_selectedGroup)
		{
			nodeFlags |= ImGuiTreeNodeFlags_Selected;
		}

		// TODO More efficient than copy
		std::string patchedName = u8"\uf1c9 " + nodeName;

		// Take the address of the variable as the unique id so it remains consistent across frames
		bool nodeOpen = ImGui::TreeNodeEx((void*)&variable, nodeFlags, patchedName.c_str());

		if (ImGui::IsItemClicked())
		{
			m_selectedGroup = &variable;
		}

		return nodeOpen;
	},
	[](const std::string& nodeName, const VariableGroupNode& variable, bool isOpen)
	{
		if (isOpen)
		{
			ImGui::TreePop();
		}
	});
}
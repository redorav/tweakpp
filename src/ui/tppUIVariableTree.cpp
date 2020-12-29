#include "tppUIVariableTree.h"

#include "server/tppServerVariableManager.h"

void tpp::UIVariableTree::Draw(const tpp::ServerVariableManager& variableDescriptionTree, const char* title, bool* open)
{
	ImGuiWindowFlags windowFlags = 0;
	windowFlags |= ImGuiWindowFlags_NoCollapse;

	ImGui::Begin("Variable Tree", nullptr, windowFlags);
	{
		variableDescriptionTree.ForEachNode(
		[](const std::string& nodeName, const VariableNode& variable)
		{
			// Take the address of the variable as the unique id so it remains consistent across frames
			bool nodeOpen = ImGui::TreeNodeEx((void*)&variable, 0, nodeName.c_str());
			return nodeOpen;
		},
		[](const std::string& nodeName, const VariableNode& variable, bool isOpen)
		{
			if (isOpen)
			{
				ImGui::TreePop();
			}
		});
	}
	ImGui::End();
}
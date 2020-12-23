#include "tppUIVariableTree.h"

void tpp::UIVariableTree::Draw(const char* title, bool* open)
{
	ImGuiWindowFlags windowFlags = 0;
	windowFlags |= ImGuiWindowFlags_NoCollapse;

	ImGui::Begin("Variable Tree", nullptr, windowFlags);
	{
		int id = 0;
		bool nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)id, 0, "Selectable Leaf 1");
		if (nodeOpen)
		{
			ImGui::TreePop();
		}
	}
	ImGui::End();
}
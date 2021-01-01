#include "tppUIVariableWindow.h"

#include "server/tppServerVariableManager.h"

void tpp::UIVariableWindow::Draw(const tpp::ServerVariableManager& variableManager, const tpp::VariableGroupNode* variableGroupNode)
{
	if (variableGroupNode)
	{
		variableManager.ForEachVariableInGroup(variableGroupNode->path, [this](const tpp::Variable* variable)
		{
			ImGui::SliderFloat(variable->GetName().c_str(), &m_dummyFloat, 0.0f, 10.0f);
		});
	}
}
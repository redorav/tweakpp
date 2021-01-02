#pragma once

namespace tpp
{
	class ServerVariableManager;
	class VariableGroupNode;

	class UIVariableWindow
	{
	public:

		UIVariableWindow() {}

		void Draw(const tpp::ServerVariableManager& variableDescriptionTree, const tpp::VariableGroupNode* variableGroupNode);

	private:

		float m_dummyFloat = 1.0f; // TODO Move to variable page
	};
}
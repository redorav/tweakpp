#pragma once

namespace tpp
{
	class ServerVariableManager;
	class VariableGroupNode;
	class Variable;

	class UIConnectionWindow
	{
	public:

		UIConnectionWindow() {}

		void Draw(const tpp::ServerVariableManager& variableManager, const char* title, const tpp::Variable*& modifiedVariable);

		const VariableGroupNode* GetSelectedGroup() const;

	private:

		char m_currentAddress[512] = {};

		const tpp::VariableGroupNode* m_selectedGroup = nullptr;
	};
}
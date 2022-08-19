#pragma once

namespace tpp
{
	class ClientVariableManager;
	class VariableGroupNode;
	class Variable;

	class UIConnectionWindow
	{
	public:

		void Draw(const tpp::ClientVariableManager* variableManager, const char* title, bool* open, const tpp::Variable*& modifiedVariable);

	private:

		char m_currentAddress[512] = {};

		const tpp::VariableGroupNode* m_selectedGroup = nullptr;
	};
}
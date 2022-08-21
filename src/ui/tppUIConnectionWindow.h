#pragma once

namespace tpp
{
	class ClientVariableManager;
	class VariableGroupNode;
	class VariableBase;

	class UIConnectionWindow
	{
	public:

		void Draw(const tpp::ClientVariableManager* variableManager, const char* title, bool* open, const tpp::VariableBase*& modifiedVariable);

	private:

		char m_currentAddress[512] = {};

		const tpp::VariableGroupNode* m_selectedGroup = nullptr;
	};
}
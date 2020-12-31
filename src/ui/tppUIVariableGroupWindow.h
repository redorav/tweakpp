#pragma once

#include "imgui.h"

namespace tpp
{
	class ServerVariableManager;
	class VariableGroupNode;

	class UIVariableGroupWindow
	{
	public:

		UIVariableGroupWindow()
		{

		}

		void Draw(const tpp::ServerVariableManager& variableManager, const char* title, bool* open = nullptr);

		const VariableGroupNode* GetSelectedGroup() const
		{
			return m_selectedGroup;
		}

	private:

		const tpp::VariableGroupNode* m_selectedGroup = nullptr;
	};
}
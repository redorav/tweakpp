#pragma once

#include <cstdint>

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

		void ShowTooltip(tpp::VariableBase* variable);

		// Which item we have captured being hovered
		void* m_wasHovering = nullptr;

		// When we started hovering over it
		uint64_t m_lastStartedHover = 0;

		char m_currentAddress[512] = {};

		const tpp::VariableGroupNode* m_selectedGroup = nullptr;
	};
}
#pragma once

#include "tppUILog.h"

#include <cstdint>
#include <memory>
#include <string>

namespace tpp
{
	class ClientVariableManager;
	class VariableGroupNode;
	class VariableBase;
	class UILog;

	class UIConnectionWindow
	{
	public:

		UIConnectionWindow(const tpp::ClientVariableManager* variableManager);

		void Draw(const tpp::ClientVariableManager* variableManager, const char* title, bool* open, const tpp::VariableBase*& modifiedVariable);

		void Log(const char* format...);

	private:

		void ShowTooltip(tpp::VariableBase* variable);

		// Which item we have captured being hovered
		void* m_wasHovering = nullptr;

		// When we started hovering over it
		uint64_t m_lastStartedHover = 0;

		char m_currentAddress[512] = {};

		const tpp::VariableGroupNode* m_selectedGroup = nullptr;

		// Mangled names to get unique ids
		std::string m_variablesWindowID;

		std::string m_logWindowID;

		std::unique_ptr<tpp::UILog> m_uiLog;
	};
}
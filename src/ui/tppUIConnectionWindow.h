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

		void Draw(const tpp::ClientVariableManager* variableManager, bool* open, const tpp::VariableBase*& modifiedVariable);

		void Log(const char* format...);

	private:

		void ShowTooltip(tpp::VariableBase* variable);

		// Which item we have captured being hovered
		void* m_wasHovering = nullptr;

		// When we started hovering over it
		uint64_t m_lastStartedHover = 0;

		// Current address in the address bar
		std::string m_currentAddress;

		// Current selected group in the group tree
		const tpp::VariableGroupNode* m_selectedGroup = nullptr;

		// Whether a connection has been established
		bool m_isConnected = false;

		// Temporary names to either get unique ids or change depending on status, icons, etc
		std::string m_variablesWindowID;

		std::string m_logWindowID;

		std::string m_currentDisplayString;

		// Use this string to patch names at runtime without allocating memory
		std::string m_scratchPatchedNames;

		// Connection log
		std::unique_ptr<tpp::UILog> m_uiLog;
	};
}
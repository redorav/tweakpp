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

	namespace SortOrder
	{
		enum T
		{
			None,
			Ascending,
			Descending
		};
	};

	struct UIInteractionData
	{
		tpp::VariableBase* editedVariable = nullptr;
		tpp::VariableBase* addedToFavorites = nullptr;
	};

	class UIConnectionWindow
	{
	public:

		UIConnectionWindow(const tpp::ClientVariableManager* variableManager);

		void Draw(const tpp::ClientVariableManager* variableManager, bool* open, UIInteractionData& interactionData);

		void Log(const char* format...);

		void Clear();

	private:

		void ShowContextMenu(tpp::VariableBase* variable, tpp::UIInteractionData& interactioinData);

		void ShowTooltip(tpp::VariableBase* variable);

		void UpdateCachedVariables(const tpp::ClientVariableManager* variableManager);

		// Which item we have captured being hovered
		void* m_wasHovering = nullptr;

		// When we started hovering over it
		uint64_t m_lastStartedHover = 0;

		// Current address in the address bar
		std::string m_currentAddress;

		// Current selected group node in the group tree
		const tpp::VariableGroupNode* m_selectedGroupNode = nullptr;

		// Whether a connection has been established
		bool m_isConnected = false;

		// Temporary names to either get unique ids or change depending on status, icons, etc
		std::string m_variablesWindowID;

		std::string m_logWindowID;

		std::string m_currentDisplayString;

		// Use this string to patch names at runtime without allocating memory
		std::string m_scratchPatchedNames;

		// Current variable cached structure. This is just a UI concept, the variable manager shouldn't know
		// about any of this. We must rebuild the cached structure in several scenarios, such as
		// - Add/remove variable from group
		// - Change sort order
		// - Change selected group

		std::vector<tpp::VariableBase*> m_currentVariables;

		SortOrder::T m_sortOrder = SortOrder::None;

		bool m_dirty = true;

		// Connection log
		std::unique_ptr<tpp::UILog> m_uiLog;
	};
}
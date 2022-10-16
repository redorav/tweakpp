#pragma once

#include "tppClientCommon.h"
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

	struct UIInteractionData
	{
		tpp::VariableBase* editedVariable = nullptr;
		tpp::VariableBase* addedToFavorites = nullptr;
		tpp::VariableBase* removedFromFavorites = nullptr;
		
		// We can remove a path when the variable doesn't exist in the database
		// but is still recorded as part of the favorites group
		std::string removedFromFavoritePath;
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

		void ShowDisabledVariableContextMenu(const std::string& variablePath, tpp::UIInteractionData& interactionData);

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

		// We need to delay the selection of the group node until we've had a chance
		// to refresh the variables contained within. Otherwise we can get inconsistent
		// data (e.g. cached variables don't reflect what the group contains)
		const tpp::VariableGroupNode* m_nextSelectedGroupNode = nullptr;

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

		// Name of the variable. When loading favorites sometimes variables aren't registered,
		// for example when using different builds on the same server. To be able to see those
		// favorites (the path at least) and remove them, we need to be able to display them
		struct CachedVariable
		{
			tpp::VariableBase* variable;
			std::string variablePath;
		};

		// Variables for display. Can be null (e.g. when coming from a favorites group)
		std::vector<CachedVariable> m_cachedVariables;

		ImGuiTableColumnFlags m_defaultVariableSortOrder = 0;

		SortOrder::T m_sortOrder = SortOrder::None;

		bool m_dirty = true;

		// Connection log
		std::unique_ptr<tpp::UILog> m_uiLog;
	};
}
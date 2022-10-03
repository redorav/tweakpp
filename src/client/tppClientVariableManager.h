#pragma once

#include "tppTypes.h"
#include "tppSerialize.h"
#include "tppISocket.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <memory>

namespace tpp
{
	class UIConnectionWindow;
	class UILog;

	// The variable database contains all the live variables for this session. Adding and removing
	// variables is done through this class. All variable managers have a database that other systems
	// reference. Variable groups and favorites should all have weak references to this database, and
	// can react accordingly.
	//
	// When a variable is deleted, the variable group should delete it from its known variables
	// However, a favorite group should still display it, greyed out until it's deleted from the
	// favorite group.
	//
	// We can make groups have only hashes or paths to the variables, because we don't look them all
	// up live. Instead we cache the sorted results in a vector, which we display every frame
	struct VariableDatabase
	{
		VariableBase* GetVariable(const std::string& path) const;

		void AddVariable(const std::string& path, const std::shared_ptr<VariableBase>& variable);

		void RemoveVariable(const std::string& path);

	private:

		std::unordered_map<std::string, std::shared_ptr<VariableBase>> m_variableHashmap;
	};

	// Contains a list of variables
	class VariableGroup
	{
	public:

		// TODO convert to just hashes. Eventually we want a path database so that favorite
		// groups can query a path/name without the variable existing in the database
		std::unordered_set<std::string> variableStrings;
	};

	// Represents a node in the variable group tree, that is itself a group and
	// can have other groups under it. A group with an empty array of nodes is a leaf
	// The variable group node is just a way to represent the tree, it doesn't own data
	class VariableGroupNode
	{
	public:

		VariableGroupNode() = default;

		VariableGroupNode(const std::string& path) : m_path(path) {}
		
		VariableGroupNode* AddFindNode(const std::string& path, const std::string& nodeName);

		template<typename FnOpen, typename FnClose>
		void ForEachNode(const FnOpen& fnOpen, const FnClose& fnClose) const
		{
			for (auto node = nodes.begin(); node != nodes.end(); ++node)
			{
				// Evaluate ourselves first
				bool processChildren = fnOpen(node->first, node->second);

				// Then call all children (this is recursive) if the function
				// determined that we should call children
				if (processChildren)
				{
					node->second.ForEachNode(fnOpen, fnClose);
				}

				// Call the close function. We need the flag that was returned
				// by the open to customize behavior in the close
				fnClose(processChildren);
			}
		}

		// Each node contains its full path so through a node we can easily access its variables
		std::string m_path;

		// Pointer to the actual group of variables
		tpp::VariableGroup* variableGroup = nullptr;

		// Have a map so that it's sorted on insertion, we also want to traverse it sorted
		// and we want to be able to find things in it as well
		// TODO Be able to sort variable groups too in the same way we sort variables
		std::map<std::string, VariableGroupNode> nodes;
	};

	struct VariableGroupTree
	{
		void AddPath(const std::string& path, tpp::VariableGroup* variableGroup);

		bool Exists(const std::string& path);

		const VariableGroupNode* GetVariableGroupNode(const std::string& path) const;

		template<typename FnOpen, typename FnClose>
		void ForEachNode(const FnOpen& fnOpen, const FnClose& fnClose) const
		{
			m_root.ForEachNode(fnOpen, fnClose);
		}

		void Clear();

		VariableGroupNode m_root;

		// Check to see if path already exists (when adding a new variable)
		// This also points to leaf nodes that are guaranteed to exist in 
		// the variable tree
		std::unordered_map<std::string, VariableGroupNode*> m_variableGroupNodeHashmap;
	};

	class ClientVariableManager
	{
	public:

		ClientVariableManager(const char* ipAddress, uint32_t port);

		// Receives incoming data, parses and makes sense of incoming variable definitions
		void UpdateConnection();

		void DrawConnectionWindow();

		const char* GetDisplayString() const;

		bool IsConnected() const;

		void Clear();

		bool MarkedAsClosed() const;

		template<typename Fn>
		void ForEachVariableInGroup(tpp::VariableGroup* group, const Fn& fn) const;

		template<typename FnOpen, typename FnClose>
		void ForEachVariableGroupNode(const FnOpen& fnOpen, const FnClose& fnClose) const;

		template<typename FnOpen, typename FnClose>
		void ForEachFavoriteGroupNode(const FnOpen& fnOpen, const FnClose& fnClose) const;

		const VariableGroup* GetVariableGroup(const std::string& path) const;

		const VariableGroupNode* GetVariableGroupNode(const std::string& path) const;

		// Favorites

		void AddToFavorites(const std::string& path, tpp::VariableBase* variable);

	private:

		void AddVariable(const std::shared_ptr<VariableBase>& variable);

		void ProcessDeclarationPacket(const std::vector<char>& currentPacketData);

		// Connection management

		std::vector<char> m_receivedData;

		std::vector<char> m_currentPacketData;

		tpp::BinarySerializationWriter m_writerStream;

		tpp::NetworkAddress m_networkAddress;

		std::unique_ptr<tpp::ISocket> m_clientSocket = nullptr;

		uint64_t m_lastAttemptedConnection = 0;

		// Variable management

		std::unordered_map<std::string, VariableGroupNode> m_favoriteGroupNodeHashmap;

		std::unordered_map<std::string, VariableGroup> m_favoriteGroupHashmap;

		VariableGroupTree m_variableGroupTree;

		std::unordered_map<std::string, VariableGroup> m_variableGroupHashmap;

		VariableDatabase m_variableDatabase;

		// UI Management

		bool m_windowOpen;

		std::string m_displayString;

		std::unique_ptr<tpp::UIConnectionWindow> m_uiConnectionWindow;
	};

	template<typename Fn>
	void tpp::ClientVariableManager::ForEachVariableInGroup(tpp::VariableGroup* group, const Fn& fn) const
	{
		if (group)
		{
			for (const std::string& variablePath : group->variableStrings)
			{
				tpp::VariableBase* variable = m_variableDatabase.GetVariable(variablePath);

				if (variable)
				{
					fn(variable);
				}
			}
		}
	}

	template<typename FnOpen, typename FnClose>
	void tpp::ClientVariableManager::ForEachVariableGroupNode(const FnOpen& fnOpen, const FnClose& fnClose) const
	{
		m_variableGroupTree.ForEachNode(fnOpen, fnClose);
	}

	template<typename FnOpen, typename FnClose>
	void tpp::ClientVariableManager::ForEachFavoriteGroupNode(const FnOpen& fnOpen, const FnClose& fnClose) const
	{
		for (auto it = m_favoriteGroupNodeHashmap.begin(), end = m_favoriteGroupNodeHashmap.end(); it != end; ++it)
		{
			// Evaluate node
			bool processChildren = fnOpen(it->first, it->second);

			// Call the close function. We need the flag that was returned by the open to customize behavior in the close
			fnClose(processChildren);
		}
	}
}
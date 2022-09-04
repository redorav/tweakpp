#pragma once

#include "tppTypes.h"
#include "tppSerialize.h"
#include "tppISocket.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <memory>

namespace tpp
{
	class UIConnectionWindow;
	class UILog;

	// Contains a list of variables
	class VariableGroup
	{
	public:

		// TODO Rework memory ownership

		std::unordered_map<std::string, std::shared_ptr<VariableBase>> variableHashmap;

		std::vector<tpp::VariableBase*> variables;
	};

	// Represents a node in the variable group tree, that is itself a group and
	// can have other groups under it. A group with an empty array of nodes is a leaf
	// The variable group node is just a way to represent the tree, it doesn't own data
	class VariableGroupNode
	{
	public:

		VariableGroupNode() {}

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
				fnClose(node->first, node->second, processChildren);
			}
		}

		// Each node contains its full path so through a node we can easily access its variables
		std::string m_path;

		// Pointer to the actual group of variables
		tpp::VariableGroup* variableGroup = nullptr;

		// Have a map so that it's sorted on insertion, we also want to traverse it sorted
		// and we want to be able to find things in it as well
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
		void ForEachVariableGroup(const FnOpen& fnOpen, const FnClose& fnClose) const;

		const VariableGroup* GetVariableGroup(const std::string& path) const;

		const VariableGroupNode* GetVariableGroupNode(const std::string& path) const;

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

		VariableGroupTree m_variableGroupTree;

		std::unordered_map<std::string, VariableGroup> m_variableGroupHashmap;

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
			for (tpp::VariableBase* variable : group->variables)
			{
				fn(variable);
			}
		}
	}

	template<typename FnOpen, typename FnClose>
	void tpp::ClientVariableManager::ForEachVariableGroup(const FnOpen& fnOpen, const FnClose& fnClose) const
	{
		m_variableGroupTree.ForEachNode(fnOpen, fnClose);
	}
}
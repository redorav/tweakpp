#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <map>

#include "tppTypes.h"
#include "tppSerialize.h"
#include "tppISocket.h"

namespace tpp
{
	// Represents a node in the variable group tree, that is itself a group and
	// can have other groups under it. A group with an empty array of nodes is a leaf
	class VariableGroupNode
	{
	public:

		VariableGroupNode() {}

		VariableGroupNode(const std::string& path) : path(path) {}
		
		VariableGroupNode* AddFindNode(const std::string& path, const std::string& nodeName);

		template<typename FnOpen, typename FnClose>
		void ForEachNode(const FnOpen& fnOpen, const FnClose& fnClose) const
		{
			for (auto& node = nodes.begin(); node != nodes.end(); ++node)
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
		std::string path;

		// Have a map so that it's sorted on insertion, we also want to traverse it sorted
		// and we want to be able to find things in it as well
		std::map<std::string, VariableGroupNode> nodes;
	};

	struct VariableGroupTree
	{
		void AddPath(const std::string& path);

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
		std::unordered_map<std::string, VariableGroupNode*> m_variableGroupNodeHashMap;
	};

	class VariableGroup
	{
	public:

		std::vector<tpp::Variable*> variables;
	};

	class ServerVariableManager
	{
	public:

		ServerVariableManager(const char* ipAddress, uint32_t port);

		void ProcessDeclarationPacket(const std::vector<char>& currentPacketData);

		void UpdateConnection();

		void AddVariable(const Variable& variable);

		void NotifyVariableModified(const tpp::Variable& inVariable);

		const Variable* GetVariable(const std::string& path) const;

		const char* GetDisplayString() const;

		void Clear();

		template<typename Fn>
		void ForEachVariableInGroup(const std::string& groupPath, const Fn& fn) const;

		template<typename Fn>
		void ForEachVariable(const Fn& fn) const;

		template<typename FnOpen, typename FnClose>
		void ForEachVariableGroup(const FnOpen& fnOpen, const FnClose& fnClose) const;

		const VariableGroup* GetVariableGroup(const std::string& path) const;

		const VariableGroupNode* GetVariableGroupNode(const std::string& path) const;

	private:

		std::vector<char> m_receivedData;

		std::vector<char> m_currentPacketData;

		tpp::SerializationStream<tpp::SerializationStreamType::RawStreamWrite> m_writerStream;

		std::string m_ipAddress;

		uint32_t m_port = 0;

		tpp::NetworkAddress m_networkAddress;

		tpp::ISocket* m_serverSocket = nullptr;

		tpp::ISocket* m_clientSocket = nullptr;

		uint64_t m_lastAttemptedConnection = 0;

		std::string m_displayString;

		VariableGroupTree m_variableGroupTree;

		std::unordered_map<std::string, VariableGroup> m_variableGroupHashMap;

		std::unordered_map<std::string, Variable> m_variableHashMap;
	};

	template<typename Fn>
	void tpp::ServerVariableManager::ForEachVariableInGroup(const std::string& groupPath, const Fn& fn) const
	{
		auto it = m_variableGroupHashMap.find(groupPath);

		if (it != m_variableGroupHashMap.end())
		{
			const VariableGroup& variableGroup = it->second;

			for (tpp::Variable* variable : variableGroup.variables)
			{
				fn(variable);
			}
		}
	}

	template<typename Fn>
	void tpp::ServerVariableManager::ForEachVariable(const Fn& fn) const
	{
		for (const auto& it : m_variableHashMap)
		{
			fn(it.first, it.second);
		}
	}

	template<typename FnOpen, typename FnClose>
	void tpp::ServerVariableManager::ForEachVariableGroup(const FnOpen& fnOpen, const FnClose& fnClose) const
	{
		m_variableGroupTree.ForEachNode(fnOpen, fnClose);
	}
}
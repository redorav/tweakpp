#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <map>

#include "tppTypes.h"

namespace tpp
{
	struct VariableNode
	{
		VariableNode* AddFindNode(const std::string& nodeName)
		{
			auto it = nodes.find(nodeName);

			if (it != nodes.end())
			{
				return &it->second;
			}
			else
			{
				auto it2 = nodes.insert({ nodeName, VariableNode() });
				return &it2.first->second;
			}
		}

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

	public:

		// Have a map so that it's sorted on insertion, we also want to traverse it sorted
		// and we want to be able to find things in it as well
		std::map<std::string, VariableNode> nodes;
	};

	struct VariableTree
	{
		void AddPath(const std::string& path)
		{
			if (!Exists(path))
			{
				VariableNode* currentNode = &m_root;

				size_t currentSlashPosition = 0;
				size_t newSlashPosition = path.find("/");

				// If there are no slashes, add the whole path
				if (newSlashPosition == std::string::npos)
				{
					currentNode = currentNode->AddFindNode(path);
				}
				else
				{
					while (currentSlashPosition != std::string::npos)
					{
						std::string nodeName = path.substr(currentSlashPosition, newSlashPosition - currentSlashPosition);

						if (nodeName.size() > 0)
						{
							currentNode = currentNode->AddFindNode(nodeName);
						}

						currentSlashPosition = newSlashPosition == std::string::npos ? std::string::npos : newSlashPosition + 1;
						newSlashPosition = path.find("/", currentSlashPosition);
					}
				}

				m_pathExistenceHashMap.insert({ path, currentNode });
			}
		}

		bool Exists(const std::string& path)
		{
			return m_pathExistenceHashMap.find(path) != m_pathExistenceHashMap.end();
		}

		template<typename FnOpen, typename FnClose>
		void ForEachNode(const FnOpen& fnOpen, const FnClose& fnClose) const
		{
			m_root.ForEachNode(fnOpen, fnClose);
		}

		VariableNode m_root;

		// Check to see if path already exists (when adding a new variable)
		// This also points to leaf nodes that are guaranteed to exist in 
		// the variable tree
		std::unordered_map<std::string, VariableNode*> m_pathExistenceHashMap;
	};

	class ServerVariableManager
	{
	public:

		void AddVariable(const std::string& path, const Variable& variable)
		{
			auto lastSlash = path.find_last_of("/");

			std::string partialPath = path.substr(0, lastSlash);

			m_variableTree.AddPath(partialPath);

			m_variableHashMap.insert({ path, variable });
		}

		const Variable& GetVariable(const std::string& path) const
		{
			auto variable = m_variableHashMap.find(path);

			if (variable != m_variableHashMap.end())
			{
				return variable->second;
			}
			else
			{
				return Variable();
			}
		}

		void Clear()
		{
			m_variableHashMap.clear();
		}

		template<typename Fn>
		void ForEachVariable(Fn fn) const
		{
			for (const auto& it : m_variableHashMap)
			{
				fn(it.first, it.second);
			}
		}

		template<typename FnOpen, typename FnClose>
		void ForEachNode(const FnOpen& fnOpen, const FnClose& fnClose) const
		{
			m_variableTree.ForEachNode(fnOpen, fnClose);
		}

	private:

		VariableTree m_variableTree;

		std::unordered_map<std::string, Variable> m_variableHashMap;
	};
}
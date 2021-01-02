#include "tppServerVariableManager.h"

tpp::VariableGroupNode* tpp::VariableGroupNode::AddFindNode(const std::string& path, const std::string& nodeName)
{
	auto it = nodes.find(nodeName);

	if (it != nodes.end())
	{
		return &it->second;
	}
	else
	{
		auto it2 = nodes.insert({ nodeName, VariableGroupNode(path) });
		return &it2.first->second;
	}
}

void tpp::VariableGroupTree::AddPath(const std::string& path)
{
	if (!Exists(path))
	{
		VariableGroupNode* currentNode = &m_root;

		size_t currentSlashPosition = 0;
		size_t newSlashPosition = path.find("/");

		// If there are no slashes, add the whole path
		if (newSlashPosition == std::string::npos)
		{
			currentNode = currentNode->AddFindNode(path, path);
		}
		else
		{
			while (currentSlashPosition != std::string::npos)
			{
				std::string nodeName = path.substr(currentSlashPosition, newSlashPosition - currentSlashPosition);
				std::string partialPath = path.substr(0, newSlashPosition);

				if (nodeName.size() > 0)
				{
					currentNode = currentNode->AddFindNode(partialPath, nodeName);
				}

				currentSlashPosition = newSlashPosition == std::string::npos ? std::string::npos : newSlashPosition + 1;
				newSlashPosition = path.find("/", currentSlashPosition);
			}
		}

		m_pathExistenceHashMap.insert({ path, currentNode });
	}
}

bool tpp::VariableGroupTree::Exists(const std::string& path)
{
	return m_pathExistenceHashMap.find(path) != m_pathExistenceHashMap.end();
}

void tpp::VariableGroupTree::Clear()
{
	m_root = VariableGroupNode("");
	m_pathExistenceHashMap.clear();
}

void tpp::ServerVariableManager::AddVariable(const Variable& variable)
{
	// TODO Make this string_view and function take a string_view
	std::string groupPath = std::string(variable.groupPath);

	m_variableGroupTree.AddPath(groupPath);

	VariableGroup* variableGroup = nullptr;

	auto variableGroupIterator = m_variableGroupHashMap.find(groupPath);

	if (variableGroupIterator == m_variableGroupHashMap.end())
	{
		variableGroupIterator = m_variableGroupHashMap.insert({ groupPath, VariableGroup() }).first;
	}

	variableGroup = &variableGroupIterator->second;

	const Variable& insertedVariable = m_variableHashMap.insert({ variable.path, variable }).first->second;

	// Insert a pointer to the variable that we inserted (as a copy)
	variableGroup->variables.push_back(&insertedVariable);
}

const tpp::Variable& tpp::ServerVariableManager::GetVariable(const std::string& path) const
{
	auto variable = m_variableHashMap.find(path);

	if (variable != m_variableHashMap.end())
	{
		return variable->second;
	}
	else
	{
		return Dummy;
	}
}

void tpp::ServerVariableManager::Clear()
{
	m_variableGroupTree.Clear();

	m_variableGroupHashMap.clear();

	m_variableHashMap.clear();
}

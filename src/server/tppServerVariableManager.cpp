#include "tppServerVariableManager.h"

#include "tppHashIdTypes.h"
#include "tppNetwork.h"

#include <memory>

namespace tpp
{
	std::unique_ptr<ServerVariableManager> GlobalClientVariableManager;

	void ServerVariableManager::Register(const tpp::VariableDescription& variableDescription)
	{
		// Add variable description to the main table
		m_variableHashmap.insert({ variableDescription.hash, variableDescription });

		// Add variable declaration operation
		m_pendingOperations.push_back({ variableDescription.hash, variableDescription.path, tpp::MessageType::Declaration });
	}

	void ServerVariableManager::Unregister(const tpp::VariableDescription& variableDescription)
	{
		// Add the deletion operation first
		m_pendingOperations.push_back({ variableDescription.hash, variableDescription.path, tpp::MessageType::Deletion });

		// Then delete the data
		m_variableHashmap.erase(variableDescription.hash);
	}

	const tpp::VariableDescription* ServerVariableManager::Find(const tpp::VariableId& id) const
	{
		auto dataIterator = m_variableHashmap.find(id);

		if (dataIterator != m_variableHashmap.end())
		{
			return &dataIterator->second;
		}
		else
		{
			return nullptr;
		}
	}

	bool ServerVariableManager::HasPendingOperations() const
	{
		return !m_pendingOperations.empty();
	}

	void ServerVariableManager::ClearPendingOperations()
	{
		m_pendingOperations.clear();
	}

	void ServerVariableManager::ResetVariableDescriptions()
	{
		ClearPendingOperations();

		for (const auto& it : m_variableHashmap)
		{
			m_pendingOperations.push_back({ it.first, it.second.path, tpp::MessageType::Declaration});
		}
	}

	tpp::ServerVariableManager* GetServerVariableManager()
	{
		if (!GlobalClientVariableManager)
		{
			GlobalClientVariableManager = std::unique_ptr<ServerVariableManager>(new ServerVariableManager());
		}

		return GlobalClientVariableManager.get();
	}
}
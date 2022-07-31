#include "tppClientVariableManager.h"

#include "tppHash.h"

#include <memory>

namespace tpp
{
	std::unique_ptr<ClientVariableManager> GlobalClientVariableManager;

	void ClientVariableManager::Register(const tpp::VariableDescription& variableDescription)
	{
		tpp::Hash hash(variableDescription.path.data(), variableDescription.path.size());

		m_variableHashmap.insert({ hash, variableDescription });
	}

	const tpp::Variable& ClientVariableManager::Find(const tpp::Hash& hash) const
	{
		auto dataIterator = m_variableHashmap.find(hash);

		if (dataIterator != m_variableHashmap.end())
		{
			return dataIterator->second.variable;
		}
		else
		{
			return Dummy;
		}
	}

	tpp::ClientVariableManager* GetClientVariableManager()
	{
		if (!GlobalClientVariableManager)
		{
			GlobalClientVariableManager = std::unique_ptr<ClientVariableManager>(new ClientVariableManager());
		}

		return GlobalClientVariableManager.get();
	}
}
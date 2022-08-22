#include "tppServerVariableManager.h"

#include "tppHash.h"

#include <memory>

namespace tpp
{
	std::unique_ptr<ServerVariableManager> GlobalClientVariableManager;

	void ServerVariableManager::Register(const tpp::VariableDescription& variableDescription)
	{
		tpp::Hash hash(variableDescription.path.data(), variableDescription.path.size());

		m_variableHashmap.insert({ hash, variableDescription });
	}

	tpp::VariableBase* ServerVariableManager::Find(const tpp::Hash& hash) const
	{
		auto dataIterator = m_variableHashmap.find(hash);

		if (dataIterator != m_variableHashmap.end())
		{
			return dataIterator->second.variable;
		}
		else
		{
			return nullptr;
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
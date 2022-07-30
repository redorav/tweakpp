#include "tppClientVariableManager.h"

namespace tpp
{
	std::unique_ptr<ClientVariableManager> GlobalClientVariableManager;

	void ClientVariableManager::Register(const std::string& path, const tpp::Variable& data)
	{
		m_hashMap.insert({ path, data });
	}

	const tpp::Variable& ClientVariableManager::Find(const std::string& path) const
	{
		auto dataIterator = m_hashMap.find(path);

		if (dataIterator != m_hashMap.end())
		{
			return dataIterator->second;
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
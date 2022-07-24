#include "tppClientVariableManager.h"

namespace tpp
{
	std::unique_ptr<ClientVariableManager> GlobalClientVariableManager;

	tpp::ClientVariableManager* GetClientVariableManager()
	{
		if (!GlobalClientVariableManager)
		{
			GlobalClientVariableManager = std::unique_ptr<ClientVariableManager>(new ClientVariableManager());
		}

		return GlobalClientVariableManager.get();
	}
}
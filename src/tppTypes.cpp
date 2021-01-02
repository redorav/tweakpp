#include "tppTypes.h"

namespace tpp
{
	#if !defined(TPP_SERVER)
	std::unique_ptr<ClientVariableManager> GlobalClientVariableManager;
	#endif

	Float::Float(const char* path, float initialValue, float minValue, float maxValue, float step)
		: currentValue(initialValue)
		, minValue(minValue)
		, maxValue(maxValue)
		, step(step)
	{
		#if !defined(TPP_SERVER)
		InitializeGlobalClientVariableManager();
		tpp::Variable variable;
		variable.size = sizeof(currentValue);
		variable.memory = &currentValue;
		variable.vdFloat = *this;
		GlobalClientVariableManager->Register(std::string(path), variable);
		#endif
	}
}
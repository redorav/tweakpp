#include "tppTypes.h"

namespace tpp
{
	#if !defined(TPP_SERVER)
	std::unique_ptr<ClientVariableManager> GlobalClientVariableManager;
	#endif

	Float::Float(const char* path, float initialValue, float minValue, float maxValue, float step)
		: m_currentValue(initialValue)
		, m_minValue(minValue)
		, m_maxValue(maxValue)
		, m_step(step)
	{
		#if !defined(TPP_SERVER)
		InitializeGlobalClientVariableManager();
		tpp::Variable variable;
		variable.size = sizeof(float);
		variable.memory = &m_currentValue;
		variable.vdFloat = *this;
		GlobalClientVariableManager->Register(std::string(path), variable);
		#endif
	}
}
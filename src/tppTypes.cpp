#include "tppTypes.h"

namespace tpp
{
	std::unique_ptr<VariableManager> GlobalVariableManager;

	Float::Float(const char* path, float initialValue, float minValue, float maxValue, float increment)
	{
		InitializeGlobalVariableManager();

		Data data = { &m_f, sizeof(float) };
		GlobalVariableManager->Register(std::string(path), data);
	}
}
#include "tppTypes.h"

#if !defined(TPP_SERVER)
#include "client/tppClientVariableManager.h"
#endif

namespace tpp
{
	Float::Float(const char* path, float initialValue, float minValue, float maxValue, float step)
		: currentValue(initialValue)
		, minValue(minValue)
		, maxValue(maxValue)
		, step(step)
	{
		#if !defined(TPP_SERVER)
		InitializeGlobalClientVariableManager();
		tpp::Variable variable(tpp::VariableType::Float);
		variable.size = sizeof(currentValue);
		variable.memory = &currentValue;
		variable.vdFloat = *this;
		GlobalClientVariableManager->Register(std::string(path), variable);
		#endif
	}

	UInt::UInt(const char* path, uint32_t initialValue, uint32_t minValue, uint32_t maxValue, uint32_t step)
		: currentValue(initialValue)
		, minValue(minValue)
		, maxValue(maxValue)
		, step(step)
	{
		#if !defined(TPP_SERVER)
		InitializeGlobalClientVariableManager();
		tpp::Variable variable(tpp::VariableType::UnsignedInteger);
		variable.size = sizeof(currentValue);
		variable.memory = &currentValue;
		variable.vdUInt = *this;
		GlobalClientVariableManager->Register(std::string(path), variable);
		#endif
	}

	Int::Int(const char* path, int32_t initialValue, int32_t minValue, int32_t maxValue, int32_t step)
		: currentValue(initialValue)
		, minValue(minValue)
		, maxValue(maxValue)
		, step(step)
	{
		#if !defined(TPP_SERVER)
		InitializeGlobalClientVariableManager();
		tpp::Variable variable(tpp::VariableType::Integer);
		variable.size = sizeof(currentValue);
		variable.memory = &currentValue;
		variable.vdInt = *this;
		GlobalClientVariableManager->Register(std::string(path), variable);
		#endif
	}

	Bool::Bool(const char* path, bool initialValue)
		: currentValue(initialValue)
	{
		#if !defined(TPP_SERVER)
		InitializeGlobalClientVariableManager();
		tpp::Variable variable(tpp::VariableType::Bool);
		variable.size = sizeof(currentValue);
		variable.memory = &currentValue;
		variable.vdBool = *this;
		GlobalClientVariableManager->Register(std::string(path), variable);
		#endif
	}

	Color3::Color3(const char* path, float r, float g, float b)
	{
		#if !defined(TPP_SERVER)
		InitializeGlobalClientVariableManager();
		tpp::Variable variable(tpp::VariableType::Color3);
		variable.size = sizeof(currentValue);
		variable.memory = currentValue;
		variable.vdColor3 = *this;
		GlobalClientVariableManager->Register(std::string(path), variable);
		#endif
	}

	Color4::Color4(const char* path, float r, float g, float b, float a)
	{
		#if !defined(TPP_SERVER)
		InitializeGlobalClientVariableManager();
		tpp::Variable variable(tpp::VariableType::Color4);
		variable.size = sizeof(currentValue);
		variable.memory = currentValue;
		variable.vdColor4 = *this;
		GlobalClientVariableManager->Register(std::string(path), variable);
		#endif
	}

	Vector3::Vector3(const char* path, float r, float g, float b)
	{
		#if !defined(TPP_SERVER)
		InitializeGlobalClientVariableManager();
		tpp::Variable variable(tpp::VariableType::Vector3);
		variable.size = sizeof(currentValue);
		variable.memory = currentValue;
		variable.vdVector3 = *this;
		GlobalClientVariableManager->Register(std::string(path), variable);
		#endif
	}

	Vector4::Vector4(const char* path, float r, float g, float b, float a)
	{
		#if !defined(TPP_SERVER)
		InitializeGlobalClientVariableManager();
		tpp::Variable variable(tpp::VariableType::Vector4);
		variable.size = sizeof(currentValue);
		variable.memory = currentValue;
		variable.vdVector4 = *this;
		GlobalClientVariableManager->Register(std::string(path), variable);
		#endif
	}
}
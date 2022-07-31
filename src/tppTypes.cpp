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
		tpp::Variable variable(tpp::VariableType::Float);
		variable.size = sizeof(currentValue);
		variable.memory = &currentValue;
		variable.vdFloat = *this;
		GetClientVariableManager()->Register(tpp::VariableDescription(variable, std::string(path)));
		#endif
	}

	UInt::UInt(const char* path, uint32_t initialValue, uint32_t minValue, uint32_t maxValue, uint32_t step)
		: currentValue(initialValue)
		, minValue(minValue)
		, maxValue(maxValue)
		, step(step)
	{
		#if !defined(TPP_SERVER)
		tpp::Variable variable(tpp::VariableType::UnsignedInteger);
		variable.size = sizeof(currentValue);
		variable.memory = &currentValue;
		variable.vdUInt = *this;
		GetClientVariableManager()->Register(tpp::VariableDescription(variable, std::string(path)));
		#endif
	}

	Int::Int(const char* path, int32_t initialValue, int32_t minValue, int32_t maxValue, int32_t step)
		: currentValue(initialValue)
		, minValue(minValue)
		, maxValue(maxValue)
		, step(step)
	{
		#if !defined(TPP_SERVER)
		tpp::Variable variable(tpp::VariableType::Integer);
		variable.size = sizeof(currentValue);
		variable.memory = &currentValue;
		variable.vdInt = *this;
		GetClientVariableManager()->Register(tpp::VariableDescription(variable, std::string(path)));
		#endif
	}

	Bool::Bool(const char* path, bool initialValue)
		: currentValue(initialValue)
	{
		#if !defined(TPP_SERVER)
		tpp::Variable variable(tpp::VariableType::Bool);
		variable.size = sizeof(currentValue);
		variable.memory = &currentValue;
		variable.vdBool = *this;
		GetClientVariableManager()->Register(tpp::VariableDescription(variable, std::string(path)));
		#endif
	}

	Color3::Color3(const char* path, float r, float g, float b)
		: r(r), g(g), b(b)
	{
		#if !defined(TPP_SERVER)
		tpp::Variable variable(tpp::VariableType::Color3);
		variable.size = sizeof(currentValue);
		variable.memory = &currentValue;
		variable.vdColor3 = *this;
		GetClientVariableManager()->Register(tpp::VariableDescription(variable, std::string(path)));
		#endif
	}

	Color4::Color4(const char* path, float r, float g, float b, float a)
		: r(r), g(g), b(b), a(a)
	{
		#if !defined(TPP_SERVER)
		tpp::Variable variable(tpp::VariableType::Color4);
		variable.size = sizeof(currentValue);
		variable.memory = &currentValue;
		variable.vdColor4 = *this;
		GetClientVariableManager()->Register(tpp::VariableDescription(variable, std::string(path)));
		#endif
	}

	Vector2::Vector2(const char* path, float x, float y)
		: x(x), y(y)
	{
#if !defined(TPP_SERVER)
		tpp::Variable variable(tpp::VariableType::Vector2);
		variable.size = sizeof(currentValue);
		variable.memory = &currentValue;
		variable.vdVector2 = *this;
		GetClientVariableManager()->Register(tpp::VariableDescription(variable, std::string(path)));
#endif
	}

	Vector3::Vector3(const char* path, float x, float y, float z)
		: x(x), y(y), z(z)
	{
		#if !defined(TPP_SERVER)
		tpp::Variable variable(tpp::VariableType::Vector3);
		variable.size = sizeof(currentValue);
		variable.memory = &currentValue;
		variable.vdVector3 = *this;
		GetClientVariableManager()->Register(tpp::VariableDescription(variable, std::string(path)));
		#endif
	}

	Vector4::Vector4(const char* path, float x, float y, float z, float w)
		: x(x), y(y), z(z), w(w)
	{
		#if !defined(TPP_SERVER)
		tpp::Variable variable(tpp::VariableType::Vector4);
		variable.size = sizeof(currentValue);
		variable.memory = &currentValue;
		variable.vdVector4 = *this;
		GetClientVariableManager()->Register(tpp::VariableDescription(variable, std::string(path)));
		#endif
	}

	Callback::Callback(const char* path, void(*callback)(void))
		: currentValue(callback)
	{
		#if !defined(TPP_SERVER)
		tpp::Variable variable(tpp::VariableType::Callback);
		variable.size = 0;
		variable.memory = callback;
		variable.vdCallback = *this;
		GetClientVariableManager()->Register(tpp::VariableDescription(variable, std::string(path)));
		#endif
	}
}
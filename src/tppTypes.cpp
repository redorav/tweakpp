#include "tppTypes.h"

#if !defined(TPP_CLIENT)
#include "server/tppServerVariableManager.h"
#endif

namespace tpp
{
	Float::Float(const char* path, float initialValue, float minValue, float maxValue, float step)
		: VariableBase((VariableType)Type)

		, currentValue(initialValue)
		, metadata(initialValue, minValue, maxValue, step)
	{
		size = sizeof(currentValue);

		#if !defined(TPP_CLIENT)
		memory = &currentValue;
		GetServerVariableManager()->Register(tpp::VariableDescription(this, std::string(path)));
		#endif
	}

	void Float::SerializeMetadata(tpp::BinarySerializationWriter& writer) const { writer << metadata; }
	void Float::DeserializeMetadata(tpp::BinarySerializationReader& reader) { reader << metadata; currentValue = metadata.defaultValue; }

	void Float::SerializeValue(tpp::BinarySerializationWriter& writer) const { writer << currentValue; }
	void Float::DeserializeValue(tpp::BinarySerializationReader& reader) { reader << currentValue; }

	UInt::UInt(const char* path, uint32_t initialValue, uint32_t minValue, uint32_t maxValue, uint32_t step)
		: VariableBase((VariableType)Type)
		, currentValue(initialValue)
		, metadata(initialValue, minValue, maxValue, step)
	{
		size = sizeof(currentValue);

		#if !defined(TPP_CLIENT)
		memory = &currentValue;
		GetServerVariableManager()->Register(tpp::VariableDescription(this, std::string(path)));
		#endif
	}

	void UInt::SerializeMetadata(tpp::BinarySerializationWriter& writer) const { writer << metadata; }
	void UInt::DeserializeMetadata(tpp::BinarySerializationReader& reader) { reader << metadata; currentValue = metadata.defaultValue; }

	void UInt::SerializeValue(tpp::BinarySerializationWriter& writer) const { writer << currentValue; }
	void UInt::DeserializeValue(tpp::BinarySerializationReader& reader) { reader << currentValue; }

	Int::Int(const char* path, int32_t initialValue, int32_t minValue, int32_t maxValue, int32_t step)
		: VariableBase((VariableType)Type)
		, currentValue(initialValue)
		, metadata(initialValue, minValue, maxValue, step)
	{
		size = sizeof(currentValue);

		#if !defined(TPP_CLIENT)
		memory = &currentValue;
		GetServerVariableManager()->Register(tpp::VariableDescription(this, std::string(path)));
		#endif
	}

	void Int::SerializeMetadata(tpp::BinarySerializationWriter& writer) const { writer << metadata; }
	void Int::DeserializeMetadata(tpp::BinarySerializationReader& reader) { reader << metadata; currentValue = metadata.defaultValue; }

	void Int::SerializeValue(tpp::BinarySerializationWriter& writer) const { writer << currentValue; }
	void Int::DeserializeValue(tpp::BinarySerializationReader& reader) { reader << currentValue; }

	Bool::Bool(const char* path, bool initialValue)
		: VariableBase((VariableType)Type)
		, currentValue(initialValue)
		, metadata(initialValue)
	{
		size = sizeof(currentValue);

		#if !defined(TPP_CLIENT)
		memory = &currentValue;
		GetServerVariableManager()->Register(tpp::VariableDescription(this, std::string(path)));
		#endif
	}

	void Bool::SerializeMetadata(tpp::BinarySerializationWriter& writer) const { writer << metadata; }
	void Bool::DeserializeMetadata(tpp::BinarySerializationReader& reader) { reader << metadata; currentValue = metadata.defaultValue; }

	void Bool::SerializeValue(tpp::BinarySerializationWriter& writer) const { writer << currentValue; }
	void Bool::DeserializeValue(tpp::BinarySerializationReader& reader) { reader << currentValue; }

	Color3::Color3(const char* path, float r, float g, float b)
		: VariableBase((VariableType)Type)
		, r(r), g(g), b(b)
		, metadata(r, g, b)
	{
		size = sizeof(currentValue);

		#if !defined(TPP_CLIENT)
		memory = &currentValue;
		GetServerVariableManager()->Register(tpp::VariableDescription(this, std::string(path)));
		#endif
	}

	void Color3::SerializeMetadata(tpp::BinarySerializationWriter& writer) const { writer << metadata; }
	void Color3::DeserializeMetadata(tpp::BinarySerializationReader& reader) { reader << metadata; currentValue = metadata.defaultValue; }

	void Color3::SerializeValue(tpp::BinarySerializationWriter& writer) const { writer << currentValue; }
	void Color3::DeserializeValue(tpp::BinarySerializationReader& reader) { reader << currentValue; }

	Color4::Color4(const char* path, float r, float g, float b, float a)
		: VariableBase((VariableType)Type)
		, r(r), g(g), b(b), a(a)
		, metadata(r, g, b, a)
	{
		size = sizeof(currentValue);

		#if !defined(TPP_CLIENT)
		memory = &currentValue;
		GetServerVariableManager()->Register(tpp::VariableDescription(this, std::string(path)));
		#endif
	}

	void Color4::SerializeMetadata(tpp::BinarySerializationWriter& writer) const { writer << metadata; }
	void Color4::DeserializeMetadata(tpp::BinarySerializationReader& reader) { reader << metadata; currentValue = metadata.defaultValue; }

	void Color4::SerializeValue(tpp::BinarySerializationWriter& writer) const { writer << currentValue; }
	void Color4::DeserializeValue(tpp::BinarySerializationReader& reader) { reader << currentValue; }

	Vector2::Vector2(const char* path, float x, float y)
		: VariableBase((VariableType)Type)
		, x(x), y(y)
		, metadata(x, y)
	{
		size = sizeof(currentValue);

		#if !defined(TPP_CLIENT)
		memory = &currentValue;
		GetServerVariableManager()->Register(tpp::VariableDescription(this, std::string(path)));
		#endif
	}

	void Vector2::SerializeMetadata(tpp::BinarySerializationWriter& writer) const { writer << metadata; }
	void Vector2::DeserializeMetadata(tpp::BinarySerializationReader& reader) { reader << metadata; currentValue = metadata.defaultValue; }

	void Vector2::SerializeValue(tpp::BinarySerializationWriter& writer) const { writer << currentValue; }
	void Vector2::DeserializeValue(tpp::BinarySerializationReader& reader) { reader << currentValue; }

	Vector3::Vector3(const char* path, float x, float y, float z)
		: VariableBase((VariableType)Type)
		, x(x), y(y), z(z)
		, metadata(x, y, z)
	{
		size = sizeof(currentValue);

		#if !defined(TPP_CLIENT)
		memory = &currentValue;
		GetServerVariableManager()->Register(tpp::VariableDescription(this, std::string(path)));
		#endif
	}

	void Vector3::SerializeMetadata(tpp::BinarySerializationWriter& writer) const { writer << metadata; }
	void Vector3::DeserializeMetadata(tpp::BinarySerializationReader& reader) { reader << metadata; currentValue = metadata.defaultValue; }

	void Vector3::SerializeValue(tpp::BinarySerializationWriter& writer) const { writer << currentValue; }
	void Vector3::DeserializeValue(tpp::BinarySerializationReader& reader) { reader << currentValue; }

	Vector4::Vector4(const char* path, float x, float y, float z, float w)
		: VariableBase((VariableType)Type)
		, x(x), y(y), z(z), w(w)
		, metadata(x, y, z, w)
	{
		size = sizeof(currentValue);

		#if !defined(TPP_CLIENT)
		memory = &currentValue;
		GetServerVariableManager()->Register(tpp::VariableDescription(this, std::string(path)));
		#endif
	}

	void Vector4::SerializeMetadata(tpp::BinarySerializationWriter& writer) const { writer << metadata; }
	void Vector4::DeserializeMetadata(tpp::BinarySerializationReader& reader) { reader << metadata; currentValue = metadata.defaultValue; }

	void Vector4::SerializeValue(tpp::BinarySerializationWriter& writer) const { writer << currentValue; }
	void Vector4::DeserializeValue(tpp::BinarySerializationReader& reader) { reader << currentValue; }

	Callback::Callback(const char* path, void(*callback)(void))
		: VariableBase((VariableType)Type)
		, currentValue(callback)
	{
		size = sizeof(currentValue);

		#if !defined(TPP_CLIENT)
		memory = &currentValue;
		GetServerVariableManager()->Register(tpp::VariableDescription(this, std::string(path)));
		#endif
	}
	
	void Callback::SerializeMetadata(tpp::BinarySerializationWriter& writer) const {}
	void Callback::DeserializeMetadata(tpp::BinarySerializationReader& reader) {}

	void Callback::SerializeValue(tpp::BinarySerializationWriter& writer) const {}
	void Callback::DeserializeValue(tpp::BinarySerializationReader& reader) {}
}

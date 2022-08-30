#include "tppTypes.h"

#if !defined(TPP_CLIENT)
#include "server/tppServerVariableManager.h"
#endif

namespace tpp
{
	Float::Float(const char* path, float initialValue, float minValue, float maxValue, float step)
		: VariableBase((VariableType)Type, sizeof(currentValue))
		, currentValue(initialValue)
		, metadata(initialValue, minValue, maxValue, step)
	{
		#if !defined(TPP_CLIENT)
		memory = &currentValue;
		GetServerVariableManager()->Register(tpp::VariableDescription(this, std::string(path)));
		#endif
	}

	void Float::SerializeMetadata(tpp::BinarySerializationWriter& writer) const { writer << metadata; }
	void Float::DeserializeMetadata(tpp::BinarySerializationReader& reader) { reader << metadata; currentValue = metadata.defaultValue; }

	void Float::SerializeValue(tpp::BinarySerializationWriter& writer) const { writer << currentValue; }
	void Float::DeserializeValue(tpp::BinarySerializationReader& reader) { reader << currentValue; }

	void Float::RevertToDefault() { currentValue = metadata.defaultValue; }
	bool Float::HasDefaultValue() { return currentValue == metadata.defaultValue; }

	UInt::UInt(const char* path, uint32_t initialValue, uint32_t minValue, uint32_t maxValue, uint32_t step)
		: VariableBase((VariableType)Type, sizeof(currentValue))
		, currentValue(initialValue)
		, metadata(initialValue, minValue, maxValue, step)
	{
		#if !defined(TPP_CLIENT)
		memory = &currentValue;
		GetServerVariableManager()->Register(tpp::VariableDescription(this, std::string(path)));
		#endif
	}

	void UInt::SerializeMetadata(tpp::BinarySerializationWriter& writer) const { writer << metadata; }
	void UInt::DeserializeMetadata(tpp::BinarySerializationReader& reader) { reader << metadata; currentValue = metadata.defaultValue; }

	void UInt::SerializeValue(tpp::BinarySerializationWriter& writer) const { writer << currentValue; }
	void UInt::DeserializeValue(tpp::BinarySerializationReader& reader) { reader << currentValue; }

	void UInt::RevertToDefault() { currentValue = metadata.defaultValue; }
	bool UInt::HasDefaultValue() { return currentValue == metadata.defaultValue; }

	Int::Int(const char* path, int32_t initialValue, int32_t minValue, int32_t maxValue, int32_t step)
		: VariableBase((VariableType)Type, sizeof(currentValue))
		, currentValue(initialValue)
		, metadata(initialValue, minValue, maxValue, step)
	{
		#if !defined(TPP_CLIENT)
		memory = &currentValue;
		GetServerVariableManager()->Register(tpp::VariableDescription(this, std::string(path)));
		#endif
	}

	void Int::SerializeMetadata(tpp::BinarySerializationWriter& writer) const { writer << metadata; }
	void Int::DeserializeMetadata(tpp::BinarySerializationReader& reader) { reader << metadata; currentValue = metadata.defaultValue; }

	void Int::SerializeValue(tpp::BinarySerializationWriter& writer) const { writer << currentValue; }
	void Int::DeserializeValue(tpp::BinarySerializationReader& reader) { reader << currentValue; }

	void Int::RevertToDefault() { currentValue = metadata.defaultValue; }
	bool Int::HasDefaultValue() { return currentValue == metadata.defaultValue; }

	Bool::Bool(const char* path, bool initialValue)
		: VariableBase((VariableType)Type, sizeof(currentValue))
		, currentValue(initialValue)
		, metadata(initialValue)
	{
		#if !defined(TPP_CLIENT)
		memory = &currentValue;
		GetServerVariableManager()->Register(tpp::VariableDescription(this, std::string(path)));
		#endif
	}

	void Bool::SerializeMetadata(tpp::BinarySerializationWriter& writer) const { writer << metadata; }
	void Bool::DeserializeMetadata(tpp::BinarySerializationReader& reader) { reader << metadata; currentValue = metadata.defaultValue; }

	void Bool::SerializeValue(tpp::BinarySerializationWriter& writer) const { writer << currentValue; }
	void Bool::DeserializeValue(tpp::BinarySerializationReader& reader) { reader << currentValue; }

	void Bool::RevertToDefault() { currentValue = metadata.defaultValue; }
	bool Bool::HasDefaultValue() { return currentValue == metadata.defaultValue; }

	Color3::Color3(const char* path, float r, float g, float b)
		: VariableBase((VariableType)Type, sizeof(currentValue))
		, r(r), g(g), b(b)
		, metadata(r, g, b)
	{
		#if !defined(TPP_CLIENT)
		memory = &currentValue;
		GetServerVariableManager()->Register(tpp::VariableDescription(this, std::string(path)));
		#endif
	}

	void Color3::SerializeMetadata(tpp::BinarySerializationWriter& writer) const { writer << metadata; }
	void Color3::DeserializeMetadata(tpp::BinarySerializationReader& reader) { reader << metadata; currentValue = metadata.defaultValue; }

	void Color3::SerializeValue(tpp::BinarySerializationWriter& writer) const { writer << currentValue; }
	void Color3::DeserializeValue(tpp::BinarySerializationReader& reader) { reader << currentValue; }

	void Color3::RevertToDefault() { currentValue = metadata.defaultValue; }
	bool Color3::HasDefaultValue() { return currentValue == metadata.defaultValue; }

	Color4::Color4(const char* path, float r, float g, float b, float a)
		: VariableBase((VariableType)Type, sizeof(currentValue))
		, r(r), g(g), b(b), a(a)
		, metadata(r, g, b, a)
	{
		#if !defined(TPP_CLIENT)
		memory = &currentValue;
		GetServerVariableManager()->Register(tpp::VariableDescription(this, std::string(path)));
		#endif
	}

	void Color4::SerializeMetadata(tpp::BinarySerializationWriter& writer) const { writer << metadata; }
	void Color4::DeserializeMetadata(tpp::BinarySerializationReader& reader) { reader << metadata; currentValue = metadata.defaultValue; }

	void Color4::SerializeValue(tpp::BinarySerializationWriter& writer) const { writer << currentValue; }
	void Color4::DeserializeValue(tpp::BinarySerializationReader& reader) { reader << currentValue; }

	void Color4::RevertToDefault() { currentValue = metadata.defaultValue; }
	bool Color4::HasDefaultValue() { return currentValue == metadata.defaultValue; }

	Vector2::Vector2(const char* path, float x, float y)
		: VariableBase((VariableType)Type, sizeof(currentValue))
		, x(x), y(y)
		, metadata(x, y)
	{
		#if !defined(TPP_CLIENT)
		memory = &currentValue;
		GetServerVariableManager()->Register(tpp::VariableDescription(this, std::string(path)));
		#endif
	}

	void Vector2::SerializeMetadata(tpp::BinarySerializationWriter& writer) const { writer << metadata; }
	void Vector2::DeserializeMetadata(tpp::BinarySerializationReader& reader) { reader << metadata; currentValue = metadata.defaultValue; }

	void Vector2::SerializeValue(tpp::BinarySerializationWriter& writer) const { writer << currentValue; }
	void Vector2::DeserializeValue(tpp::BinarySerializationReader& reader) { reader << currentValue; }

	void Vector2::RevertToDefault() { currentValue = metadata.defaultValue; }
	bool Vector2::HasDefaultValue() { return currentValue == metadata.defaultValue; }

	Vector3::Vector3(const char* path, float x, float y, float z)
		: VariableBase((VariableType)Type, sizeof(currentValue))
		, x(x), y(y), z(z)
		, metadata(x, y, z)
	{
		#if !defined(TPP_CLIENT)
		memory = &currentValue;
		GetServerVariableManager()->Register(tpp::VariableDescription(this, std::string(path)));
		#endif
	}

	void Vector3::SerializeMetadata(tpp::BinarySerializationWriter& writer) const { writer << metadata; }
	void Vector3::DeserializeMetadata(tpp::BinarySerializationReader& reader) { reader << metadata; currentValue = metadata.defaultValue; }

	void Vector3::SerializeValue(tpp::BinarySerializationWriter& writer) const { writer << currentValue; }
	void Vector3::DeserializeValue(tpp::BinarySerializationReader& reader) { reader << currentValue; }

	void Vector3::RevertToDefault() { currentValue = metadata.defaultValue; }
	bool Vector3::HasDefaultValue() { return currentValue == metadata.defaultValue; }

	Vector4::Vector4(const char* path, float x, float y, float z, float w)
		: VariableBase((VariableType)Type, sizeof(currentValue))
		, x(x), y(y), z(z), w(w)
		, metadata(x, y, z, w)
	{
		#if !defined(TPP_CLIENT)
		memory = &currentValue;
		GetServerVariableManager()->Register(tpp::VariableDescription(this, std::string(path)));
		#endif
	}

	void Vector4::SerializeMetadata(tpp::BinarySerializationWriter& writer) const { writer << metadata; }
	void Vector4::DeserializeMetadata(tpp::BinarySerializationReader& reader) { reader << metadata; currentValue = metadata.defaultValue; }

	void Vector4::SerializeValue(tpp::BinarySerializationWriter& writer) const { writer << currentValue; }
	void Vector4::DeserializeValue(tpp::BinarySerializationReader& reader) { reader << currentValue; }

	void Vector4::RevertToDefault() { currentValue = metadata.defaultValue; }
	bool Vector4::HasDefaultValue() { return currentValue == metadata.defaultValue; }

	Callback::Callback(const char* path, void(*callback)(void))
		: VariableBase((VariableType)Type, 0)
		, currentValue(callback)
	{
		#if !defined(TPP_CLIENT)
		memory = &currentValue;
		GetServerVariableManager()->Register(tpp::VariableDescription(this, std::string(path)));
		#endif
	}
	
	void Callback::SerializeMetadata(tpp::BinarySerializationWriter& writer) const {}
	void Callback::DeserializeMetadata(tpp::BinarySerializationReader& reader) {}

	void Callback::SerializeValue(tpp::BinarySerializationWriter& writer) const {}
	void Callback::DeserializeValue(tpp::BinarySerializationReader& reader)
	{
		// Deserializing the value means calling the function
		currentValue();
	}

	void Callback::RevertToDefault() {}
	bool Callback::HasDefaultValue() { return true; }

	String::String(const char* path, const char* defaultValue)
		: VariableBase((VariableType)Type, 0)
		, currentValue(defaultValue)
	{
		#if !defined(TPP_CLIENT)
		GetServerVariableManager()->Register(tpp::VariableDescription(this, std::string(path)));
		#endif
	}

	void String::SerializeMetadata(tpp::BinarySerializationWriter& writer) const
	{
		writer << defaultValue;
	}

	void String::DeserializeMetadata(tpp::BinarySerializationReader& reader)
	{
		reader << defaultValue;
		currentValue = defaultValue;
	}

	void String::SerializeValue(tpp::BinarySerializationWriter& writer) const { writer << currentValue; }
	void String::DeserializeValue(tpp::BinarySerializationReader& reader) { reader << currentValue; }

	void String::RevertToDefault() { currentValue = defaultValue; }
	bool String::HasDefaultValue() { return currentValue == defaultValue; }

	Enum::Enum(const char* path, int defaultValue, const std::vector<EnumEntry>& entries)
		: VariableBase((VariableType)Type, sizeof(currentValue))
		, currentValue(defaultValue)
	{
		metadata.defaultValue = defaultValue;
		metadata.entries = entries;

		#if !defined(TPP_CLIENT)
		memory = &currentValue;
		GetServerVariableManager()->Register(tpp::VariableDescription(this, std::string(path)));
		#endif
	}

	void Enum::SerializeMetadata(tpp::BinarySerializationWriter& writer) const
	{
		writer << metadata.defaultValue;
		writer << metadata.entries;
	}

	void Enum::DeserializeMetadata(tpp::BinarySerializationReader& reader)
	{
		reader << metadata.defaultValue;
		reader << metadata.entries;
		currentValue = metadata.defaultValue;
	}

	void Enum::SerializeValue(tpp::BinarySerializationWriter& writer) const { writer << currentValue; }
	void Enum::DeserializeValue(tpp::BinarySerializationReader& reader) { reader << currentValue; }

	void Enum::RevertToDefault() { currentValue = metadata.defaultValue; }
}

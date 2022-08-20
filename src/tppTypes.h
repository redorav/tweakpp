#pragma once

#include "tppCommon.h"

#if defined(TPP_CLIENT)
#include <string>
#include "tppHash.h"
#endif

#include "tppSerialize.h"

namespace tpp
{
	enum class VariableType : uint8_t
	{
		Float = 0,
		UnsignedInteger = 1,
		Integer = 2,
		Bool = 3,
		Color3 = 4,
		Color4 = 5,
		Vector2 = 6,
		Vector3 = 7,
		Vector4 = 8,
		String = 9,
		Enum = 10,
		Callback = 11,
		Invalid = 0xff
	};

	template<typename T, int N>
	struct vector
	{
		T data[N];
	};

	struct FloatMetadata
	{
		FloatMetadata() {}

		FloatMetadata(float defaultValue, float minValue, float maxValue, float step)
			: defaultValue(defaultValue), minValue(minValue), maxValue(maxValue), step(step) {}

		float defaultValue = 0.0f;
		float minValue = 0.0f;
		float maxValue = 0.0f;
		float step = 0.0f;
	};

	class Float
	{
	public:

		Float(const char* path, float initialValue, float minValue, float maxValue, float step);

		void SerializeMetadata(tpp::BinarySerializationWriter& writer) const;
		void DeserializeMetadata(tpp::BinarySerializationReader& reader);

		void SerializeValue(tpp::BinarySerializationWriter& writer) const;
		void DeserializeValue(tpp::BinarySerializationReader& reader);

		operator float()
		{
			return currentValue;
		}

		// Need mutable for the server to modify value through the UI widgets
		mutable float currentValue = 0.0f;

		FloatMetadata metadata;

		enum : uint32_t
		{
			type = VariableType::Float
		};
	};

	struct UIntMetadata
	{
		UIntMetadata(uint32_t defaultValue, uint32_t minValue, uint32_t maxValue, uint32_t step)
			: defaultValue(defaultValue), minValue(minValue), maxValue(maxValue), step(step) {}

		uint32_t defaultValue = 0u;
		uint32_t minValue = 0u;
		uint32_t maxValue = 0u;
		uint32_t step = 0u;
	};

	class UInt
	{
	public:

		UInt(const char* path, uint32_t initialValue, uint32_t minValue, uint32_t maxValue, uint32_t step);

		void SerializeMetadata(tpp::BinarySerializationWriter& writer) const;
		void DeserializeMetadata(tpp::BinarySerializationReader& reader);

		void SerializeValue(tpp::BinarySerializationWriter& writer) const;
		void DeserializeValue(tpp::BinarySerializationReader& reader);

		operator uint32_t()
		{
			return currentValue;
		}

		mutable uint32_t currentValue = 0u;

		UIntMetadata metadata;

		enum : uint32_t
		{
			type = VariableType::UnsignedInteger
		};
	};

	struct IntMetadata
	{
		IntMetadata(int32_t defaultValue, int32_t minValue, int32_t maxValue, int32_t step)
			: defaultValue(defaultValue), minValue(minValue), maxValue(maxValue), step(step) {}

		int32_t defaultValue = 0;
		int32_t minValue = 0;
		int32_t maxValue = 0;
		int32_t step = 0;
	};

	class Int
	{
	public:

		Int(const char* path, int32_t initialValue, int32_t minValue, int32_t maxValue, int32_t step);

		void SerializeMetadata(tpp::BinarySerializationWriter& writer) const;
		void DeserializeMetadata(tpp::BinarySerializationReader& reader);

		void SerializeValue(tpp::BinarySerializationWriter& writer) const;
		void DeserializeValue(tpp::BinarySerializationReader& reader);

		mutable int32_t currentValue = 0;

		IntMetadata metadata;

		enum : uint32_t
		{
			type = VariableType::Integer
		};
	};

	struct BoolMetadata
	{
		BoolMetadata(bool defaultValue) : defaultValue(defaultValue) {}
		bool defaultValue = false;
	};

	class Bool
	{
	public:

		Bool(const char* path, bool initialValue);

		void SerializeMetadata(tpp::BinarySerializationWriter& writer) const;
		void DeserializeMetadata(tpp::BinarySerializationReader& reader);

		void SerializeValue(tpp::BinarySerializationWriter& writer) const;
		void DeserializeValue(tpp::BinarySerializationReader& reader);

		mutable bool currentValue = 0;

		BoolMetadata metadata;

		enum : uint32_t
		{
			type = VariableType::Bool
		};
	};

	struct Color3Metadata
	{
		Color3Metadata(float r, float g, float b) : r(r), g(g), b(b) {}

		union
		{
			struct { float r, g, b; };
			vector<float, 3> defaultValue;
		};
	};
	
	class Color3
	{
	public:

		Color3(const char* path, float r, float g, float b);

		void SerializeMetadata(tpp::BinarySerializationWriter& writer) const;
		void DeserializeMetadata(tpp::BinarySerializationReader& reader);

		void SerializeValue(tpp::BinarySerializationWriter& writer) const;
		void DeserializeValue(tpp::BinarySerializationReader& reader);

		union
		{
			struct { float r, g, b; };
			vector<float, 3> currentValue;
		};

		Color3Metadata metadata;

		enum : uint32_t
		{
			type = VariableType::Color3
		};
	};

	struct Color4Metadata
	{
		Color4Metadata(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}

		union
		{
			struct { float r, g, b, a; };
			vector<float, 4> defaultValue;
		};
	};

	class Color4
	{
	public:

		Color4(const char* path, float r, float g, float b, float a);

		void SerializeMetadata(tpp::BinarySerializationWriter& writer) const;
		void DeserializeMetadata(tpp::BinarySerializationReader& reader);

		void SerializeValue(tpp::BinarySerializationWriter& writer) const;
		void DeserializeValue(tpp::BinarySerializationReader& reader);

		union
		{
			struct { float r, g, b, a; };
			vector<float, 4> currentValue;
		};

		Color4Metadata metadata;

		enum : uint32_t
		{
			type = VariableType::Color4
		};
	};

	struct Vector2Metadata
	{
		Vector2Metadata(float x, float y) : x(x), y(y) {}

		union
		{
			struct { float x, y; };
			vector<float, 2> defaultValue;
		};
	};

	class Vector2
	{
	public:

		Vector2(const char* path, float x, float y);

		void SerializeMetadata(tpp::BinarySerializationWriter& writer) const;
		void DeserializeMetadata(tpp::BinarySerializationReader& reader);

		void SerializeValue(tpp::BinarySerializationWriter& writer) const;
		void DeserializeValue(tpp::BinarySerializationReader& reader);

		union
		{
			struct { float x, y; };
			vector<float, 2> currentValue;
		};

		Vector2Metadata metadata;

		enum : uint32_t
		{
			type = VariableType::Vector2
		};
	};

	struct Vector3Metadata
	{
		Vector3Metadata(float x, float y, float z) : x(x), y(y), z(z) {}
		union
		{
			struct { float x, y, z; };
			vector<float, 3> defaultValue;
		};
	};

	class Vector3
	{
	public:

		Vector3(const char* path, float x, float y, float z);

		void SerializeMetadata(tpp::BinarySerializationWriter& writer) const;
		void DeserializeMetadata(tpp::BinarySerializationReader& reader);

		void SerializeValue(tpp::BinarySerializationWriter& writer) const;
		void DeserializeValue(tpp::BinarySerializationReader& reader);

		union
		{
			struct { float x, y, z; };
			vector<float, 3> currentValue;
		};

		Vector3Metadata metadata;

		enum : uint32_t
		{
			type = VariableType::Vector3
		};
	};

	struct Vector4Metadata
	{
		Vector4Metadata(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

		union
		{
			struct { float x, y, z, w; };
			vector<float, 4> defaultValue;
		};
	};

	class Vector4
	{
	public:

		Vector4(const char* path, float x, float y, float z, float w);

		void SerializeMetadata(tpp::BinarySerializationWriter& writer) const;
		void DeserializeMetadata(tpp::BinarySerializationReader& reader);

		void SerializeValue(tpp::BinarySerializationWriter& writer) const;
		void DeserializeValue(tpp::BinarySerializationReader& reader);

		union
		{
			struct { float x, y, z, w; };
			vector<float, 4> currentValue;
		};

		Vector4Metadata metadata;

		enum : uint32_t
		{
			type = VariableType::Vector4
		};
	};

	class Callback
	{
	public:

		Callback(const char* path, void(*callback)(void));

		enum : uint32_t
		{
			type = VariableType::Callback
		};

		void(*currentValue)(void);
	};

	// Can be any of the allowed types
	class Variable
	{
	public:

#if defined(TPP_CLIENT)

		Variable() {}

		Variable(tpp::VariableType type, const std::string& path, const tpp::Hash& hash)
			: type(type), path(path), hash(hash)
		{
			if (path.size() > 0)
			{
				size_t lastSlash = path.find_last_of("/");
				size_t afterLastSlash = lastSlash + 1;

				if (lastSlash != std::string::npos)
				{
					groupPath = std::string(path.data(), lastSlash);
					name = std::string(path.data() + afterLastSlash, path.size() - afterLastSlash);
				}
			}
		}

		const std::string& GetGroupPath() const
		{
			return groupPath;
		}

		const std::string& GetName() const
		{
			return name;
		}

		std::string path;

		std::string groupPath;

		tpp::Hash hash;

		std::string name;
#else

		Variable(tpp::VariableType type) : type(type) {}

#endif

		void* memory = nullptr;

		uint32_t size = 0;

		tpp::VariableType type = tpp::VariableType::Invalid;

		union
		{
			tpp::Float vdFloat;

			tpp::UInt vdUInt;

			tpp::Int vdInt;

			tpp::Bool vdBool;
			
			tpp::Color3 vdColor3;
			
			tpp::Color4 vdColor4;

			tpp::Vector2 vdVector2;

			tpp::Vector3 vdVector3;

			tpp::Vector4 vdVector4;

			tpp::Callback vdCallback;
		};
	};
}
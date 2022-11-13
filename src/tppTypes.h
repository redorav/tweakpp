#pragma once

#include "tppCommon.h"

#if defined(TPP_CLIENT)
#include "tppHashIdTypes.h"
#endif

#include "tppSerialize.h"

#include <string>

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
		Flags8 = 12,
		Flags16 = 13,
		Flags32 = 14,
		Flags64 = 15,
		Invalid = 0xff
	};

	template<typename T, int N>
	struct array
	{
		bool operator == (const array<T, N>& other)
		{
			for (int i = 0; i < N; ++i)
			{
				if (data[i] != other.data[i])
				{
					return false;
				}
			}

			return true;
		}

		T data[N];
	};

	class VariableBase
	{
	public:

#if defined(TPP_CLIENT)

		void SetPath(const std::string& path)
		{
			m_path = path;

			if (path.size() > 0)
			{
				size_t lastSlash = path.find_last_of("/");
				size_t afterLastSlash = lastSlash + 1;

				if (lastSlash != std::string::npos)
				{
					m_groupPath = std::string(path.data(), lastSlash);
					m_name = std::string(path.data() + afterLastSlash, path.size() - afterLastSlash);
				}
			}
		}

		void SetId(const tpp::VariableId& id)
		{
			m_id = id;
		}

		const std::string& GetPath() const
		{
			return m_path;
		}

		const std::string& GetGroupPath() const
		{
			return m_groupPath;
		}

		const std::string& GetName() const
		{
			return m_name;
		}

		const tpp::VariableId& GetId() const
		{
			return m_id;
		}

	private:

		std::string m_path;

		std::string m_groupPath;

		// Unique identifier sent to us by server (variable pointer)
		tpp::VariableId m_id;

		std::string m_name;

#else

		void* memory = nullptr;

#endif

	public:

		VariableBase(tpp::VariableType type, uint32_t size) : type(type), size(size) {}

		virtual ~VariableBase();

		uint32_t size = 0;

		tpp::VariableType type = tpp::VariableType::Invalid;

		virtual void SerializeMetadata(tpp::BinarySerializationWriter& writer) const = 0;
		virtual void DeserializeMetadata(tpp::BinarySerializationReader & reader) = 0;
		virtual void SerializeValue(tpp::BinarySerializationWriter & writer) const = 0;
		virtual void DeserializeValue(tpp::BinarySerializationReader & reader) = 0;
		virtual void RevertToDefault() = 0;
		virtual bool HasDefaultValue() = 0;
	};

	class Float final : public VariableBase
	{
	public:

#if defined(TPP_CLIENT)
		Float() : VariableBase((VariableType)Type, sizeof(currentValue)) {}
#endif
		Float(const char* path, float initialValue, float minValue, float maxValue, float step);

		virtual ~Float() {}

		virtual void SerializeMetadata(tpp::BinarySerializationWriter& writer) const override;
		virtual void DeserializeMetadata(tpp::BinarySerializationReader& reader) override;
		virtual void SerializeValue(tpp::BinarySerializationWriter& writer) const override;
		virtual void DeserializeValue(tpp::BinarySerializationReader& reader) override;
		virtual void RevertToDefault() override;
		virtual bool HasDefaultValue() override;

		operator float()
		{
			return currentValue;
		}

		// Need mutable for the server to modify value through the UI widgets
		mutable float currentValue = 0.0f;

		struct FloatMetadata
		{
#if defined(TPP_CLIENT)
			FloatMetadata() {}
#endif
			FloatMetadata(float defaultValue, float minValue, float maxValue, float step)
				: defaultValue(defaultValue), minValue(minValue), maxValue(maxValue), step(step) {}

			float defaultValue = 0.0f;
			float minValue = 0.0f;
			float maxValue = 0.0f;
			float step = 0.0f;
		} metadata;

		enum : uint32_t
		{
			Type = VariableType::Float
		};
	};

	class UInt final : public VariableBase
	{
	public:

#if defined(TPP_CLIENT)
		UInt() : VariableBase((VariableType)Type, sizeof(currentValue)) {}
#endif
		UInt(const char* path, uint32_t initialValue, uint32_t minValue, uint32_t maxValue, uint32_t step);

		virtual ~UInt() {}

		virtual void SerializeMetadata(tpp::BinarySerializationWriter& writer) const override;
		virtual void DeserializeMetadata(tpp::BinarySerializationReader& reader) override;
		virtual void SerializeValue(tpp::BinarySerializationWriter& writer) const override;
		virtual void DeserializeValue(tpp::BinarySerializationReader& reader) override;
		virtual void RevertToDefault() override;
		virtual bool HasDefaultValue() override;

		operator uint32_t()
		{
			return currentValue;
		}

		mutable uint32_t currentValue = 0u;

		struct UIntMetadata
		{
#if defined(TPP_CLIENT)
			UIntMetadata() {}
#endif
			UIntMetadata(uint32_t defaultValue, uint32_t minValue, uint32_t maxValue, uint32_t step)
				: defaultValue(defaultValue), minValue(minValue), maxValue(maxValue), step(step) {}

			uint32_t defaultValue = 0u;
			uint32_t minValue = 0u;
			uint32_t maxValue = 0u;
			uint32_t step = 0u;
		} metadata;

		enum : uint32_t
		{
			Type = VariableType::UnsignedInteger
		};
	};

	class Int final : public VariableBase
	{
	public:

#if defined(TPP_CLIENT)
		Int() : VariableBase((VariableType)Type, sizeof(currentValue)) {}
#endif
		Int(const char* path, int32_t initialValue, int32_t minValue, int32_t maxValue, int32_t step);

		virtual ~Int() {}

		virtual void SerializeMetadata(tpp::BinarySerializationWriter& writer) const override;
		virtual void DeserializeMetadata(tpp::BinarySerializationReader& reader) override;
		virtual void SerializeValue(tpp::BinarySerializationWriter& writer) const override;
		virtual void DeserializeValue(tpp::BinarySerializationReader& reader) override;
		virtual void RevertToDefault() override;
		virtual bool HasDefaultValue() override;

		mutable int32_t currentValue = 0;

		struct IntMetadata
		{
#if defined(TPP_CLIENT)
			IntMetadata() {}
#endif
			IntMetadata(int32_t defaultValue, int32_t minValue, int32_t maxValue, int32_t step)
				: defaultValue(defaultValue), minValue(minValue), maxValue(maxValue), step(step) {}

			int32_t defaultValue = 0;
			int32_t minValue = 0;
			int32_t maxValue = 0;
			int32_t step = 0;
		} metadata;

		enum : uint32_t
		{
			Type = VariableType::Integer
		};
	};

	class Bool final : public VariableBase
	{
	public:

#if defined(TPP_CLIENT)
		Bool() : VariableBase((VariableType)Type, sizeof(currentValue)) {}
#endif
		Bool(const char* path, bool initialValue);

		virtual ~Bool() {}

		virtual void SerializeMetadata(tpp::BinarySerializationWriter& writer) const override;
		virtual void DeserializeMetadata(tpp::BinarySerializationReader& reader) override;
		virtual void SerializeValue(tpp::BinarySerializationWriter& writer) const override;
		virtual void DeserializeValue(tpp::BinarySerializationReader& reader) override;
		virtual void RevertToDefault() override;
		virtual bool HasDefaultValue() override;

		mutable bool currentValue = 0;

		struct BoolMetadata
		{
#if defined(TPP_CLIENT)
			BoolMetadata() {}
#endif
			BoolMetadata(bool defaultValue) : defaultValue(defaultValue) {}
			bool defaultValue = false;
		} metadata;

		enum : uint32_t
		{
			Type = VariableType::Bool
		};
	};
	
	class Color3 final : public VariableBase
	{
	public:

#if defined(TPP_CLIENT)
		Color3() : VariableBase((VariableType)Type, sizeof(currentValue)) {}
#endif
		Color3(const char* path, float r, float g, float b);

		virtual ~Color3() {}

		virtual void SerializeMetadata(tpp::BinarySerializationWriter& writer) const override;
		virtual void DeserializeMetadata(tpp::BinarySerializationReader& reader) override;
		virtual void SerializeValue(tpp::BinarySerializationWriter& writer) const override;
		virtual void DeserializeValue(tpp::BinarySerializationReader& reader) override;
		virtual void RevertToDefault() override;
		virtual bool HasDefaultValue() override;

		union
		{
			struct { float r, g, b; };
			array<float, 3> currentValue;
		};

		struct Color3Metadata
		{
#if defined(TPP_CLIENT)
			Color3Metadata() {}
#endif
			Color3Metadata(float r, float g, float b) : r(r), g(g), b(b) {}

			union
			{
				struct { float r, g, b; };
				array<float, 3> defaultValue;
			};
		} metadata;

		enum : uint32_t
		{
			Type = VariableType::Color3
		};
	};

	class Color4 final : public VariableBase
	{
	public:

#if defined(TPP_CLIENT)
		Color4() : VariableBase((VariableType)Type, sizeof(currentValue)) {}
#endif
		Color4(const char* path, float r, float g, float b, float a);

		virtual ~Color4() {}

		virtual void SerializeMetadata(tpp::BinarySerializationWriter& writer) const override;
		virtual void DeserializeMetadata(tpp::BinarySerializationReader& reader) override;
		virtual void SerializeValue(tpp::BinarySerializationWriter& writer) const override;
		virtual void DeserializeValue(tpp::BinarySerializationReader& reader) override;
		virtual void RevertToDefault() override;
		virtual bool HasDefaultValue() override;

		union
		{
			struct { float r, g, b, a; };
			array<float, 4> currentValue;
		};

		struct Color4Metadata
		{
#if defined(TPP_CLIENT)
			Color4Metadata() {}
#endif
			Color4Metadata(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}

			union
			{
				struct { float r, g, b, a; };
				array<float, 4> defaultValue;
			};
		} metadata;

		enum : uint32_t
		{
			Type = VariableType::Color4
		};
	};

	class Vector2 final : public VariableBase
	{
	public:

#if defined(TPP_CLIENT)
		Vector2() : VariableBase((VariableType)Type, sizeof(currentValue)) {}
#endif
		Vector2(const char* path, float x, float y);

		virtual ~Vector2() {}

		virtual void SerializeMetadata(tpp::BinarySerializationWriter& writer) const override;
		virtual void DeserializeMetadata(tpp::BinarySerializationReader& reader) override;
		virtual void SerializeValue(tpp::BinarySerializationWriter& writer) const override;
		virtual void DeserializeValue(tpp::BinarySerializationReader& reader) override;
		virtual void RevertToDefault() override;
		virtual bool HasDefaultValue() override;

		union
		{
			struct { float x, y; };
			array<float, 2> currentValue;
		};

		struct Vector2Metadata
		{
#if defined(TPP_CLIENT)
			Vector2Metadata() {}
#endif
			Vector2Metadata(float x, float y) : x(x), y(y) {}

			union
			{
				struct { float x, y; };
				array<float, 2> defaultValue;
			};
		} metadata;

		enum : uint32_t
		{
			Type = VariableType::Vector2
		};
	};

	class Vector3 final : public VariableBase
	{
	public:

#if defined(TPP_CLIENT)
		Vector3() : VariableBase((VariableType)Type, sizeof(currentValue)) {}
#endif
		Vector3(const char* path, float x, float y, float z);

		virtual ~Vector3() {}

		virtual void SerializeMetadata(tpp::BinarySerializationWriter& writer) const override;
		virtual void DeserializeMetadata(tpp::BinarySerializationReader& reader) override;
		virtual void SerializeValue(tpp::BinarySerializationWriter& writer) const override;
		virtual void DeserializeValue(tpp::BinarySerializationReader& reader) override;
		virtual void RevertToDefault() override;
		virtual bool HasDefaultValue() override;

		union
		{
			struct { float x, y, z; };
			array<float, 3> currentValue;
		};

		struct Vector3Metadata
		{
#if defined(TPP_CLIENT)
			Vector3Metadata() {}
#endif
			Vector3Metadata(float x, float y, float z) : x(x), y(y), z(z) {}
			union
			{
				struct { float x, y, z; };
				array<float, 3> defaultValue;
			};
		} metadata;

		enum : uint32_t
		{
			Type = VariableType::Vector3
		};
	};

	class Vector4 final : public VariableBase
	{
	public:

#if defined(TPP_CLIENT)
		Vector4() : VariableBase((VariableType)Type, sizeof(currentValue)) {}
#endif
		Vector4(const char* path, float x, float y, float z, float w);

		virtual ~Vector4() {}

		virtual void SerializeMetadata(tpp::BinarySerializationWriter& writer) const override;
		virtual void DeserializeMetadata(tpp::BinarySerializationReader& reader) override;
		virtual void SerializeValue(tpp::BinarySerializationWriter& writer) const override;
		virtual void DeserializeValue(tpp::BinarySerializationReader& reader) override;
		virtual void RevertToDefault() override;
		virtual bool HasDefaultValue() override;

		union
		{
			struct { float x, y, z, w; };
			array<float, 4> currentValue;
		};

		struct Vector4Metadata
		{
#if defined(TPP_CLIENT)
			Vector4Metadata() {}
#endif
			Vector4Metadata(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

			union
			{
				struct { float x, y, z, w; };
				array<float, 4> defaultValue;
			};
		} metadata;

		enum : uint32_t
		{
			Type = VariableType::Vector4
		};
	};

	class Callback final : public VariableBase
	{
	public:

#if defined(TPP_CLIENT)
		Callback() : VariableBase((VariableType)Type, 0) {}
#endif
		Callback(const char* path, void(*callback)(void));

		virtual ~Callback() {}

		virtual void SerializeMetadata(tpp::BinarySerializationWriter& writer) const override;
		virtual void DeserializeMetadata(tpp::BinarySerializationReader& reader) override;
		virtual void SerializeValue(tpp::BinarySerializationWriter& writer) const override;
		virtual void DeserializeValue(tpp::BinarySerializationReader& reader) override;
		virtual void RevertToDefault() override;
		virtual bool HasDefaultValue() override;

		enum : uint32_t
		{
			Type = VariableType::Callback
		};

		void(*currentValue)(void);
	};

	class String final : public VariableBase
	{
	public:

#if defined(TPP_CLIENT)
		String() : VariableBase((VariableType)Type, 0) {}
#endif
		String(const char* path, const char* defaultValue);

		virtual ~String() {}

		virtual void SerializeMetadata(tpp::BinarySerializationWriter& writer) const override;
		virtual void DeserializeMetadata(tpp::BinarySerializationReader& reader) override;
		virtual void SerializeValue(tpp::BinarySerializationWriter& writer) const override;
		virtual void DeserializeValue(tpp::BinarySerializationReader& reader) override;
		virtual void RevertToDefault() override;
		virtual bool HasDefaultValue() override;

		std::string defaultValue;

		enum : uint32_t
		{
			Type = VariableType::String
		};

		std::string currentValue;
	};

	template<typename T>
	struct ValueNameEntry
	{
		T value;
		std::string name;
	};

	typedef ValueNameEntry<uint32_t> EnumEntry;

	// Add serialization/deserialization functions to the ValueNameEntry structs, which we'll use for enumeration, flags, etc
	template<typename UnderlyingT>
	inline BinarySerializationReader& operator << (BinarySerializationReader& stream, tpp::ValueNameEntry<UnderlyingT>& value)
	{
		stream << value.value;
		stream << value.name;
		return stream;
	}

	template<typename UnderlyingT>
	inline BinarySerializationWriter& operator << (BinarySerializationWriter& stream, const tpp::ValueNameEntry<UnderlyingT>& value)
	{
		stream << value.value;
		stream << value.name;
		return stream;
	}

	class Enum final : public VariableBase
	{
	public:

#if defined(TPP_CLIENT)
		Enum() : VariableBase((VariableType)Type, sizeof(currentValue)) {}
#endif

		Enum(const char* path, int defaultValue, const std::vector<EnumEntry>& entries);

		virtual ~Enum() {}

		virtual void SerializeMetadata(tpp::BinarySerializationWriter& writer) const override;
		virtual void DeserializeMetadata(tpp::BinarySerializationReader& reader) override;
		virtual void SerializeValue(tpp::BinarySerializationWriter& writer) const override;
		virtual void DeserializeValue(tpp::BinarySerializationReader& reader) override;
		virtual void RevertToDefault() override;
		virtual bool HasDefaultValue() override;

		enum : uint32_t
		{
			Type = VariableType::Enum
		};

		int currentValue = 0;

		struct EnumMetadata
		{
			int defaultValue;
			std::vector<EnumEntry> entries;
		} metadata;
	};

	template<typename UnderlyingT, VariableType VariableT>
	class Flags final : public VariableBase
	{
	public:

		typedef ValueNameEntry<UnderlyingT> EntryType;
		typedef UnderlyingT UnderlyingType;

#if defined(TPP_CLIENT)
		Flags() : VariableBase((VariableType)Type, sizeof(currentValue)) {}
#endif

		Flags(const char* path, UnderlyingT defaultValue, const std::vector<std::string>& entries);

		virtual ~Flags() {}

		virtual void SerializeMetadata(tpp::BinarySerializationWriter& writer) const override;
		virtual void DeserializeMetadata(tpp::BinarySerializationReader& reader) override;
		virtual void SerializeValue(tpp::BinarySerializationWriter& writer) const override;
		virtual void DeserializeValue(tpp::BinarySerializationReader& reader) override;
		virtual void RevertToDefault() override;
		virtual bool HasDefaultValue() override;

		enum : uint32_t
		{
			Type = VariableT
		};

		UnderlyingT currentValue = (UnderlyingT)0;

		struct FlagsMetadata
		{
			UnderlyingT defaultValue = 0;
			std::vector<std::string> entries;
		} metadata;
	};

	typedef Flags<uint8_t, VariableType::Flags8> Flags8;
	typedef Flags<uint16_t, VariableType::Flags16> Flags16;
	typedef Flags<uint32_t, VariableType::Flags32> Flags32;
	typedef Flags<uint64_t, VariableType::Flags64> Flags64;
}
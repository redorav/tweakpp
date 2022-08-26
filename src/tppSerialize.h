#pragma once

#include "tppHash.h"

#include "tppNetwork.h"

#include <string>
#include <vector>

namespace tpp
{
	namespace SerializationStreamType
	{
		enum T : uint32_t
		{
			Write,
			Read
		};
	}

	// This serialization class is able to support input and output serialization (not simultaneously)
	// through the same operators to be able to share code
	template<SerializationStreamType::T StreamTypeT>
	class SerializationStreamBase
	{
	public:

		static const bool IsReading() { return StreamTypeT == SerializationStreamType::Read; }
		static const bool IsWriting() { return StreamTypeT == SerializationStreamType::Write; }

		// Reserve a certain size (this is only useful when writing)
		SerializationStreamBase(uint32_t initialSize = 0)
		{
			m_data.reserve(initialSize);
		}

		// Clear the data
		void Clear()
		{
			m_currentPosition = 0;
			m_data.clear();
		}

		// Reset serializer so we're back at the beginning
		// In write mode we clear the data we have. In read
		// mode we wind back the pointer
		void Reset()
		{
			if (IsWriting())
			{
				m_data.clear();
			}
			else
			{
				m_currentPosition = 0;
			}
		}

		const std::vector<char>& Buffer() { return m_data; }

		const char* Data() const { return m_data.data(); }

		char* Back() { return &m_data.back(); }

		size_t Size() const { return m_data.size(); }
		
		bool Empty() const { return m_data.empty(); }

		void WriteRaw(const char* value, uint32_t length)
		{
			m_data.insert(m_data.end(), value, value + length);
		}

	protected:

		template<typename T>
		void Write(const T& value)
		{
			WriteRaw((const char*)(&value), sizeof(value));
		}

		template<typename T>
		void Read(T& value)
		{
			value = *reinterpret_cast<T*>(&m_data[m_currentPosition]);
			m_currentPosition += sizeof(value);
		}

		size_t m_currentPosition = 0;

		std::vector<char> m_data;
	};

	template<SerializationStreamType::T StreamTypeT>
	class BinarySerializer;

	template<>
	class BinarySerializer<SerializationStreamType::Write> : public SerializationStreamBase<SerializationStreamType::Write>
	{
	public:

		using SerializationStreamBase<SerializationStreamType::Write>::SerializationStreamBase;

		BinarySerializer& operator << (const bool& value)     { Write(value); return *this; }
		BinarySerializer& operator << (const char& value)     { Write(value); return *this; }
		BinarySerializer& operator << (const uint8_t& value)  { Write(value); return *this; }
		BinarySerializer& operator << (const uint16_t& value) { Write(value); return *this; }
		BinarySerializer& operator << (const uint32_t& value) { Write(value); return *this; }
		BinarySerializer& operator << (const uint64_t& value) { Write(value); return *this; }
		BinarySerializer& operator << (const int8_t& value)   { Write(value); return *this; }
		BinarySerializer& operator << (const int16_t& value)  { Write(value); return *this; }
		BinarySerializer& operator << (const int32_t& value)  { Write(value); return *this; }
		BinarySerializer& operator << (const int64_t& value)  { Write(value); return *this; }
		BinarySerializer& operator << (const float& value)    { Write(value); return *this; }
		BinarySerializer& operator << (const double& value)   { Write(value); return *this; }

		BinarySerializer& operator << (const std::string& v)
		{
			Write((uint32_t)v.length());
			WriteRaw(v.c_str(), (uint32_t)v.length());
			return *this;
		}

		template<typename T>
		BinarySerializer& operator << (const std::vector<T>& vector)
		{
			Write((uint32_t)vector.size());

			if (std::is_trivially_copyable<T>::value)
			{
				WriteRaw((const char*)vector.data(), (uint32_t)(vector.size() * sizeof(T)));
			}
			else
			{
				for (const T& v : vector)
				{
					*this << v;
				}
			}
			return *this;
		}

		template<typename T, std::enable_if_t<std::is_trivially_copy_assignable<T>::value, bool> = true>
		BinarySerializer& operator << (const T& v)
		{
			static_assert(std::is_trivially_copy_assignable<T>::value, "T must be trivially copy-assignable");
			Write(v);
			return *this;
		}
	};

	template<>
	class BinarySerializer<SerializationStreamType::Read> : public SerializationStreamBase<SerializationStreamType::Read>
	{
	public:

		using SerializationStreamBase<SerializationStreamType::Read>::SerializationStreamBase;

		// Pass in data that we want serialized
		BinarySerializer(const std::vector<char>& data)
		{
			m_data = data;
		}

		BinarySerializer(const char* data, size_t size)
		{
			m_data.assign(data, data + size);
		}

		bool HasData() const
		{
			return m_currentPosition < m_data.size();
		}

		BinarySerializer& operator << (bool& value)     { Read(value); return *this; }
		BinarySerializer& operator << (char& value)     { Read(value); return *this; }
		BinarySerializer& operator << (uint8_t& value)  { Read(value); return *this; }
		BinarySerializer& operator << (uint16_t& value) { Read(value); return *this; }
		BinarySerializer& operator << (uint32_t& value) { Read(value); return *this; }
		BinarySerializer& operator << (uint64_t& value) { Read(value); return *this; }
		BinarySerializer& operator << (int8_t& value)   { Read(value); return *this; }
		BinarySerializer& operator << (int16_t& value)  { Read(value); return *this; }
		BinarySerializer& operator << (int32_t& value)  { Read(value); return *this; }
		BinarySerializer& operator << (int64_t& value)  { Read(value); return *this; }
		BinarySerializer& operator << (float& value)    { Read(value); return *this; }
		BinarySerializer& operator << (double& value)   { Read(value); return *this; }

		SerializationStreamBase& operator << (std::string& v)
		{
			uint32_t sizeBytes;
			Read(sizeBytes);

			v.reserve(sizeBytes);
			v.assign(reinterpret_cast<const char*>(&m_data[m_currentPosition]), sizeBytes);
			m_currentPosition += sizeBytes;

			return *this;
		}

		template<typename T>
		BinarySerializer& operator << (std::vector<T>& vector)
		{
			uint32_t size;
			Read(size);

			if (std::is_trivially_copyable<T>::value)
			{
				uint32_t sizeBytes = size * sizeof(T);
				vector.assign(reinterpret_cast<const T*>(&m_data[m_currentPosition]), reinterpret_cast<const T*>(&m_data[m_currentPosition]) + size);
				m_currentPosition += sizeBytes;
			}
			else
			{
				vector.resize(size);
				for (uint32_t i = 0; i < size; ++i)
				{
					T& value = vector[i];
					*this << value;
				}
			}
			return *this;
		}

		template<typename T, std::enable_if_t<std::is_trivially_copy_assignable<T>::value, bool> = true>
		SerializationStreamBase& operator << (T& v)
		{
			static_assert(std::is_trivially_copy_assignable<T>::value, "T must be trivially copy-assignable");
			Read(v);
			return *this;
		}
	};

	typedef BinarySerializer<SerializationStreamType::Write> BinarySerializationWriter;
	typedef BinarySerializer<SerializationStreamType::Read> BinarySerializationReader;
}
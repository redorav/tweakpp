#pragma once

#include "tppHash.h"

#include "tppNetwork.h"

namespace tpp
{
	namespace SerializationStreamType
	{
		enum T
		{
			BinaryWrite,
			BinaryRead
		};
	}

	template<SerializationStreamType::T StreamType>
	class SerializationStream {};

	template<>
	class SerializationStream<SerializationStreamType::BinaryWrite>
	{
	public:

		SerializationStream(uint32_t initialSize = 0)
		{
			m_data.reserve(initialSize);
		}

		void Clear() { m_data.clear(); }

		const char* Data() const { return m_data.data(); }

		char* Back() { return &m_data.back(); }

		size_t Size() const { return m_data.size(); }
		
		bool Empty() const { return m_data.empty(); }

		template<typename S>
		SerializationStream& operator << (const S& v)
		{
			m_data.insert(m_data.end(), reinterpret_cast<const char*>(&v), reinterpret_cast<const char*>(&v) + sizeof(v));
			return *this;
		}

		SerializationStream& operator << (const std::string& v)
		{
			m_data.insert(m_data.end(), v.c_str(), v.c_str() + v.size() + 1); // Include null terminator
			return *this;
		}

	private:

		std::vector<char> m_data;
	};

	template<>
	class SerializationStream<SerializationStreamType::BinaryRead>
	{
	public:

		SerializationStream(uint32_t initialSize)
		{
			m_data.reserve(initialSize);
		}

		template<typename S>
		SerializationStream& operator << (S& v)
		{
			v = *reinterpret_cast<S*>(&m_data[m_currentPosition]);
			m_currentPosition += sizeof(v);
			return *this;
		}

		SerializationStream& operator << (std::string& v)
		{
			v = &m_data[m_currentPosition];
			m_currentPosition += v.size() + 1; // Include null terminator
			return *this;
		}

	private:

		size_t m_currentPosition = 0;

		std::vector<char> m_data;
	};

	typedef SerializationStream<SerializationStreamType::BinaryWrite> BinarySerializationWriter;
	typedef SerializationStream<SerializationStreamType::BinaryRead> BinarySerializationReader;
}
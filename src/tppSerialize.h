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

	template<SerializationStreamType::T StreamType>
	class Archive
	{
	public:

		Archive(SerializationStream<StreamType>& stream) : m_stream(stream)
		{
	
		}

		void Clear()
		{
			m_stream.Clear();
		}

		// TODO Remove ugly const_cast and just make explicit that things can be modified. It does
		// mean that we have to change the code upstream
		void SerializeTppVariableUpdatePacket(const tpp::Variable& inVariable)
		{
			tpp::Variable& variable = const_cast<tpp::Variable&>(inVariable);

			size_t startSize = m_stream.Size();

			// We don't know what the size is going to be yet so we put 0. The final step will patch the header with the
			// size that we know once the whole message has been constructed
			m_stream << tpp::MessageHeader(0, tpp::MessageType::Update);

			// We don't serialize the path when updating the packet, as we pass in the hash

			if (variable.type == tpp::VariableType::Float)
			{
				SerializeTppVariableUpdate(variable.vdFloat, variable.hash);
			}
			else if (variable.type == tpp::VariableType::UnsignedInteger)
			{
				SerializeTppVariableUpdate(variable.vdInt, variable.hash);
			}
			else if (variable.type == tpp::VariableType::Integer)
			{
				SerializeTppVariableUpdate(variable.vdInt, variable.hash);
			}
			else if (variable.type == tpp::VariableType::Bool)
			{
				SerializeTppVariableUpdate(variable.vdBool, variable.hash);
			}
			else if (variable.type == tpp::VariableType::Color3)
			{
				SerializeTppVariableUpdate(variable.vdColor3, variable.hash);
			}
			else if (variable.type == tpp::VariableType::Color4)
			{
				SerializeTppVariableUpdate(variable.vdColor4, variable.hash);
			}
			else if (variable.type == tpp::VariableType::Vector2)
			{
				SerializeTppVariableUpdate(variable.vdVector2, variable.hash);
			}
			else if (variable.type == tpp::VariableType::Vector3)
			{
				SerializeTppVariableUpdate(variable.vdVector3, variable.hash);
			}
			else if (variable.type == tpp::VariableType::Vector4)
			{
				SerializeTppVariableUpdate(variable.vdVector4, variable.hash);
			}
			else if (variable.type == tpp::VariableType::Callback)
			{
				SerializeTppVariableUpdate(variable.vdCallback, variable.hash);
			}
			else
			{
				printf("Variable %s not serialized correctly\n", variable.path.c_str());
			}

			// Patch in the packet size
			size_t totalSize = m_stream.Size() - startSize;
			size_t packetSize = totalSize - sizeof(tpp::MessageHeader);

			tpp::MessageHeader* header = reinterpret_cast<tpp::MessageHeader*>(m_stream.Back() - totalSize + 1);
			header->size = (decltype(header->size))packetSize;
		}

		template<typename TppVariable>
		void SerializeTppVariableUpdate(TppVariable& variable, const tpp::Hash& hash)
		{
			// If we're updating, we only need to send the current value
			m_stream << tpp::VariableHeader(variable.type, sizeof(variable.currentValue), (uint64_t)hash);
			m_stream << variable.currentValue;
		}

		template<>
		void SerializeTppVariableUpdate<tpp::Callback>(tpp::Callback& variable, const tpp::Hash& hash)
		{
			m_stream << tpp::VariableHeader(variable.type, 0, hash);
		}
	
		void SerializePath(std::string& path)
		{
			m_stream << path;
		}

		const SerializationStream<StreamType>& GetStream() const
		{
			return m_stream;
		}

	private:
	
		SerializationStream<StreamType>& m_stream;
	};
}
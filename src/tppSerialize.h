#pragma once

// Example
//
// HEADER
// tpp MessageType::Update 33 2 - Update message that is 33 bytes, version 2
// 
// MESSAGE
// [Header][path]Rendering/SSR/Number of Rays\0 [var][tppFloat]
// [Header][path]Rendering/SSR/Optimized\0      [var][tppBool]
// [Header][path]Rendering/SSR/Clear\0          [var][tppString]

namespace tpp
{
	namespace SerializationStreamType
	{
		enum T
		{
			RawStreamWrite,
			RawStreamRead
		};
	}

	template<SerializationStreamType::T StreamType>
	class SerializationStream {};

	template<>
	class SerializationStream<SerializationStreamType::RawStreamWrite>
	{
	public:

		SerializationStream(uint32_t initialSize)
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
			m_data.insert(m_data.end(), v.c_str(), v.c_str() + v.size());
			return *this;
		}

	private:

		std::vector<char> m_data;
	};

	template<SerializationStreamType::T StreamType>
	class Archive
	{
	public:

		Archive(uint32_t initialSize)
		{
	
		}

		void Clear()
		{
			m_stream.Clear();
		}

		void SerializeTppVariableUpdatePacket(const tpp::Variable& variable)
		{
			size_t startSize = m_stream.Size();

			// We don't know what the size is going to be yet so we put 0. The final step will patch the header with the
			// size that we know once the whole message has been constructed
			m_stream << tpp::MessageHeader(0, tpp::MessageType::Update);

			SerializePath(variable.path);
			
			if (variable.type == tpp::VariableType::Float)
			{
				SerializeTppVariable(tpp::MessageType::Update, variable.vdFloat);
			}
			else if (variable.type == tpp::VariableType::Color3)
			{
				SerializeTppVariable(tpp::MessageType::Update, variable.vdColor3);
			}

			size_t totalSize = m_stream.Size() - startSize;
			size_t packetSize = totalSize - sizeof(tpp::MessageHeader);

			tpp::MessageHeader* header = reinterpret_cast<tpp::MessageHeader*>(m_stream.Back() - totalSize + 1);
			header->size = (decltype(header->size))packetSize;
		}

		void SerializeVariableDescription(const std::string& path, const tpp::Variable& variable)
		{
			size_t startSize = m_stream.Size();

			// We don't know what the size is going to be yet so we put 0. The final step will patch the header with the
			// size that we know once the whole message has been constructed
			m_stream << tpp::MessageHeader(0, tpp::MessageType::Declaration);

			SerializePath(path);

			if (variable.type == tpp::VariableType::Float)
			{
				SerializeTppVariable(tpp::MessageType::Declaration, variable.vdFloat);
			}
			else if (variable.type == tpp::VariableType::Color3)
			{
				SerializeTppVariable(tpp::MessageType::Declaration, variable.vdColor3);
			}

			size_t totalSize = m_stream.Size() - startSize;
			size_t packetSize = totalSize - sizeof(tpp::MessageHeader);

			tpp::MessageHeader* header = reinterpret_cast<tpp::MessageHeader*>(m_stream.Back() - totalSize + 1);
			header->size = (decltype(header->size))packetSize;
		}

		template<typename TppVariable>
		void SerializeTppVariable(const tpp::MessageType messageType, const TppVariable& variable)
		{
			// If we're updating, we only need to send the current value. Otherwise, we send the entire variable
			if (messageType == tpp::MessageType::Update)
			{
				m_stream << tpp::VariableHeader(variable.type, sizeof(variable.currentValue));
				m_stream << variable.currentValue;
			}
			else
			{
				m_stream << tpp::VariableHeader(variable.type, sizeof(variable));
				m_stream << variable;
			}
		}
	
		void SerializePath(const std::string& path)
		{
			m_stream << path << '\0';
		}

		const SerializationStream<StreamType>& GetStream() const
		{
			return m_stream;
		}

	private:
	
		SerializationStream<StreamType> m_stream = SerializationStream<StreamType>(512);
	};

	template<typename S>
	void SerializeRaw(std::vector<char>& stream, const S& s)
	{
		stream.insert(stream.end(), reinterpret_cast<const char*>(&s), reinterpret_cast<const char*>(&s) + sizeof(s));
	}

	void Serialize(std::vector<char>& stream, float value)
	{
		SerializeRaw(stream, tpp::VariableHeader(tpp::VariableType::Float, sizeof(value)));
		SerializeRaw(stream, value);
	}

	void SerializeColor3(std::vector<char>& stream, const float* value)
	{
		SerializeRaw(stream, tpp::VariableHeader(tpp::VariableType::Color3, 4 * 3));
		stream.insert(stream.end(), reinterpret_cast<const char*>(value), reinterpret_cast<const char*>(value) + 4 * 3);
	}

	void Serialize(std::vector<char>& stream, const tpp::Float& variable)
	{
		SerializeRaw(stream, tpp::VariableHeader(variable.type, sizeof(variable)));
		SerializeRaw(stream, variable);
	}

	void Serialize(std::vector<char>& stream, const tpp::Color3& variable)
	{
		SerializeRaw(stream, tpp::VariableHeader(variable.type, sizeof(variable)));
		SerializeRaw(stream, variable);
	}

	void Serialize(std::vector<char>& stream, const std::string& path)
	{
		stream.insert(stream.end(), tpp::PathString, tpp::PathString + strlen(tpp::PathString));
		stream.insert(stream.end(), path.begin(), path.end() + 1); // Include null terminator
	}
}
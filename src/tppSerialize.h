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
			m_data.insert(m_data.end(), v.c_str(), v.c_str() + v.size() + 1); // Include null terminator
			return *this;
		}

	//private:

		std::vector<char> m_data;
	};

	template<>
	class SerializationStream<SerializationStreamType::RawStreamRead>
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

	//private:

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

			SerializePath(variable.path);
			
			if (variable.type == tpp::VariableType::Float)
			{
				SerializeTppVariable(tpp::MessageType::Update, variable.vdFloat);
			}
			else if (variable.type == tpp::VariableType::UnsignedInteger)
			{
				SerializeTppVariable(tpp::MessageType::Update, variable.vdInt);
			}
			else if (variable.type == tpp::VariableType::Integer)
			{
				SerializeTppVariable(tpp::MessageType::Update, variable.vdInt);
			}
			else if (variable.type == tpp::VariableType::Bool)
			{
				SerializeTppVariable(tpp::MessageType::Update, variable.vdBool);
			}
			else if (variable.type == tpp::VariableType::Color3)
			{
				SerializeTppVariable(tpp::MessageType::Update, variable.vdColor3);
			}
			else if (variable.type == tpp::VariableType::Color4)
			{
				SerializeTppVariable(tpp::MessageType::Update, variable.vdColor4);
			}
			else
			{
				printf("Variable %s not serialized correctly\n", variable.path.c_str());
			}

			size_t totalSize = m_stream.Size() - startSize;
			size_t packetSize = totalSize - sizeof(tpp::MessageHeader);

			tpp::MessageHeader* header = reinterpret_cast<tpp::MessageHeader*>(m_stream.Back() - totalSize + 1);
			header->size = (decltype(header->size))packetSize;
		}

		void SerializeVariableDescription(const std::string& inPath, const tpp::Variable& inVariable)
		{
			tpp::Variable& variable = const_cast<tpp::Variable&>(inVariable);
			std::string& path = const_cast<std::string&>(inPath);

			size_t startSize = m_stream.Size();

			// We don't know what the size is going to be yet so we put 0. The final step will patch the header with the
			// size that we know once the whole message has been constructed
			m_stream << tpp::MessageHeader(0, tpp::MessageType::Declaration);

			SerializePath(path);

			if (variable.type == tpp::VariableType::Float)
			{
				SerializeTppVariable(tpp::MessageType::Declaration, variable.vdFloat);
			}
			else if (variable.type == tpp::VariableType::UnsignedInteger)
			{
				SerializeTppVariable(tpp::MessageType::Declaration, variable.vdUInt);
			}
			else if (variable.type == tpp::VariableType::Integer)
			{
				SerializeTppVariable(tpp::MessageType::Declaration, variable.vdInt);
			}
			else if (variable.type == tpp::VariableType::Bool)
			{
				SerializeTppVariable(tpp::MessageType::Declaration, variable.vdBool);
			}
			else if (variable.type == tpp::VariableType::Color3)
			{
				SerializeTppVariable(tpp::MessageType::Declaration, variable.vdColor3);
			}
			else if (variable.type == tpp::VariableType::Color4)
			{
				SerializeTppVariable(tpp::MessageType::Declaration, variable.vdColor4);
			}
			else
			{
				printf("Variable %s not serialized correctly\n", path.c_str());
			}

			size_t totalSize = m_stream.Size() - startSize;
			size_t packetSize = totalSize - sizeof(tpp::MessageHeader);

			tpp::MessageHeader* header = reinterpret_cast<tpp::MessageHeader*>(m_stream.Back() - totalSize + 1);
			header->size = (decltype(header->size))packetSize;
		}

		template<typename TppVariable>
		void SerializeTppVariable(tpp::MessageType messageType, TppVariable& variable)
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
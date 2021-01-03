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
	void SerializeCommandHeader(std::vector<char>& stream, tpp::MessageType messageType)
	{
		// We don't know what the size is going to be yet so we put 0. The final step will patch the header with the
		// size that we know once the whole message has been constructed
		tpp::MessageHeader header;
		header.messageType = messageType;
		header.messageSize = 0;

		stream.insert(stream.end(), (const char*)&header, (const char*)&header + sizeof(header));
	}

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
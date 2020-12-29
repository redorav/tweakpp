#pragma once

// Example
//
// HEADER
// tpp MessageType::Update 33 2 - Update message that is 33 bytes, version 2
// 
// MESSAGE
// [Header][path]Rendering/SSR/Number of Rays\0 [var][floatType][floatValue]
// [Header][path]Rendering/SSR/Optimized\0      [var][boolType][boolValue]
// [Header][path]Rendering/SSR/Clear\0          [var][functionType]

void SerializeCommandHeader(std::vector<char>& commandStream, tpp::MessageType messageType)
{
	// We don't know what the size is going to be yet so we put 0. The final step will patch the header with the
	// size that we know once the whole message has been constructed
	tpp::MessageHeader header;
	header.messageType = messageType;
	header.messageSize = 0;

	commandStream.insert(commandStream.end(), (const char*)&header, (const char*)&header + sizeof(header));
}

// TODO Templated function that just copies whatever data we pass through
// TODO Some types like strings probably need special handling
void SerializeFloatProperty(std::vector<char>& commandStream, tpp::PropertyType type, float value)
{
	tpp::PropertyHeader propertyHeader;
	propertyHeader.type = type;
	propertyHeader.size = sizeof(value);
	commandStream.insert(commandStream.end(), reinterpret_cast<char*>(&propertyHeader), reinterpret_cast<char*>(&propertyHeader) + sizeof(propertyHeader));
	commandStream.insert(commandStream.end(), reinterpret_cast<char*>(&value), reinterpret_cast<char*>(&value) + sizeof(value));
}

void SerializeFloat(std::vector<char>& commandStream, float value)
{
	tpp::VariableHeader variableHeader;
	variableHeader.type = tpp::VariableType::Float;
	variableHeader.size = 4;
	commandStream.insert(commandStream.end(), reinterpret_cast<char*>(&variableHeader), reinterpret_cast<char*>(&variableHeader) + sizeof(variableHeader));
	commandStream.insert(commandStream.end(), reinterpret_cast<char*>(&value), reinterpret_cast<char*>(&value) + sizeof(value));
}

void SerializePath(std::vector<char>& commandStream, const std::string& path)
{
	commandStream.insert(commandStream.end(), tpp::PathString, tpp::PathString + strlen(tpp::PathString));
	commandStream.insert(commandStream.end(), path.begin(), path.end() + 1);
}
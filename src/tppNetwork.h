#pragma once

#include "tppCommon.h"

namespace tpp
{
	// Example Header
	// tpp MessageType::Update 33 2 - Update message that is 33 bytes, version 2

	static const char HeaderString[3] = { 't', 'p', 'p' };

	enum class VariableType : uint8_t;

	enum class MessageType : uint8_t
	{
		Declaration = 0, // Client tells server a variable of a certain type and parameters exists
		Update      = 1, // Server tells client to update a variable's value
		None        = 255,
	};

	enum class Version : uint16_t
	{
		InitialVersion = 0,
		CurrentVersion = InitialVersion,
	};

	// Pack so we can serialize directly from the incoming message
	// Do not shuffle the fields without changing the version!

	TPP_PACK_BEGIN
	struct MessageHeader
	{
		MessageHeader() {}

		MessageHeader(uint32_t size, MessageType type) : size(size), type(type) {}

		char tpp[3] = { HeaderString[0], HeaderString[1], HeaderString[2] }; // 3 bytes
		Version version = Version::CurrentVersion; // 2 bytes
		uint32_t size = 0; // 4 bytes
		MessageType type = MessageType::None; // 1 byte
	};
	TPP_PACK_END

	static_assert(sizeof(HeaderString) == sizeof(MessageHeader::tpp), "MessageHeader size mismatch");

	TPP_PACK_BEGIN
	struct VariableHeader
	{
		VariableHeader() {}

		VariableHeader(tpp::VariableType type, uint32_t size, uint64_t hash) : type(type), size(size), hash(hash) {}

		VariableHeader(uint32_t type, uint32_t size, uint64_t hash) : VariableHeader(static_cast<tpp::VariableType>(type), size, hash) {}

		// Type of variable
		VariableType type; // 1 byte
		
		// Size of the variable data
		uint32_t size; // 4 bytes
		
		// Unique identifier. Typically formed from the path
		uint64_t hash; // 8 bytes
	};
	TPP_PACK_END

	class ISocket;

	class Network
	{
	public:

		static void Initialize();

		static void Shutdown();

		static ISocket* CreateSocket();
	};
}
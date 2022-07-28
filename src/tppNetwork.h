#pragma once

#include "tppCommon.h"

namespace tpp
{
	static const char* HeaderString   = "tpp";
	static const char* PathString     = "path";
	static const char* VariableString = "var";

	enum class VariableType : uint8_t;

	enum class MessageType : uint8_t
	{
		Declaration = 0, // Client tells server a variable of a certain type and parameters exists
		Update      = 1, // Server tells client to update a variable's value
	};

	enum class Version : uint16_t
	{
		InitialVersion = 0,
		CurrentVersion = InitialVersion
	};

	// Pack so we can serialize directly from the incoming message
	// Do not shuffle the fields without changing the version!

	TPP_PACK_BEGIN
	struct MessageHeader
	{
		MessageHeader() {}

		MessageHeader(uint32_t size, MessageType type) : size(size), type(type) {}

		char tpp[3] = { 't', 'p', 'p' };
		Version version = Version::CurrentVersion;
		uint32_t size;
		MessageType type;
	};
	TPP_PACK_END

	TPP_PACK_BEGIN
	struct VariableHeader
	{
		VariableHeader(uint32_t type, uint32_t size) : type(static_cast<tpp::VariableType>(type)), size(size) {}

		VariableHeader(tpp::VariableType type, uint32_t size) : type(type), size(size) {}

		VariableType type;
		uint32_t size;
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
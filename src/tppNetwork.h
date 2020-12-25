#pragma once

#include <string>

#if defined(_MSC_VER)

#define TPP_PACK_BEGIN __pragma(pack(push, 1))
#define TPP_PACK_END   __pragma(pack(pop))

#endif

namespace tpp
{
	static const char* HeaderString = "tpp";
	static const char* PathString   = "path";
	static const char* TypeString   = "type";
	static const char* ValueString  = "value";

	enum class MessageType : uint32_t
	{
		Declaration = 0, // Client tells server a variable of a certain type and parameters exists
		Update      = 1, // Server tells client to update a variable's value
	};

	enum class Version : uint32_t
	{
		InitialVersion = 0,
		CurrentVersion = InitialVersion
	};

	// Pack so we can serialize directly from the incoming message

	TPP_PACK_BEGIN
	struct MessageHeader
	{
		const char tpp[3] = { 't', 'p', 'p' };
		Version version = Version::CurrentVersion;
		uint32_t messageSize;
		MessageType messageType;
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
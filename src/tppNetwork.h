#pragma once

#include <string>

#if defined(_MSC_VER)

#define TPP_PACK_BEGIN __pragma(pack(push, 1))
#define TPP_PACK_END   __pragma(pack(pop))

#endif

namespace tpp
{
	static const char* HeaderString   = "tpp";
	static const char* PathString     = "path";
	static const char* VariableString = "var";

	enum class VariableType : uint32_t
	{
		Float = 0,
		UnsignedInteger = 1,
		Integer = 2,
		Color3 = 3,
		Color4 = 4,
		Bool = 5,
		Function = 6,
		Invalid = 0xffffffff
	};

	enum class PropertyType : uint32_t
	{
		Min,
		Max,
		Step,
	};

	TPP_PACK_BEGIN
	struct PropertyHeader
	{
		PropertyType type;
		uint32_t size;
	};
	TPP_PACK_END

	enum class MessageType : uint32_t
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

	TPP_PACK_BEGIN
	struct MessageHeader
	{
		const char tpp[3] = { 't', 'p', 'p' };
		Version version = Version::CurrentVersion;
		uint32_t messageSize;
		MessageType messageType;
	};
	TPP_PACK_END

	TPP_PACK_BEGIN
	struct VariableHeader
	{
		const char var[3] = { 'v', 'a', 'r' };
		tpp::VariableType type;
		uint32_t size;
	};
	TPP_PACK_END

	TPP_PACK_BEGIN
	struct VariableDeclarationHeader
	{
		const char var[3] = { 'v', 'a', 'r' };
		tpp::VariableType type;
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
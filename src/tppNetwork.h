#pragma once

#include <string>

namespace tpp
{
	static const char* HeaderString = "tpp";
	static const char* PathString   = "path";
	static const char* TypeString   = "type";
	static const char* ValueString  = "value";

	class ISocket;

	class Network
	{
	public:

		static void Initialize();

		static void Shutdown();

		static ISocket* CreateSocket();
	};
}
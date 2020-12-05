#pragma once

namespace tpp
{
	class ISocket;

	class Network
	{
	public:

		static void Initialize();

		static void Deinitialize();

		static ISocket* CreateSocket();
	};
}
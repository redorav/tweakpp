#pragma once

#include "tppISocket.h"

namespace tpp
{
	struct NetworkAddress;
	class ISocket;

	class Server
	{
	public:

		static void Initialize(const tpp::NetworkAddress& networkAddress);

		static void Update();

		static void Shutdown();

		static tpp::ISocket* GlobalServerSocket;

		static tpp::ISocket* GlobalClientSocket;

		static tpp::NetworkAddress GlobalNetworkAddress;

		static const int BufferLength = 512;

		static char ReceiveBuffer[BufferLength];

		// TODO Remove once we have a more dynamic system
		static bool SentFullVariableTable;
	};
};
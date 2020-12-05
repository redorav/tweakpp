#pragma once

#include "tppNetwork.h"
#include "tppSocketPOSIX.h"

#if defined(_WIN32)

#include <winsock2.h>

#endif

namespace tpp
{
	void Network::Initialize()
	{
		WSADATA wsaData;

		// Initialize Winsock
		int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

		if (iResult == 0)
		{
			//printf("WSAStartup initialized correctly\n");
		}
		else
		{
			//printf("WSAStartup failed with error: %d\n", iResult);
			//return 1;
		}
	}

	void Network::Deinitialize()
	{
		WSACleanup();
	}

	ISocket* Network::CreateSocket()
	{
		return new SocketPOSIX();
	}
}
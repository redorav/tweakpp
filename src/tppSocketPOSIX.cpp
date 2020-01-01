#include "tppSocketPOSIX.h"

#if defined(_WIN32)

	#include <winsock2.h>
	#include <ws2tcpip.h>

#endif

#include <cstdio>

namespace tpp
{
	SocketPOSIX::SocketPOSIX() : m_socketHandle(INVALID_SOCKET)
	{
	}

	SocketPOSIX::SocketPOSIX(int socketHandle) : m_socketHandle(socketHandle)
	{
	}

	ISocket* SocketPOSIX::Accept(const NetworkAddress& address)
	{
		sockaddr clientAddr = {};
		socklen_t clientAddrSize = sizeof(clientAddr);
		int clientSocketHandle = accept(m_socketHandle, &clientAddr, &clientAddrSize);

		if (clientSocketHandle == INVALID_SOCKET)
		{
			return nullptr;
		}

		SocketPOSIX* clientSocketPOSIX = new SocketPOSIX(clientSocketHandle);

		return clientSocketPOSIX;
	}

	SocketReturn::T SocketPOSIX::Create()
	{
		m_socketHandle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (m_socketHandle == INVALID_SOCKET)
		{
			printf("Socket failed with error: %ld\n", WSAGetLastError());
			return SocketReturn::InvalidSocket;
		}
		else
		{
			printf("Socket created correctly\n");
		}

		return SocketReturn::Ok;
	};

	SocketReturn::T SocketPOSIX::Connect(const NetworkAddress& networkAddress)
	{
		addrinfo  hints = {};
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		char buffer[32];
		snprintf(buffer, sizeof(buffer), "%d", networkAddress.port);

		// Get the address info for the address and port
		addrinfo* addressInfo = nullptr;
		int result = getaddrinfo(networkAddress.address, buffer, &hints, &addressInfo);
		if (result != 0)
		{
			return SocketReturn::Error;
		}

		result = connect(m_socketHandle, addressInfo->ai_addr, (int)addressInfo->ai_addrlen);

		freeaddrinfo(addressInfo);

		if (result == SOCKET_ERROR)
		{
			closesocket(m_socketHandle);
			m_socketHandle = INVALID_SOCKET;
			return SocketReturn::InvalidSocket;
		}

		return SocketReturn::Ok;
	}

	// https://stackoverflow.com/questions/4160347/close-vs-shutdown-socket
	SocketReturn::T SocketPOSIX::Close()
	{
		int result = 0;

		// First shutdown socket from sending/receiving anything else. This sends a FIN packet
#if defined(_WIN32)
		result = shutdown(m_socketHandle, SD_BOTH);
#else
		result = shutdown(m_socketHandle, SHUT_RDWR);
#endif

		if (result == SOCKET_ERROR)
		{
			return SocketReturn::Error;
		}

		// Then close the socket which actually destroys resources, etc. This sends a RST packet
#if defined(_WIN32)
		result = closesocket(m_socketHandle);
#else
		result = close(m_socketHandle);
#endif

		if (result == SOCKET_ERROR)
		{
			return SocketReturn::Error;
		}

		return SocketReturn::Ok;
	}

	SocketReturn::T SocketPOSIX::Listen(int port)
	{
		// https://docs.microsoft.com/en-us/windows/desktop/api/winsock/nf-winsock-setsockopt
		// Allows the socket to be bound to an address that is already in use.For more information, see bind.Not applicable on ATM sockets.
		int option = 1;
		int result = setsockopt(m_socketHandle, SOL_SOCKET, SO_REUSEADDR, (char*)&option, sizeof(option));
		if (result != 0)
		{
			printf("Could not set reuse. %s", strerror(errno));
			return SocketReturn::Error;
		}

		// Bind to listening port
		sockaddr_in inSockAddress = {};
		inSockAddress.sin_family = AF_INET;
		inSockAddress.sin_port = htons((unsigned short)port); // Convert to short
		inSockAddress.sin_addr.s_addr = INADDR_ANY;

		result = bind(m_socketHandle, (const sockaddr*)&inSockAddress, sizeof(sockaddr_in));
		if (result == SOCKET_ERROR)
		{
			closesocket(m_socketHandle);
			m_socketHandle = INVALID_SOCKET;
			return SocketReturn::Error;
		}

		// Start listening
		result = listen(m_socketHandle, 8);
		if (result == SOCKET_ERROR)
		{
			closesocket(m_socketHandle);
			m_socketHandle = INVALID_SOCKET;
			return SocketReturn::Error;
		}

		return SocketReturn::Ok;
	}

	// https://docs.microsoft.com/en-us/windows/desktop/api/winsock/nf-winsock-recv
	// http://man7.org/linux/man-pages/man2/recv.2.html
	SocketReturn::T SocketPOSIX::Receive(char* buffer, int bufferLength)
	{
		int result = recv(m_socketHandle, buffer, bufferLength, 0);

		if (result > 0)
		{
			printf("Message received: %.*s (%d bytes)\n", result, buffer, result);
			return (SocketReturn::T)result;
		}
		else if (result == 0)
		{
			// If the connection has been gracefully closed, the return value is zero.
			return SocketReturn::ConnectionClosed;
		}
		else
		{
			int error = WSAGetLastError();
			switch (error)
			{
				case ETIMEDOUT:
					return SocketReturn::Timeout;
				default:
					return SocketReturn::Error;
			}
		}
	}

	SocketReturn::T SocketPOSIX::Send(const char* buffer, int bufferSize)
	{
		int result = send(m_socketHandle, buffer, bufferSize, 0);

		if (result > 0)
		{
			printf("Message sent: %.*s (%d bytes)\n", result, buffer, result);
		}
		else
		{
			int error = WSAGetLastError();
			switch (error)
			{
				case ETIMEDOUT:
					printf("Timeout occurred\n");
					return SocketReturn::Timeout;
				case ECONNRESET:
					printf("Timeout closed\n");
					return SocketReturn::ConnectionClosed;
				default:
					return SocketReturn::Error;
			};
		}

		return (SocketReturn::T)result;
	}

	SocketReturn::T SocketPOSIX::Shutdown(ShutdownOption option)
	{
		int result = 0;
		int iOption = SD_BOTH;

		switch (option)
		{
#if defined(_WIN32)
			case ShutdownOption::Send: iOption = SD_SEND; break;
			case ShutdownOption::Receive: iOption = SD_RECEIVE; break;
			case ShutdownOption::Both: iOption = SD_BOTH; break;
			default: iOption = SD_BOTH; break;
#else
			case ShutdownOption::Send: iOption = SHUT_WR; break;
			case ShutdownOption::Receive: iOption = SHUT_RD; break;
			case ShutdownOption::Both: iOption = SHUT_RDWR; break;
			default: iOption = SD_BOTH; break;
#endif
		}

		result = shutdown(m_socketHandle, iOption);

		if (result == SOCKET_ERROR)
		{
			return SocketReturn::Error;
		}

		return SocketReturn::Ok;
	}
}
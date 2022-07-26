#include "tppSocketPOSIX.h"

#if defined(_WIN32)
	
	#include <winsock2.h>
	#include <ws2tcpip.h>

#endif

#include <cstdio>

namespace tpp
{
	// The error code values are different between Windows and Unix systems. Even though Windows used to
	// provide a compatibility layer they no longer recommend it. Therefore we set up out own translation
	// layer to handle it.
#if defined(_WIN32)

	#define TPP_EPREFIX(x) WSA##x

#else

	#define TPP_EPREFIX(x) x

#endif

	static const int TPP_WOULDBLOCK     = TPP_EPREFIX(EWOULDBLOCK);
	static const int TPP_INPROGRESS     = TPP_EPREFIX(EINPROGRESS);
	static const int TPP_ALREADY        = TPP_EPREFIX(EALREADY);
	static const int TPP_NOTSOCK        = TPP_EPREFIX(ENOTSOCK);
	static const int TPP_DESTADDRREQ    = TPP_EPREFIX(EDESTADDRREQ);
	static const int TPP_MSGSIZE        = TPP_EPREFIX(EMSGSIZE);
	static const int TPP_PROTOTYPE      = TPP_EPREFIX(EPROTOTYPE);
	static const int TPP_NOPROTOOPT     = TPP_EPREFIX(ENOPROTOOPT);
	static const int TPP_PROTONOSUPPORT = TPP_EPREFIX(EPROTONOSUPPORT);
	static const int TPP_SOCKTNOSUPPORT = TPP_EPREFIX(ESOCKTNOSUPPORT);
	static const int TPP_OPNOTSUPP      = TPP_EPREFIX(EOPNOTSUPP);
	static const int TPP_PFNOSUPPORT    = TPP_EPREFIX(EPFNOSUPPORT);
	static const int TPP_AFNOSUPPORT    = TPP_EPREFIX(EAFNOSUPPORT);
	static const int TPP_ADDRINUSE      = TPP_EPREFIX(EADDRINUSE);
	static const int TPP_ADDRNOTAVAIL   = TPP_EPREFIX(EADDRNOTAVAIL);
	static const int TPP_NETDOWN        = TPP_EPREFIX(ENETDOWN);
	static const int TPP_NETUNREACH     = TPP_EPREFIX(ENETUNREACH);
	static const int TPP_NETRESET       = TPP_EPREFIX(ENETRESET);
	static const int TPP_CONNABORTED    = TPP_EPREFIX(ECONNABORTED);
	static const int TPP_CONNRESET      = TPP_EPREFIX(ECONNRESET);
	static const int TPP_NOBUFS         = TPP_EPREFIX(ENOBUFS);
	static const int TPP_ISCONN         = TPP_EPREFIX(EISCONN);
	static const int TPP_NOTCONN        = TPP_EPREFIX(ENOTCONN);
	static const int TPP_SHUTDOWN       = TPP_EPREFIX(ESHUTDOWN);
	static const int TPP_TOOMANYREFS    = TPP_EPREFIX(ETOOMANYREFS);
	static const int TPP_TIMEDOUT       = TPP_EPREFIX(ETIMEDOUT);
	static const int TPP_CONNREFUSED    = TPP_EPREFIX(ECONNREFUSED);
	static const int TPP_LOOP           = TPP_EPREFIX(ELOOP);
	static const int TPP_NAMETOOLONG    = TPP_EPREFIX(ENAMETOOLONG);
	static const int TPP_HOSTDOWN       = TPP_EPREFIX(EHOSTDOWN);
	static const int TPP_HOSTUNREACH    = TPP_EPREFIX(EHOSTUNREACH);
	static const int TPP_NOTEMPTY       = TPP_EPREFIX(ENOTEMPTY);
	static const int TPP_PROCLIM        = TPP_EPREFIX(EPROCLIM);
	static const int TPP_USERS          = TPP_EPREFIX(EUSERS);
	static const int TPP_DQUOT          = TPP_EPREFIX(EDQUOT);
	static const int TPP_STALE          = TPP_EPREFIX(ESTALE);
	static const int TPP_REMOTE         = TPP_EPREFIX(EREMOTE);

	SocketPOSIX::SocketPOSIX() : m_socketHandle(INVALID_SOCKET)
	{
	}

	SocketPOSIX::SocketPOSIX(uint64_t socketHandle) : m_socketHandle(socketHandle)
	{
	}

	SocketReturn::T SocketPOSIX::Accept(const NetworkAddress& address, ISocket* const clientSocket)
	{
		if (clientSocket)
		{
			sockaddr clientAddr;
			socklen_t clientAddrSize = sizeof(clientAddr);
			SOCKET clientSocketHandle = accept(m_socketHandle, &clientAddr, &clientAddrSize);

			if (clientSocketHandle != INVALID_SOCKET)
			{
				SocketPOSIX* const posixSocket = static_cast<SocketPOSIX* const>(clientSocket);
				posixSocket->m_socketHandle = clientSocketHandle;
				posixSocket->m_isConnected = true;
				return SocketReturn::Ok;
			}
			else
			{
				SocketReturn::InvalidSocket;
			}
		}

		return SocketReturn::Error;
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
		SocketReturn::T result;

		addrinfo  hints = {};
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		char buffer[32];
		snprintf(buffer, sizeof(buffer), "%d", networkAddress.port);

		// Get the address info for the address and port
		addrinfo* addressInfo = nullptr;
		int getaddrResult = getaddrinfo(networkAddress.address, buffer, &hints, &addressInfo);
		if (getaddrResult == 0)
		{
			int connectResult = connect(m_socketHandle, addressInfo->ai_addr, (int)addressInfo->ai_addrlen);

			if (connectResult == SOCKET_ERROR)
			{
				result = GetLastError();

				if (result == tpp::SocketReturn::WouldBlock || result == tpp::SocketReturn::ConnectionInProgress)
				{
					// If the result would block or the connection is in progress (these messages typically happen when
					// using non-blocking sockets) we aren't yet connected but we also don't want to close the socket
					// as if an error had occurred
				}
				else if (result == tpp::SocketReturn::Ok)
				{
					m_isConnected = true;
					result = SocketReturn::Ok;
				}
				else
				{
					closesocket(m_socketHandle);
					m_socketHandle = INVALID_SOCKET;
					m_isConnected = false;
				}
			}
			else
			{
				m_isConnected = true;
				result = SocketReturn::Ok;
			}
		}
		else
		{
			result = SocketReturn::Error;
		}

		freeaddrinfo(addressInfo);

		return result;
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

		m_isConnected = false;

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

		m_socketHandle = INVALID_SOCKET;

		if (result == SOCKET_ERROR)
		{
			return SocketReturn::Error;
		}

		return SocketReturn::Ok;
	}

	SocketReturn::T SocketPOSIX::Listen(int port)
	{
		// https://docs.microsoft.com/en-us/windows/desktop/api/winsock/nf-winsock-setsockopt
		// Allows the socket to be bound to an address that is already in use. For more information, see bind. Not applicable on ATM sockets.
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
			return SocketReturn::InvalidSocket;
		}

		// Start listening
		result = listen(m_socketHandle, 8);
		if (result == SOCKET_ERROR)
		{
			closesocket(m_socketHandle);
			m_socketHandle = INVALID_SOCKET;
			return SocketReturn::InvalidSocket;
		}

		return SocketReturn::Ok;
	}

	// https://docs.microsoft.com/en-us/windows/desktop/api/winsock/nf-winsock-recv
	// http://man7.org/linux/man-pages/man2/recv.2.html
	SocketReturn::T SocketPOSIX::Receive(char* buffer, int bufferLength)
	{
		int recvResult = recv(m_socketHandle, buffer, bufferLength, 0);

		if (recvResult > 0)
		{
			printf("Message received: %.*s (%d bytes)\n", recvResult, buffer, recvResult);
			return (SocketReturn::T)recvResult;
		}
		else 
		{
			SocketReturn::T error;

			if (recvResult == 0)
			{
				error = SocketReturn::ConnectionClosed;
			}
			else
			{
				error = GetLastError();
			}

			if (error != SocketReturn::Timeout && error != SocketReturn::WouldBlock)
			{
				m_isConnected = false;
			}

			return error;
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
			return GetLastError();
		}

		return (SocketReturn::T)result;
	}

	void SocketPOSIX::SetBlockingI(bool blocking)
	{
		u_long mode = blocking ? 0 : 1;

#if defined(_WIN32)
		int result = ioctlsocket((SOCKET)m_socketHandle, FIONBIO, &mode);
#else
		int result = ioctl(m_socketHandle, FIONBIO, &mode);
#endif

		if (result == SOCKET_ERROR)
		{
			printf("Error setting socket blocking status");
		}
	}

	SocketReturn::T SocketPOSIX::Shutdown(Channel channel)
	{
		int result = 0;
		int iOption = SD_BOTH;

		switch (channel)
		{
#if defined(_WIN32)
			case Channel::Send: iOption = SD_SEND; break;
			case Channel::Receive: iOption = SD_RECEIVE; break;
			case Channel::Both: iOption = SD_BOTH; break;
			default: iOption = SD_BOTH; break;
#else
			case Channel::Send: iOption = SHUT_WR; break;
			case Channel::Receive: iOption = SHUT_RD; break;
			case Channel::Both: iOption = SHUT_RDWR; break;
			default: iOption = SHUT_RDWR; break;
#endif
		}

		result = shutdown(m_socketHandle, iOption);

		if (result == SOCKET_ERROR)
		{
			return SocketReturn::Error;
		}

		return SocketReturn::Ok;
	}

	// https://stackoverflow.com/questions/2876024/linux-is-there-a-read-or-recv-from-socket-with-timeout
	void SocketPOSIX::SetTimeout(Channel channel, uint32_t milliseconds)
	{
#if defined(_WIN32)
		DWORD timeout = milliseconds;
#else
		timeval timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = milliseconds * 1000;
#endif

		if (channel == Channel::Send || channel == Channel::Both)
		{
			setsockopt(m_socketHandle, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));
		}

		if (channel == Channel::Receive || channel == Channel::Both)
		{
			setsockopt(m_socketHandle, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
		}
	}

	tpp::SocketReturn::T SocketPOSIX::GetLastError()
	{
#if defined(_WIN32)
		int error = WSAGetLastError();
#else
		int error = errno;
#endif

		switch (error)
		{
			case TPP_WOULDBLOCK:
				//printf("Would block\n");
				return SocketReturn::WouldBlock;
			case TPP_TIMEDOUT:
				printf("Timeout occurred\n");
				return SocketReturn::Timeout;
			case TPP_CONNREFUSED:
			case TPP_CONNRESET:
				printf("Connection closed\n");
				return SocketReturn::ConnectionClosed;
			case TPP_NOTSOCK:
				printf("Not a socket\n");
				return SocketReturn::NotASocket;
			case TPP_INPROGRESS:
			case TPP_ALREADY:
				return SocketReturn::ConnectionInProgress;
			case TPP_ISCONN:
				return SocketReturn::Ok;
			default:
				return SocketReturn::Error;
		}
	}
}
#include "tppISocket.h"

namespace tpp
{
	NetworkAddress::NetworkAddress(const char* in_address, int in_port)
	{
		strncpy(address, in_address, sizeof(address));
		port = in_port;
	}

	void ISocket::SetBlocking(bool blocking)
	{
		m_isBlocking = blocking;
		SetBlockingI(blocking);
	}
}
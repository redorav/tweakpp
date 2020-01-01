#include "tppISocket.h"
#include <string>

namespace tpp
{
	NetworkAddress::NetworkAddress(const char* in_address, int in_port)
	{
		strncpy(address, in_address, sizeof(address));
		port = in_port;
	}
}
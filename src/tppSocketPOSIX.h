#pragma once

#include "tppISocket.h"

namespace tpp
{
	class SocketPOSIX : public ISocket
	{
	public:

		SocketPOSIX();

		SocketPOSIX(uint64_t socketHandle);

		virtual SocketReturn::T Accept(const NetworkAddress& address, ISocket* const clientSocket) override;

		virtual SocketReturn::T Connect(const NetworkAddress& address) override;

		virtual SocketReturn::T Close() override;

		virtual SocketReturn::T Create() override;

		virtual SocketReturn::T Listen(int port) override;

		virtual SocketReturn::T Receive(char* buffer, int bufferSize) override;

		virtual SocketReturn::T Send(const char* buffer, int bufferSize) override;

		virtual void SetBlockingI(bool blocking) override;

		virtual SocketReturn::T Shutdown(Channel option) override;

		virtual void SetTimeout(Channel channel, uint32_t milliseconds) override;

	private:

		static tpp::SocketReturn::T GetLastError();

		uint64_t m_socketHandle;
	};
}
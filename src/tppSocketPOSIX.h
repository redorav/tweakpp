#include "tppISocket.h"

namespace tpp
{
	class SocketPOSIX : public ISocket
	{
	public:

		SocketPOSIX();

		SocketPOSIX(int socketHandle);

		virtual ISocket* Accept(const NetworkAddress& address) override;

		virtual SocketReturn::T Connect(const NetworkAddress& address) override;

		virtual SocketReturn::T Close() override;

		virtual SocketReturn::T Create() override;

		virtual SocketReturn::T Listen(int port) override;

		virtual SocketReturn::T Receive(char* buffer, int bufferSize) override;

		virtual SocketReturn::T Send(const char* buffer, int bufferSize) override;

		virtual SocketReturn::T Shutdown(ShutdownOption option) override;

	private:

		int m_socketHandle;
	};
}
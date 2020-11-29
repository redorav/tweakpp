#include <cstdint>

namespace tpp
{
	// These values need to be negative so Send and Receive can return a positive value for the number of bytes
	// returned when reading from a socket
	namespace SocketReturn
	{
		enum T
		{
			Ok               =  0,
			Error            = -1,
			Timeout          = -2,
			InvalidSocket    = -3,
			ConnectionClosed = -4,
			WouldBlock       = -5,
		};
	};

	enum class Channel
	{
		Send,
		Receive,
		Both
	};

	struct NetworkAddress
	{
		NetworkAddress(const char* inAddress, int inPort);

		char address[32];
		int  port;
	};

	// Interface to a socket
	class ISocket
	{
	public:

		virtual ISocket* Accept(const NetworkAddress& address) = 0;

		virtual SocketReturn::T Connect(const NetworkAddress& address) = 0;

		// Destroys socket for any further transmission
		virtual SocketReturn::T Close() = 0;

		// Create the socket
		virtual SocketReturn::T Create() = 0;

		// Listen to a specific port
		virtual SocketReturn::T Listen(int port) = 0;

		// Receive data
		virtual SocketReturn::T Receive(char* buffer, int bufferSize) = 0;

		// Send data
		virtual SocketReturn::T Send(const char* buffer, int bufferSize) = 0;

		// Set blocking
		void SetBlocking(bool blocking);

		// Shut down socket for sending, receiving, or both
		virtual SocketReturn::T Shutdown(Channel option) = 0;

		virtual void SetTimeout(Channel channel, uint32_t milliseconds) = 0;

		// Return if socket is blocking
		bool IsBlocking();

	private:

		virtual void SetBlockingI(bool blocking) = 0;

		bool m_isBlocking;
	};

	inline bool ISocket::IsBlocking()
	{
		return m_isBlocking;
	}
}
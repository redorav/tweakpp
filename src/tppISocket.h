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

		virtual SocketReturn::T Accept(const NetworkAddress& address, ISocket* const clientSocket) = 0;

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
		bool IsBlocking() const;

		bool IsConnected() const;

	protected:

		virtual void SetBlockingI(bool blocking) = 0;

		// Sockets are never actually 'connected'. Every send or receive can return a failure of some kind, so all
		// we can really do is assume that if we were connected in the past we might be connected now. However,
		// knowing that we don't have a connection open or that it has been dropped does allow us to make good
		// decisions, such as to reopen the connection
		bool m_isConnected = false;

		bool m_isBlocking;
	};

	inline bool ISocket::IsBlocking() const
	{
		return m_isBlocking;
	}

	inline bool ISocket::IsConnected() const
	{
		return m_isConnected;
	}
}
#pragma once

#include <cstdint>

namespace tpp
{
	// These values need to be negative so Send and Receive can return a positive value for the number of bytes
	// returned when reading from a socket
	namespace SocketReturn
	{
		enum T
		{
			Ok                   =  0, // Operation completed successfully
			Error                = -1, // A generic error
			Timeout              = -2, // Operation timed out (if we have a timeout on the socket)
			InvalidSocket        = -3, // Socket is invalid
			ConnectionClosed     = -4, // One of the ends closed the connection
			WouldBlock           = -5, // Non-blocking sockets call receive or send that would normally block but aren't ready yet
			NotASocket           = -6, // Normally means we have called a function on an invalid socket handle
			ConnectionInProgress = -7, // A connection in progress typically happens when a non-blocking socket is trying to connect
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

		// Accept incoming connection that is being listened to. Typically the server does this
		virtual SocketReturn::T Accept(const NetworkAddress& address, ISocket* const clientSocket) = 0;

		// Try to establish a connection with someone who is listening to this address
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

		// Set timeout on send and receive operations
		virtual void SetTimeout(Channel channel, uint32_t milliseconds) = 0;

		// Return whether socket is blocking
		bool IsBlocking() const;

		// Return whether socket's last operation was successful
		// See explanation below for what this means
		bool IsConnected() const;

	protected:

		virtual void SetBlockingI(bool blocking) = 0;

		// Sockets are never actually 'connected'. Every send or receive can return a failure of some kind, so all
		// we can really do is assume that if we were connected in the past we might be connected now. However,
		// knowing that we don't have a connection open or that it has been dropped does allow us to make good
		// decisions, such as to reopen the connection
		bool m_isConnected = false;

		bool m_isBlocking = true;
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
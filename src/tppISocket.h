
namespace tpp
{
	// These values need to be negative so Send and Receive can return the same error values
	// as the other functions (bar the fact that they are cast as int)
	namespace SocketReturn
	{
		enum T
		{
			Ok               =  0,
			Error            = -1,
			Timeout          = -2,
			InvalidSocket    = -3,
			ConnectionClosed = -4
		};
	};

	enum class ShutdownOption
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

		virtual SocketReturn::T Close() = 0;

		virtual SocketReturn::T Create() = 0;

		virtual SocketReturn::T Listen(int port) = 0;

		virtual SocketReturn::T Receive(char* buffer, int bufferSize) = 0;

		virtual SocketReturn::T Send(const char* buffer, int bufferSize) = 0;

		virtual SocketReturn::T Shutdown(ShutdownOption option) = 0;
	};

}
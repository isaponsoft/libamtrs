#ifndef	__inarikvs__server__acceptor__hpp
#define	__inarikvs__server__acceptor__hpp
#include <amtrs/net.hpp>
#include <amtrs/mutex.hpp>
#include "../inarikvs-def.hpp"
#include "../inarikvs-types.hpp"
INARIKVS_NAMESPACE_BEGIN

class	acceptor
{
public:
	using	socket_type	= amtrs::net::socket;

	acceptor(uint16_t _port)
		: mPort(_port)
		, mSocket(socket_type(AF_INET, SOCK_STREAM, 0))
	{}

	void bind(int _acceptCount)
	{
		struct sockaddr_in	bindAddr;
		bindAddr.sin_family			= PF_INET;
		bindAddr.sin_addr.s_addr	= htonl(INADDR_ANY);
		bindAddr.sin_port			= htons(mPort);

		mSocket.bind(bindAddr);
		mSocket.listen(5);
	}

	bool empty() const noexcept { return mSocket.empty(); }

	void noneblocking()
	{
		mSocket.nonebloking();
	}
	
	socket_type accept() { return mSocket.accept(); }

	uint16_t port() const noexcept { return mPort; }
	
private:
	socket_type		mSocket;
	uint16_t		mPort;
};



INARIKVS_NAMESPACE_END
#endif

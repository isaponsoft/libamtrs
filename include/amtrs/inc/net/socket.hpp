#ifndef	__libamtrs__net__socket__hpp
#define	__libamtrs__net__socket__hpp
#if		AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_WIN32
#pragma	comment(lib, "wsock32.lib")
#pragma	comment(lib, "ws2_32.lib")
#include <atomic>
#else
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#endif
#include "def.hpp"
AMTRS_NET_NAMESPACE_BEGIN


#if		AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_WIN32
#else
struct	socket_trits
{
	using	socket_type			= int;
	using	socklen_type		= socklen_t;

	struct	startupper{};

	union	address
	{
		struct sockaddr		addr;
		struct sockaddr_in	inet;
	};

	static constexpr socklen_type	nsock	= -1;

	static void socket_close(socket_type _socket)
	{
		::close(_socket);
	}

	static void noneblock(socket_type _socket)
	{
		int		val = 1;
		ioctl(_socket, FIONBIO, &val);
	}
};
#endif

template<class TritsT>
class	basic_socket : TritsT::startupper
{
public:
	using	trits_type		= TritsT;
	using	socket_type		= typename trits_type::socket_type;
	using	socklen_type	= typename trits_type::socklen_type;
	using	address_type	= typename trits_type::address;

	static constexpr socklen_type	nsock	= trits_type::nsock;


	//! ソケットを指定したファミリで初期化します。
	basic_socket(int _socketFamily, int _socketType, int _protocol)
		: mSocket(::socket(_socketFamily, _socketType, _protocol))
	{}

	//! 接続要求を受け付けます。
	basic_socket accept(struct sockaddr& _clientAddr, std::size_t& _addrSize)
	{
		socklen_type	len	= static_cast<socklen_type>(_addrSize);
		auto			s	= ::accept(get(), &_clientAddr, &len);
		_addrSize	= len;
		return	basic_socket(s);
	}

	//! 接続要求を受け付けます。
	basic_socket accept(struct sockaddr_in& _clientAddr, std::size_t& _addrSize)
	{
		return	accept(reinterpret_cast<struct sockaddr&>(_clientAddr), _addrSize);
	}

	//! 接続要求を受け付けます。
	basic_socket accept()
	{
		struct sockaddr	d1;
		std::size_t		d2	= sizeof(d1);
		return	accept(d1, d2);
	}



	//! インターネットアドレスをバインドします。
	int bind(const sockaddr_in& _inaddr)
	{
		return	bind(reinterpret_cast<const sockaddr*>(&_inaddr), sizeof(sockaddr_in));
	}

	//! インターネットアドレスをバインドします。
	int bind(const struct sockaddr* _addr, std::size_t _addrSize)
	{
		return	::bind(get(), _addr, _addrSize);
	}

	//! リスニングを開始します。
	int listen(int _maxAccept)
	{
		return	::listen(get(), _maxAccept);
	}

	//! ノンブロッキングモードに変更します。
	void nonebloking()
	{
		trits_type::noneblock(get());
	}

	//! データを受信します。
	//!	\return
	//!		受信したバイト数を返します。
	int recv(void* _buf, std::size_t _buffSize, int _flags = 0)
	{
		if (empty())
		{
			return	-1;
		}

		int		sz	= ::recv(get(), (char*)_buf, _buffSize, _flags);
		if (sz == -1)
		{
			if ((errno == EAGAIN) || (errno == EWOULDBLOCK ))
			{
				// ノンブロックの時、まだ届いていない
			}
			else if ((errno == ENOTSOCK) || (errno == ECONNREFUSED) || (errno == EBADF))
			{
				// 接続が閉じられた
				clear();
			}
		}
		if (sz == 0)
		{
			// EOF
			clear();
		}
		return	sz;
	}

	//! データを受信します。
	int send(const void* _buf, std::size_t _buffSize, int _flags = 0)
	{
		return	::send(get(), (const char*)_buf, _buffSize, _flags);
	}


	explicit basic_socket(socket_type _s)
		: mSocket(_s)
	{}

	basic_socket() = default;
	basic_socket(const basic_socket&) = delete;
	basic_socket& operator = (const basic_socket&) = delete;

	basic_socket(basic_socket&& _r)
		: mSocket(_r.release())
	{}

	basic_socket& operator = (basic_socket&& _r)
	{
		clear();
		mSocket	= _r.release();
		return	*this;
	}

	~basic_socket()
	{
		clear();
	}

	bool empty() const noexcept { return mSocket == nsock; }

	//! ソケットの接続を切り消去します。
	void clear()
	{
		if (!empty())
		{
			trits_type::socket_close(mSocket);
			mSocket	= nsock;
		}
	}

	//! ソケットのハンドルを返します。
	socket_type get() const noexcept { return mSocket; }

	//! ソケットの所有権を破棄します。
	socket_type release() noexcept
	{
		socket_type	retval	= mSocket;
		mSocket	= nsock;
		return	retval;
	}

	void swap(basic_socket& _b) noexcept
	{
		std::swap(mSocket, _b.mSocket);
	}

private:
	socket_type		mSocket		= nsock;
};
AMTRS_NET_NAMESPACE_END


#if		AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_WIN32
AMTRS_NET_NAMESPACE_BEGIN

class	winsock_startupper
{
public:
	static void link()
	{
		if (ref().load() == 0)
		{
			WSADATA	ws = {};
			WSAStartup(MAKEWORD(2,0), &ws);
		}
		++ref();
	}

	static void unlink()
	{
		--ref();
		if (ref().load() == 0)
		{
			WSACleanup();
		}
	}

private:
	static std::atomic<unsigned int>& ref()
	{
		static std::atomic<unsigned int>	s_ref;
		return	s_ref;
	}
};



struct	socket_trits
{
	struct	startupper
	{
		startupper() { winsock_startupper::link(); }
		~startupper() { winsock_startupper::unlink(); }
	};



	using	socket_type			= SOCKET;
	using	socklen_type		= int;




	union	address
	{
		struct sockaddr		addr;
		struct sockaddr_in	inet;
	};

	static constexpr socket_type		nsock	= INVALID_SOCKET;

	static void socket_close(socket_type _socket)
	{
		::closesocket(_socket);
	}

	static void noneblock(socket_type _socket)
	{
		unsigned long	val = 1;
		ioctlsocket(_socket, FIONBIO, &val);
	}
};



AMTRS_NET_NAMESPACE_END
#endif

AMTRS_NET_NAMESPACE_BEGIN
using	socket	= basic_socket<socket_trits>;
AMTRS_NET_NAMESPACE_END
#endif

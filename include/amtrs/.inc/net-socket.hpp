/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__net__socket__socket_traits_hpp
#define	__libamtrs__net__socket__socket_traits_hpp
#if		AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_WIN32
#pragma	comment(lib, "wsock32.lib")
#pragma	comment(lib, "ws2_32.lib")
#include <atomic>
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#endif
AMTRS_NET_NAMESPACE_BEGIN

struct	__socket_info;
template<class Traits>	class	basic_socket;



using	socket	= basic_socket<__socket_info>;


// ****************************************************************************
//! socket(BSD)のラッパー。
// ----------------------------------------------------------------------------
//! OSによる初期化と終了処理の違いを吸収します。
// ----------------------------------------------------------------------------
template<class Traits>
class	basic_socket : Traits::startupper, public netbase
{
public:
	using	trits_type		= Traits;
	using	socket_type		= typename trits_type::socket_type;
	using	socklen_type	= typename trits_type::socklen_type;


	static constexpr socket_type	nsock	= trits_type::nsock;


	static basic_socket<Traits> connect(const address& _addr)
	{
		if (_addr.empty())
		{
			AMTRS_DEBUG_LOG("address error");
			return	{};
		}

		basic_socket<Traits>	s(_addr->ai_family, _addr->ai_socktype, _addr->ai_protocol);
		if (s.empty())
		{
			AMTRS_DEBUG_LOG("socket error : %s", (std::error_code(errno, std::generic_category()).message()).c_str());
			return	{};
		}
		if (s.connect(_addr->ai_addr, _addr->ai_addrlen) < 0)
		{
			int	e	= errno;
			AMTRS_DEBUG_LOG("socket connection error : %s", (std::error_code(e, std::generic_category()).message()).c_str());
			if (e == EINPROGRESS)
			{
				fd_set	sockets;
				FD_ZERO(&sockets);
				FD_SET(s.get(), &sockets);
				while (select(static_cast<int>(s.get()) + 1, nullptr, &sockets, nullptr, nullptr) <= 0){}
			}

			e	= errno;
			if (e)
			{
				AMTRS_DEBUG_LOG("socket connection error : %s", (std::error_code(errno, std::generic_category()).message()).c_str());
				s.clear();
				errno	= e;
				return	{};
			}
		}
		return	s;
		
	}

	template<class URI>
	static basic_socket<Traits> connect(uri::info<char>* _buff, const URI& _uri, int _socketType = 0)
	{
		uri::info<char>	t;
		if (!_buff)
		{
			_buff	= &t;
		}
		uri::parse(*_buff, (std::string_view)_uri);
		address	adr(*_buff, _socketType);
		if (adr.empty())
		{
			AMTRS_DEBUG_LOG("address error host = \"%s\".", ((std::string)_buff->host).c_str());
			return	{};
		}
		return	connect(adr);
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

/*
	//! 接続要求を受け付けます。
	basic_socket accept(struct sockaddr_in& _clientAddr, std::size_t& _addrSize)
	{
		return	accept(reinterpret_cast<struct sockaddr&>(_clientAddr), _addrSize);
	}
*/

	//! 接続要求を受け付けます。
	basic_socket accept()
	{
		struct sockaddr	d1;
		std::size_t		d2	= sizeof(d1);
		return	accept(d1, d2);
	}


	size_t mtu()
	{
#ifdef	IP_MTU
		int				mtusize	= 0;
		socklen_type	optlen	= sizeof(mtusize);
		if (::getsockopt(mSocket, 0, IP_MTU, (char*)&mtusize, &optlen))
		{
			#if	0
			auto	e	= errno;
			switch (e)
			{
				case EBADF : std::cout << "EBADF" << std::endl; break;
				case EFAULT : std::cout << "EFAULT" << std::endl; break;
				case EINVAL : std::cout << "EINVAL" << std::endl; break;
				case ENOPROTOOPT : std::cout << "ENOPROTOOPT" << std::endl; break;
				case ENOTSOCK : std::cout << "ENOTSOCK" << std::endl; break;
			}
			#endif
			mtusize	= 1500;
		}
		AMTRS_DEBUG_LOG("MTU %d %d", (int)mtusize, (int)optlen);
		return	mtusize;
#else
		return	1500;
#endif
	}

/*
	//! インターネットアドレスをバインドします。
	int bind(struct sockaddr_in const& _inaddr)
	{
		return	bind(reinterpret_cast<const sockaddr*>(&_inaddr), sizeof(_inaddr));
	}

*/
	//! インターネットアドレスをバインドします。
	int bind(const struct sockaddr* _addr, std::size_t _addrSize)
	{
		return	::bind(get(), _addr, _addrSize);
	}


	int connect(struct sockaddr const* _addr, size_t _addrSize)
	{
		return	::connect(get(), _addr, static_cast<socklen_type>(_addrSize));
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
		return	::recv(get(), (char*)_buf, _buffSize, _flags);
	}


	//! データを受信します。
	int send(const void* _buf, std::size_t _buffSize, int _flags = 0)
	{
		return	::send(get(), (const char*)_buf, _buffSize, _flags);
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


#if		AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_WIN32
struct	__socket_info
{
	struct	startupper
	{
		startupper() { link(); }
		~startupper() { unlink(); }
	};

	using	socket_type			= SOCKET;
	using	socklen_type		= int;

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


	static void link()
	{
		if (ref().load() == 0)
		{
			WSADATA	ws = {};
			int	err	= WSAStartup(MAKEWORD(2,0), &ws);
			if (err)
			{
				std::error_code(err, std::generic_category());
			}
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
#else
struct	__socket_info
{
	using	socket_type			= int;
	using	socklen_type		= socklen_t;

	struct	startupper{};


	static constexpr socket_type	nsock	= -1;

	static void socket_close(socket_type _socket)
	{
		::close(_socket);
	}

	static void noneblock(socket_type _socket)
	{
#ifdef	FIONBIO
		int		val = 1;
		ioctl(_socket, FIONBIO, &val);
#elif	defined(F_SETFL) && defined(O_NONBLOCK)
		fcntl(_socket, F_SETFL, O_NONBLOCK);
#else
#		warning "None blocking mode unsupported."
#endif
	}
};
#endif


AMTRS_NET_NAMESPACE_END
#endif

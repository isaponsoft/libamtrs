/******************************************************************************
Copyright (c) 2019, isaponsoft (Isao Shibuya)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the libamtrs project.
******************************************************************************/
#ifndef	__libamtrs__net__socket__socket__hpp
#define	__libamtrs__net__socket__socket__hpp
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
#include "../address.hpp"
AMTRS_NET_NAMESPACE_BEGIN

// ****************************************************************************
//! socket(BSD)のラッパーです。
// ----------------------------------------------------------------------------
//! OSによる初期化と終了処理の違いを吸収します。
// ----------------------------------------------------------------------------
template<class Traits>
class	basic_socket : Traits::startupper
{
public:
	using	trits_type		= Traits;
	using	socket_type		= typename trits_type::socket_type;
	using	socklen_type	= typename trits_type::socklen_type;
	using	address_type	= typename trits_type::address;

	static constexpr socklen_type	nsock	= trits_type::nsock;


	static basic_socket<Traits> connect(const address& _addr)
	{
std::cout << "1" << std::endl;
		if (_addr.empty())
		{
std::cout << "_addr.empty" << std::endl;
			return	{};
		}

		basic_socket<Traits>	s(_addr->ai_family, _addr->ai_socktype, _addr->ai_protocol);
		if (s.empty())
		{
std::cout << "sock err" << std::endl;
			return	{};
		}

		if (s.connect(_addr->ai_addr, _addr->ai_addrlen) < 0)
		{
			int	e	= errno;
std::cout << e << "/" << EINPROGRESS << std::endl;
			if (e != EINPROGRESS)
			{
				s.clear();
				errno	= e;
				return	{};
			}
			fd_set	sockets;
			FD_ZERO(&sockets);
			FD_SET(s.get(), &sockets);
			while (select(s.get() + 1, nullptr, &sockets, nullptr, nullptr) <= 0){}
		}
std::cout << "2" << std::endl;
		
		return	s;
		
	}

	template<class URI>
	static basic_socket<Traits> connect(uri::info<char>* _buff, const URI& _uri)
	{
		uri::info<char>	t;
		if (!_buff)
		{
			_buff	= &t;
		}
		uri::parse(*_buff, (std::string_view)_uri);
		address	adr(*_buff);
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


	int connect(struct sockaddr const* _addr, size_t _addrSize)
	{
		return	::connect(get(), _addr, _addrSize);
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



struct	socket_traits
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
#else
struct	socket_traits
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
using	socket	= basic_socket<socket_traits>;
AMTRS_NET_NAMESPACE_END
#endif

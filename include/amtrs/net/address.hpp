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
#ifndef	__libamtrs__net__address__hpp
#define	__libamtrs__net__address__hpp
#if		AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_WIN32
#pragma	comment(lib, "wsock32.lib")
#pragma	comment(lib, "ws2_32.lib")
#define	NOMINMAX		// 
#include <winsock2.h>
#include <ws2tcpip.h>
#include <atomic>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#endif
#include "uri.hpp"
AMTRS_NET_NAMESPACE_BEGIN


// ****************************************************************************
//! socket addrinfo を簡単に扱うためのラッパークラスです。
// ----------------------------------------------------------------------------
//! ムーブはできますがコピーは禁止です。自動的に　freeaddrinfo() を呼び出し
//! 不要になったaddrinfoを開放します。
// ----------------------------------------------------------------------------
class	address
{
public:
	using	value_type	= ::addrinfo;

	address() = default;
	address(address&& _r)
		: mAddrInfo(_r.mAddrInfo)
	{
		_r.mAddrInfo	= nullptr;
	}

	address(value_type* _r)
		: mAddrInfo(_r)
	{}

	address& operator = (address&& _r)
	{
		clear();
		mAddrInfo		= _r.mAddrInfo;
		_r.mAddrInfo	= nullptr;
		return	*this;
	}

	// ========================================================================
	//! 指定するホストとサービス（またはポート）に対するアドレス情報を取得します。
	// ------------------------------------------------------------------------
	//! 成否は empty() で調べることができます。
	// ------------------------------------------------------------------------
	address(char const* _host, char const* _service, struct addrinfo const* _hints = nullptr)
	{
		value_type	hints;
		if (!_hints)
		{
			hints	= {0};
			_hints	= &hints;
		}
		initialize(*_hints, _host, _service);
	}


	// ========================================================================
	//! uri::info で指定するホストとサービス（またはポート）に対するアドレス情報を取得します。
	// ------------------------------------------------------------------------
	//! 成否は empty() で調べることができます。
	// ------------------------------------------------------------------------
	template<class CharT>
	address(uri::info<CharT> const& _info, int _socketFamily = 0, int _socketType = 0)
	{
		value_type	hints	= {0};
		hints.ai_family		= _socketFamily;
		hints.ai_socktype	= _socketType;

		std::string			hostname(_info.host);
		std::string			service(_info.port.empty() ? _info.scheme : _info.port);

		initialize(hints, hostname.data(), service.data());
	}

	~address()
	{
		clear();
	}

	void clear()
	{
		if (mAddrInfo)
		{
			::freeaddrinfo(mAddrInfo);
		}
	}

	bool empty() const noexcept
	{
		return	mAddrInfo == nullptr;
	}



	// ========================================================================
	//! addrinfoのメンバへアクセスします。
	// ------------------------------------------------------------------------
	const value_type* operator -> () const noexcept { return mAddrInfo; }



	bool initialize(struct addrinfo const& _hints, char const* _host, char const* _service)
	{
		clear();
		value_type*	addr;
		if (auto err = getaddrinfo(_host, _service, &_hints, &addr); !err)
		{
			mAddrInfo	= addr;
			return	true;
		}

#if	0
		char		name[NI_MAXHOST] = { 0 };
		char		ntop[NI_MAXHOST] = { 0 };
		using		type	= struct sockaddr;
		type*		saddr	= new type();
		socklen_t 	len		= sizeof(type);
		int			ecode	= getnameinfo(saddr, len, ntop, sizeof(ntop), NULL, 0, NI_NUMERICHOST);
		if (ecode != 0)
		{
			std::cout << "getnameinfo:" << gai_strerror(ecode) << std::endl;
		}
		else
		{
			
			std::cout << "OK" << std::endl;
			std::cout << (char*)name << std::endl;
			std::cout << (char*)ntop << std::endl;
			return	true;
		}
#endif
		return	false;
	}


private:
	value_type*	mAddrInfo	= nullptr;
};


AMTRS_NET_NAMESPACE_END
#endif

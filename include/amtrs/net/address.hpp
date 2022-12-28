/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__net__address__hpp
#define	__libamtrs__net__address__hpp
#include <system_error>
#include "../string/sstring.hpp"
#include "uri.hpp"
#if		AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_WIN32
#pragma	comment(lib, "wsock32.lib")
#pragma	comment(lib, "ws2_32.lib")
#include <ws2tcpip.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#endif

#include <iostream>

AMTRS_NET_NAMESPACE_BEGIN

struct	netbase
{
	netbase();
	~netbase();
};


// ****************************************************************************
//! socket addrinfo を簡単に扱うためのラッパークラスです。
// ----------------------------------------------------------------------------
//! ムーブはできますがコピーは禁止です。自動的に　freeaddrinfo() を呼び出し
//! 不要になったaddrinfoを開放します。
// ----------------------------------------------------------------------------
class	address : public netbase
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

	address(char const* _host, uint16_t _port, struct addrinfo const* _hints = nullptr)
	{
		value_type	hints;
		if (!_hints)
		{
			hints	= {0};
			_hints	= &hints;
		}
		char	service[100];
		char*	cur	= service + sizeof(service);
		*--cur	= 0;
		while (_port)
		{
			*--cur	= '0' + (_port % 10);
			_port	= _port / 10;
		}
		initialize(*_hints, _host, cur);
	}


	// ========================================================================
	//! uri::info で指定するホストとサービス（またはポート）に対するアドレス情報を取得します。
	// ------------------------------------------------------------------------
	//! 成否は empty() で調べることができます。
	// ------------------------------------------------------------------------
	template<class CharT>
	address(uri::info<CharT> const& _info, int _socketType = 0, int _socketFamily = 0, int _protocol = 0)
	{
		value_type	hints	= {0};
		hints.ai_family		= AF_UNSPEC;
		hints.ai_socktype	= SOCK_STREAM;
		hints.ai_protocol 	= _protocol;
		sstring				hostname(_info.host);
		sstring				service(_info.port.empty() ? _info.scheme : _info.port);
		if (service == "http")
		{
			service	= "80";
		}
		if (service == "https")
		{
			service	= "443";
		}
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


	const value_type* get() const noexcept { return mAddrInfo; }
	value_type* get() noexcept { return mAddrInfo; }


	bool initialize(struct addrinfo const& _hints, char const* _host, char const* _service)
	{
		clear();
		value_type*	addr;
		if (auto err = getaddrinfo(_host, _service, &_hints, &addr); !err)
		{
			mAddrInfo	= addr;
			return	true;
		}

#if	1
		char		name[NI_MAXHOST] = { 0 };
		char		ntop[NI_MAXHOST] = { 0 };
		using		type	= struct sockaddr;
		type*		saddr	= new type();
		socklen_t 	len		= sizeof(type);
		int			ecode	= getnameinfo(saddr, len, ntop, sizeof(ntop), NULL, 0, NI_NUMERICHOST);
		if (ecode != 0)
		{
			throw	std::error_code(ecode, std::generic_category());
		}
		else
		{
			mAddrInfo	= reinterpret_cast<value_type*>(saddr);
			return	true;
		}
#endif
		throw	std::error_code(ecode, std::generic_category());
		return	false;
	}


private:
	value_type*	mAddrInfo	= nullptr;
};


AMTRS_NET_NAMESPACE_END
#endif

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
#ifndef	__libamtrs__opt__net__ssl_stream__hpp
#define	__libamtrs__opt__net__ssl_stream__hpp
#if	__has_include(<openssl/ssl.h>)
#define	AMTRS_SSL_SUPPORTED	1
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "../../net/http/def.hpp"
AMTRS_NET_NAMESPACE_BEGIN


template<class CharT>
class	ssl_stream
{
public:
	using	socket_type		= typename socket_traits::socket_type;
	using	socklen_type	= typename socket_traits::socklen_type;

	using	char_type		= CharT;
	using	off_type		= typename std::basic_iostream<CharT>::off_type;
	using	pos_type		= typename std::basic_iostream<CharT>::pos_type;
	using	iostate			= typename std::basic_iostream<CharT>::iostate;
	using	size_type		= size_t;

	static constexpr socklen_type	nsock	= socket_traits::nsock;

	ssl_stream() = delete;

	ssl_stream(ssl_stream&& _r)
		: mSocket(_r.mSocket)
		, mSSL(_r.mSSL)
		, mContext(_r.mContext)
	{
		_r.mSocket	= nsock;
		_r.mSSL		= nullptr;
		_r.mContext	= nullptr;
	}

	ssl_stream(socket_type _s)
		: mSocket(_s)
	{
		SSL_library_init();
		mContext	= SSL_CTX_new(SSLv23_client_method());
		mSSL		= SSL_new(mContext);

		if (SSL_set_fd(mSSL, mSocket))
		{
			if (SSL_connect(mSSL) == 1)
			{
				return;
			}
		}
		setstate(std::ios::badbit);
	}

	ssl_stream& operator = (ssl_stream&& _r)
	{
		mSocket		= _r.mSocket;
		mSSL		= _r.mSSL;
		mContext	= _r.mContext;
		_r.mSocket	= nsock;
		_r.mSSL		= nullptr;
		_r.mContext	= nullptr;
		return	*this;
	}


	~ssl_stream()
	{
		if (mSSL)
		{
			SSL_free(mSSL);
			SSL_CTX_free(mContext);
		}
	}

	ssl_stream& read(char_type* _buff, size_t _n)
	{
		mGCount	= SSL_read(mSSL, _buff, _n);
		return	*this;
	}

	ssl_stream& write(char_type const* _buff, size_t _n)
	{
		mPCount	= SSL_write(mSSL, _buff, _n);
		return	*this;
	}



	size_t gcount() const noexcept { return mGCount; }
	size_t pcount() const noexcept { return mPCount; }


	explicit operator bool() const { return good(); }

	bool good() const noexcept { return rdstate() == std::ios::goodbit; }
	bool eof()  const noexcept { return rdstate() & std::ios::eofbit  ? true : false; }
	bool fail() const noexcept { return rdstate() & std::ios::failbit ? true : false; }
	bool bad()  const noexcept { return rdstate() & std::ios::badbit  ? true : false; }

	iostate rdstate() const noexcept { return mStatus; }
	void setstate(iostate _state) noexcept { clear(rdstate()|_state); }
	void clear(iostate _state = std::ios::goodbit) noexcept { mStatus = _state; }


private:
	socket_type		mSocket 	= nsock;
	SSL_CTX*		mContext	= nullptr;
	SSL*			mSSL		= nullptr;

	iostate			mStatus		= std::ios::goodbit;
	size_t			mGCount		= 0;
	size_t			mPCount		= 0;
};



AMTRS_NET_NAMESPACE_END
#else	// __has_include(<openssl/ssl.h>)
#define	AMTRS_SSL_SUPPORTED	0
#endif	// __has_include(<openssl/ssl.h>)
#endif

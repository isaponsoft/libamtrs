/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__opt__net__ssl_stream__hpp
#define	__libamtrs__opt__net__ssl_stream__hpp
#if		AMTRS_SSL_ENABLE
#include <openssl/ssl.h>
#include <openssl/err.h>
AMTRS_NET_NAMESPACE_BEGIN


template<class CharT, class Sock>
class	basic_ssl_stream;

using	ssl_stream	= basic_ssl_stream<char, socket>;


template<class CharT, class Sock>
class	basic_ssl_stream
{
public:
	using	socket_type		= typename Sock::socket_type;
	using	socklen_type	= typename Sock::socklen_type;

	using	char_type		= CharT;
	using	off_type		= typename std::basic_iostream<CharT>::off_type;
	using	pos_type		= typename std::basic_iostream<CharT>::pos_type;
	using	iostate			= typename std::basic_iostream<CharT>::iostate;
	using	size_type		= size_t;

	static constexpr socklen_type	nsock	= static_cast<socklen_type>(Sock::nsock);

	basic_ssl_stream() = delete;


	basic_ssl_stream(basic_ssl_stream&& _r)
		: mSocket(std::move(_r.mSocket))
		, mSSL(_r.mSSL)
		, mContext(_r.mContext)
	{
		_r.mSSL		= nullptr;
		_r.mContext	= nullptr;
	}

	basic_ssl_stream(Sock _s)
		: mSocket(std::move(_s))
	{
		SSL_library_init();
		mContext	= SSL_CTX_new(SSLv23_client_method());
		mSSL		= SSL_new(mContext);

		if (SSL_set_fd(mSSL, static_cast<int>(mSocket.get())))
		{
			if (SSL_connect(mSSL) == 1)
			{
				return;
			}
		}
		setstate(std::ios::badbit);
	}


	~basic_ssl_stream()
	{
		if (mSSL)
		{
			SSL_shutdown(mSSL);
			SSL_free(mSSL);
			SSL_CTX_free(mContext);
		}
	}

	basic_ssl_stream& read(char_type* _buff, size_t _n)
	{
AMTRS_WARN_LOG("ssl recv");
		auto	rs	= SSL_read(mSSL, _buff, static_cast<int>(_n));
		auto	s	= SSL_get_error(mSSL, static_cast<int>(rs));
AMTRS_WARN_LOG("ssl recv %d %d", (int)rs, (int)s);
		mGCount	= rs >= 0
				? rs
				: 0;
		switch (s)
		{
			// Success.
			case SSL_ERROR_NONE:
			{
				break;
			}

			// End data
			case SSL_ERROR_ZERO_RETURN:
			{
				setstate(std::ios::eofbit);
				break;
			}

			// Pending
			case SSL_ERROR_WANT_READ:
			{
				break;
			}

			// Pending
			case SSL_ERROR_WANT_WRITE:
			{
				break;
			}

			// System calling
			case SSL_ERROR_SYSCALL:
			{
				setstate(std::ios::failbit);
				break;
			}

			default:
			{
				setstate(std::ios::failbit);
				break;
			}
		}
		return	*this;
	}

	basic_ssl_stream& write(char_type const* _buff, size_t _n)
	{
		mPCount	= SSL_write(mSSL, _buff, static_cast<int>(_n));
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
	Sock			mSocket;
	SSL*			mSSL		= nullptr;
	SSL_CTX*		mContext	= nullptr;

	iostate			mStatus		= std::ios::goodbit;
	size_t			mGCount		= 0;
	size_t			mPCount		= 0;
};



AMTRS_NET_NAMESPACE_END


AMTRS_IO_NAMESPACE_BEGIN

template<class Elm>
struct	streamif_traits<net::ssl_stream, Elm>
{
	using	char_type	= Elm;
	using	value_type	= net::ssl_stream;
	using	size_type	= std::streamsize;

protected:
	streamif_traits(net::ssl_stream*)
	{}

	streamif_base::iostate read(value_type& _value, size_type& _readsize, char_type* _data, size_type _size)
	{
		_value.read(_data, _size);
		_readsize	= _value.gcount();
		return	_value.rdstate();
	}

	streamif_base::iostate write(value_type& _value, size_type& _readsize, char_type const* _data, size_type _size)
	{
		_value.write(_data, _size);
		_readsize	= _value.pcount();
		return	_value.rdstate();
	}
};


inline auto make_streamif(net::ssl_stream _value)
{
	return	make_basic_streamif<net::ssl_stream, char>(std::move(_value));
}


AMTRS_IO_NAMESPACE_END
#endif
#endif

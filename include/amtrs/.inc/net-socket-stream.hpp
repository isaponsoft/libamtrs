/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__net____socketsocket_stream__hpp
#define	__libamtrs__net____socketsocket_stream__hpp
AMTRS_IO_NAMESPACE_BEGIN

template<class Elm>
struct	streamif_traits<net::socket, Elm>
{
	using	char_type	= Elm;
	using	value_type	= net::socket;
	using	size_type	= std::streamsize;

	net::socket& socket()
	{
		return	*mSock;
	}

protected:
	streamif_traits(net::socket* _sock)
		: mSock(_sock)
	{}

	void close(value_type& _value)
	{
		_value.clear();
	}

	streamif_base::iostate read(value_type& _value, size_type& _readsize, char_type* _data, size_type _size)
	{
AMTRS_WARN_LOG("socket recv");
		auto	s	= _value.recv(_data, static_cast<int>(_size), 0);
AMTRS_WARN_LOG("socket recv %d", (int)s);
		if (s == 0)
		{
			return	streamif_base::eofbit;
		}
		if (s < 0)
		{
			int	err	= errno;
			return	(err == EAGAIN) || (err == EWOULDBLOCK)
					? streamif_base::againbit
					: streamif_base::badbit;
		}
		_readsize	= static_cast<size_type>(s);
		return	streamif_base::goodbit;
	}

	streamif_base::iostate write(value_type& _value, size_type& _readsize, char_type const* _data, size_type _size)
	{
		auto	s	= _value.send(_data, static_cast<int>(_size), 0);
		if (s == 0)
		{
			return	streamif_base::eofbit;
		}
		if (s < 0)
		{
			int	err	= errno;
			return	(err == EAGAIN) || (err == EWOULDBLOCK)
					? streamif_base::againbit
					: streamif_base::badbit;
		}
		_readsize	= static_cast<size_type>(s);
		return	streamif_base::goodbit;
	}

private:
	net::socket*	mSock;
};

using	socket_streamif	= basic_streamif<net::socket, char>;



inline auto make_streamif(net::socket _value)
{
	return	make_basic_streamif<net::socket, char>(std::move(_value));
}


AMTRS_IO_NAMESPACE_END
#endif

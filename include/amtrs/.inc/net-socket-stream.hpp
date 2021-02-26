/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__net____socketsocket_stream__hpp
#define	__libamtrs__net____socketsocket_stream__hpp
AMTRS_NET_NAMESPACE_BEGIN


template<class Sock>
class	basic_socket_stream;

using	socket_stream	= basic_socket_stream<socket>;


template<class Sock>
class	basic_socket_stream
{
public:
	using	socket_type		= typename Sock::socket_type;
	using	socklen_type	= typename Sock::socklen_type;
	using	iostate			= std::ios_base::iostate;

	using	char_type		= char;
	using	off_type		= typename std::basic_iostream<char_type>::off_type;
	using	pos_type		= typename std::basic_iostream<char_type>::pos_type;
	using	size_type		= size_t;


	static constexpr socket_type	nsock	= Sock::nsock;
	static constexpr iostate		goodbit	= std::ios_base::goodbit;
	static constexpr iostate		badbit	= std::ios_base::badbit;
	static constexpr iostate		failbit	= std::ios_base::failbit;
	static constexpr iostate		eofbit	= std::ios_base::eofbit;

	basic_socket_stream(socket_type _sock = nsock)
		: mSocket(_sock)
	{}





	basic_socket_stream(const basic_socket_stream&) = default;
	basic_socket_stream(basic_socket_stream&&) = default;
	basic_socket_stream& operator = (const basic_socket_stream&) = default;
	basic_socket_stream& operator = (basic_socket_stream&&) = default;


	basic_socket_stream& read(char* _buff, size_t _n, int _flag = 0)
	{
		if (good())
		{
			mGCount	= 0;
			auto	s	= ::recv(mSocket, _buff, static_cast<int>(_n), _flag);
			if (s > 0)
			{
				// 正常終了
				mGCount	= s;
			}
			else if (s == 0)
			{
				// EOFに到達した
				setstate(eofbit);
			}
			else if (s < 0)
			{
				// 何かしらのエラー
				int	err	= errno;
				if ((err == EAGAIN) || (err == EWOULDBLOCK))
				{
					// ノンブロックの時、まだ届いていない
					setstate(failbit);
				}
				else
				{
					// エラー
					setstate(badbit);
				}
			}
		}
		return	*this;
	}

	basic_socket_stream& write(char const* _buff, size_t _n, int _flag = 0)
	{
		if (good())
		{
			mGCount	= 0;
			auto	s	= ::send(mSocket, _buff, static_cast<int>(_n), _flag);
			if (s > 0)
			{
				// 正常終了
				mPCount	= s;
			}
			else if (s == 0)
			{
				// EOFに到達した
				setstate(eofbit);
			}
			else if (s < 0)
			{
				// 何かしらのエラー
				int	err	= errno;
				if ((err == EAGAIN) || (err == EWOULDBLOCK))
				{
					// ノンブロックの時、まだ届いていない
					setstate(failbit);
				}
				else
				{
					// エラー
					setstate(badbit);
				}
			}
		}
		return	*this;
	}

	std::size_t gcount() const noexcept { return mGCount; }
	std::size_t pcount() const noexcept { return mPCount; }

	explicit operator bool() const { return good(); }

	bool good() const noexcept { return rdstate() == goodbit; }
	bool eof()  const noexcept { return rdstate() & eofbit  ? true : false; }
	bool fail() const noexcept { return rdstate() & failbit ? true : false; }
	bool bad()  const noexcept { return rdstate() & badbit  ? true : false; }

	iostate rdstate() const noexcept { return mStatus; }
	void setstate(iostate _state) noexcept { clear(rdstate()|_state); }
	void clear(iostate _state = goodbit) noexcept { mStatus = _state; }




private:
	socket_type	mSocket	= nsock;
	iostate		mStatus	= goodbit;
	std::size_t	mGCount	= 0;
	std::size_t	mPCount	= 0;

	// unsupport
	pos_type tellg();
	basic_socket_stream& seekg(off_type _off, std::ios_base::seekdir _dir);
};

AMTRS_NET_NAMESPACE_END
#endif

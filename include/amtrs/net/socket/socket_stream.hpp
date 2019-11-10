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
#ifndef	__libamtrs__net____socketsocket_stream__hpp
#define	__libamtrs__net____socketsocket_stream__hpp
#include "socket.hpp"
AMTRS_NET_NAMESPACE_BEGIN


class	socket_stream
{
public:
	using	socket_type		= socket_traits::socket_type;
	using	socklen_type	= socket_traits::socklen_type;
	using	iostate			= std::ios_base::iostate;

	using	char_type		= char;
	using	off_type		= typename std::basic_iostream<char_type>::off_type;
	using	pos_type		= typename std::basic_iostream<char_type>::pos_type;
	using	size_type		= size_t;


	static constexpr socket_type	nsock	= socket_traits::nsock;
	static constexpr iostate		goodbit	= std::ios_base::goodbit;
	static constexpr iostate		badbit	= std::ios_base::badbit;
	static constexpr iostate		failbit	= std::ios_base::failbit;
	static constexpr iostate		eofbit	= std::ios_base::eofbit;

	socket_stream(socket_type _sock = nsock)
		: mSocket(_sock)
	{}





	socket_stream(const socket_stream&) = default;
	socket_stream(socket_stream&&) = default;
	socket_stream& operator = (const socket_stream&) = default;
	socket_stream& operator = (socket_stream&&) = default;


	socket_stream& read(char* _buff, size_t _n, int _flag = 0)
	{
		if (good())
		{
			mGCount	= 0;
			int	s	= ::recv(mSocket, _buff, _n, _flag);
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

	socket_stream& write(char const* _buff, size_t _n, int _flag = 0)
	{
		if (good())
		{
			mGCount	= 0;
			int	s	= ::send(mSocket, _buff, _n, _flag);
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
	socket_stream& seekg(off_type _off, std::ios_base::seekdir _dir);
};

AMTRS_NET_NAMESPACE_END
#endif

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
#ifndef	__libamtrs__compress__zlib_inflate_stream_in__hpp
#define	__libamtrs__compress__zlib_inflate_stream_in__hpp
#include <cstring>
#include <iosfwd>
#include "../../crypto/crc32.hpp"
#include "../../io/read.hpp"
#include "../../io/stream_in-fwd.hpp"

#if		__has_include(<zlib.h>)
#include <zlib.h>
#elif	__has_include(<zlib/zlib.h>)
#include <zlib/zlib.h>
#else
#error	"zlib.h" is not found
#endif
#

AMTRS_IO_NAMESPACE_BEGIN

template<class Stream>
struct	inflate_stream_in_traits
{

	void update_checksum(void const* _buf, size_t _len)
	{
		mCRC32(_buf, _len);
	}


	uint32_t checksum() const noexcept
	{
		return	mCRC32 ^ static_cast<uint32_t>(-1);
	}

private:
	 crypto::crc32	mCRC32;
};

template<class Stream, class StreamObj = Stream, class Traits = inflate_stream_in_traits<Stream>>
class	inflate_stream_in
		: public Traits
{
public:
	static constexpr size_t	buffer_size	= 1024;

	using	stream_type		= Stream;
	using	char_type		= typename stream_type::char_type;
	using	off_type		= typename stream_type::off_type;
	using	pos_type		= typename stream_type::pos_type;
	using	iostate			= typename stream_type::iostate;
	using	traits_type		= Traits;

	template<class T>
	inflate_stream_in(T&& _stream)
		: mStream(std::forward<T>(_stream))
	{
		std::memset(&mZ, 0, sizeof(mZ));
		::inflateInit2(&mZ, -MAX_WBITS);

		mInSize			= size(mStream);
		mZ.next_out		= (Bytef*)mOut;
		mZ.avail_out	= buffer_size;
		mStatus			= mStream.rdstate();
	}

	~inflate_stream_in()
	{
		inflateEnd(&mZ);
	}

	inflate_stream_in& read(char_type* _buff, size_t _n)
	{
		mGCount	= 0;
		if (good())
		{
			size_t		reqsize	= _n;
			char_type*	out		= _buff;
			for (bool exec = true; exec && reqsize;)
			{
				exec = false;


				// 入力バッファが空っぽになったら元のストリームから補充する
				if ((mZ.avail_in == 0) && (mInSize > 0))
				{
					mZ.next_in		=  (Bytef*)mIn;
					mZ.avail_in		=  io::read(mIn, mStream, std::min<size_t>(mInSize, buffer_size));
					if (!mStream)
					{
						mStatus		= mStream.rdstate();
					}
					mInSize			-= mZ.avail_in;
					if (mInSize == 0)
					{
						mFlush	= Z_FINISH;
					}

					exec		= true;
				}
				if (mZ.avail_in > 0 && mZ.avail_out > 0)
				{
					switch (inflate(&mZ, mFlush))
					{
						case Z_OK:
							exec		= true;
							break;
						case Z_BUF_ERROR :
							break;
						case Z_STREAM_END :
							break;
							case Z_NEED_DICT:    // 辞書が必要
							break;
						case Z_DATA_ERROR:   // 不正なデータ
							break;
						case Z_STREAM_ERROR:
							break;
					}
				}

				// out に蓄積されているデータを可能な限り転送する。
				auto	tank	= buffer_size - mZ.avail_out - mCursor;		// 転送待ちのサイズ
				if (tank > 0)
				{
					auto	ts	= std::min<size_t>(tank, reqsize);

					traits_type::update_checksum((char_type*)mOut + mCursor, ts);

					std::copy_n((char_type*)mOut + mCursor, ts, out);
					out			+= ts;
					mCursor		+= ts;
					mGCount		+= ts;
					reqsize		-= ts;
					exec		= true;
				}
				if (mCursor == buffer_size)
				{
					mZ.next_out		= (Bytef*)mOut;
					mZ.avail_out	= buffer_size;
					mCursor			= 0;
					exec			= true;
				}

				if (!exec)
				{
					setstate(std::ios::eofbit);
				}
			}
		}
		return	*this;
	}



	std::size_t gcount() const noexcept { return (std::size_t)mGCount; }

	explicit operator bool() const { return good(); }

	bool good() const noexcept { return rdstate() == std::ios::goodbit; }
	bool eof()  const noexcept { return rdstate() & std::ios::eofbit  ? true : false; }
	bool fail() const noexcept { return rdstate() & std::ios::failbit ? true : false; }
	bool bad()  const noexcept { return rdstate() & std::ios::badbit  ? true : false; }

	iostate rdstate() const noexcept { return mStatus; }
	void setstate(iostate _state) noexcept { clear(rdstate()|_state); }
	void clear(iostate _state = std::ios::goodbit) noexcept { mStatus = _state; }



private:
	z_stream		mZ;
	StreamObj		mStream;
	iostate			mStatus		= std::ios::goodbit;
	char			mIn[buffer_size];
	char			mOut[buffer_size];
	pos_type		mCursor	= 0;
	size_t			mGCount	= 0;
	size_t			mInSize;
	int				mFlush	= Z_SYNC_FLUSH;
};




template<class... T>
struct	io_traits<inflate_stream_in<T...>>
{
	using	io_type		= inflate_stream_in<T...>;
	using	char_type	= typename io_type::char_type;
	using	off_type	= typename io_type::off_type;
	using	size_type	= typename io_type::off_type;

	size_type read(void* _buffer, io_type& _io, size_type _request)
	{
		_io.read((char_type*)_buffer, _request);
		return	_io.gcount();
	}

	size_type size(io_type& _io)
	{
		return	_io.size();
	}
};


template<class IN>
auto make_inflate_stream_in(IN& _in)
{
	return	inflate_stream_in<IN, IN&>(_in);
}


template<class IN>
auto make_inflate_stream_in(IN&& _in)
{
	return	inflate_stream_in<IN, IN>(_in);
}


AMTRS_IO_NAMESPACE_END
#endif

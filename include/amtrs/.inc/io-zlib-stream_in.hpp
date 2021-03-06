/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__compress__zlib_inflate_stream_in__hpp
#define	__libamtrs__compress__zlib_inflate_stream_in__hpp
#include <cstring>
#include <iosfwd>
#include "../crypto.hpp"

#if		__has_include(<zlib.h>)
#include <zlib.h>
#elif	__has_include(<zlib/zlib.h>)
#include <zlib/zlib.h>
#else
#error	"zlib.h" is not found
#endif


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
					mZ.avail_in		=  (uInt)io::read(mIn, mStream, std::min<size_t>(mInSize, buffer_size));
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



template<class INDATA>
auto make_inflate_stream_in(INDATA& _iput)
{
	return	inflate_stream_in<INDATA, INDATA&>(_iput);
}


template<class INDATA>
auto make_inflate_stream_in(INDATA&& _in)
{
	return	inflate_stream_in<INDATA, INDATA>(_in);
}


AMTRS_IO_NAMESPACE_END
#endif

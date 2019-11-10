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
DISCLAIMED. In NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER In CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING In ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the libamtrs project.
******************************************************************************/
#ifndef	__libamtrs__io__limit__hpp
#define	__libamtrs__io__limit__hpp
#include "def.hpp"
#include "io_traits-fwd.hpp"
#include "stream_in-fwd.hpp"
AMTRS_IO_NAMESPACE_BEGIN

template<class...>
class	limit;



template<class T>
auto make_limit(T& _stream, size_t _limit) -> limit<T&>
{
	return	limit<T&>(_stream, _limit);
}



template<class T>
auto make_limit(T&& _stream, size_t _limit) -> limit<T>
{
	return	limit<T>(std::move(_stream), _limit);
}



template<class Stream>
class	limit<Stream&>
{
public:
	using	stream_type	= Stream;
	using	char_type	= typename stream_type::char_type;
	using	off_type	= typename stream_type::off_type;
	using	pos_type	= typename stream_type::pos_type;
	using	iostate		= typename stream_type::iostate;


	template<class In>
	limit(In&& _in, pos_type _limitsize)
		: mStream(std::forward<In>(_in))
		, mLimit(_limitsize)
	{
		mStart	= mStream.tellg();
	}



	limit& read(char_type* _buff, size_t _n)
	{
		if (good())
		{
			mStream.read(_buff, std::min<size_t>(_n, mLimit - mCursor));
			mCursor	+= mStream.gcount();
		}
		return	*this;
	}

	std::size_t gcount() const noexcept { return mStream.gcount(); }

	explicit operator bool() const { return mStream.good(); }

	bool good() const noexcept { return mStream.good(); }
	bool eof()  const noexcept { return mStream.eof();  }
	bool fail() const noexcept { return mStream.fail(); }
	bool bad()  const noexcept { return mStream.bad();  }

	auto rdstate() const noexcept { return mStream.rdstate(); }
	void setstate(iostate _state) noexcept { mStream.setstate(_state); }
	void clear(iostate _state = iostate::goodbit) noexcept { mStream.clear(_state); }


	pos_type tellg()
	{
		return	good()
				? mCursor
				: pos_type(-1);
	}

	limit& seekg(off_type _off, std::ios_base::seekdir _dir)
	{
		if (good())
		{
			switch (_dir)
			{
				case std::ios_base::beg:
				{
					mCursor	= _off;
					break;
				}

				case std::ios_base::cur:
				{
					mCursor	+= _off;
					break;
				}

				case std::ios_base::end:
				{
					mCursor	= mLimit - _off;
					break;
				}

				default:
				{}
			}
			if (mCursor > mLimit)
			{
				mCursor	= mLimit;
			}
			mStream.seekg(std::ios_base::beg, mStart + mCursor);
		}
		return	*this;
	}

	size_t size() const noexcept
	{
		return	mLimit;
	}


private:
	stream_type&	mStream;
	pos_type		mStart;
	pos_type		mCursor	= 0;
	pos_type		mLimit;
};



template<class Stream>
class	limit<Stream>
{
public:
	using	stream_type	= Stream;
	using	char_type	= typename stream_type::char_type;
	using	off_type	= typename stream_type::off_type;
	using	pos_type	= typename stream_type::pos_type;
	using	iostate		= typename stream_type::iostate;


	//! 読み込みサイズに制限を付けます。現在の読み取り位置からの相対位置になります。
	template<class In>
	limit(In&& _in, pos_type _limitsize)
		: mStream(std::forward<In>(_in))
		, mLimit(_limitsize)
	{
		mStart	= mStream.tellg();
	}


	limit& read(char_type* _buff, size_t _n)
	{
		if (good())
		{
			mStream.read(_buff, std::min(_n, mLimit - mCursor));
			mCursor	+= mStream.gcount();
		}
		return	*this;
	}

	std::size_t gcount() const noexcept { return mStream.gcount(); }

	explicit operator bool() const { return mStream.good(); }

	bool good() const noexcept { return mStream.good(); }
	bool eof()  const noexcept { return mStream.eof();  }
	bool fail() const noexcept { return mStream.fail(); }
	bool bad()  const noexcept { return mStream.bad();  }

	auto& rdstate() const noexcept { return mStream; }
	void setstate(iostate _state) noexcept { mStream.setstate(_state); }
	void clear(iostate _state = iostate::goodbit) noexcept { mStream.clear(_state); }


	pos_type tellg()
	{
		return	good()
				? mCursor
				: pos_type(-1);
	}

	limit& seekg(off_type _off, std::ios_base::seekdir _dir)
	{
		if (good())
		{
			switch (_dir)
			{
				case std::ios_base::beg:
				{
					mCursor	= _off;
					break;
				}

				case std::ios_base::cur:
				{
					mCursor	+= _off;
					break;
				}

				case std::ios_base::end:
				{
					mCursor	= mLimit - _off;
					break;
				}

				default:
				{}
			}
			if (mCursor > mLimit)
			{
				mCursor	= mLimit;
			}
			mStream.seekg(std::ios_base::beg, mStart + mCursor);
		}
		return	*this;
	}

	size_t size() const noexcept
	{
		return	mLimit;
	}


private:
	stream_type		mStream;
	pos_type		mStart;
	pos_type		mCursor	= 0;
	pos_type		mLimit;
};


template<class... T>
struct	io_traits<limit<T...>>
{
	using	stream_type	= limit<T...>;
	using	char_type	= typename stream_type::char_type;


	size_t read(void* _buff, stream_type& _io, size_t _size)
	{
		_io.read(reinterpret_cast<char_type*>(_buff), _size);
		auto	r	= _io.gcount();
		return	r;
	}


	size_t write(stream_type& _io, const void* _buff, size_t _size)
	{
		_io.write(reinterpret_cast<const char_type*>(_buff), _size);
		auto	r	= _io.gcount();
		return	r;
	}



	bool seekg(stream_type& _io, std::ios::off_type _off, std::ios::seekdir _dir)
	{
		_io.seekg(_off, _dir);
		return	true;
	}


	bool seekp(stream_type& _io, std::ios::off_type _off, std::ios::seekdir _dir)
	{
		_io.seekp(_off, _dir);
		return	true;
	}


	uintmax_t size(stream_type& _io)
	{
		return	_io.size();
	}
};



AMTRS_IO_NAMESPACE_END
#endif

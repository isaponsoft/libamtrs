/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__io__limit__hpp
#define	__libamtrs__io__limit__hpp
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
	using	pos_type	= typename stream_type::pos_type;
	using	iostate		= typename stream_type::iostate;


	template<class In>
	limit(In&& _in, pos_type _limitsize)
		: mStream(std::forward<In>(_in))
		, mLimit(_limitsize)
	{
		mStart	= tell(mStream);
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

	limit& seekg(pos_type _off, std::ios_base::seekdir _dir)
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
			mCursor	+= io::read(_buff, mStream, std::min(_n, mLimit - mCursor));
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

/*
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

*/

AMTRS_IO_NAMESPACE_END
#endif

﻿/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__io__stream_in_view__hpp
#define	__libamtrs__io__stream_in_view__hpp
AMTRS_IO_NAMESPACE_BEGIN



template<class T>
class	stream_in<view<T>>
{
public:
	using	value_type		= T;
	using	char_type		= T;
	using	pointer			= T*;
	using	const_pointer	= typename std::add_const<T>::type*;


	using	iostate			= std::ios_base::iostate;
	using	off_type		= size_t;
	using	pos_type		= size_t;
	static constexpr iostate	goodbit	= std::ios_base::goodbit;
	static constexpr iostate	badbit	= std::ios_base::badbit;
	static constexpr iostate	failbit	= std::ios_base::failbit;
	static constexpr iostate	eofbit	= std::ios_base::eofbit;

	stream_in(const stream_in&) = default;
	stream_in(stream_in&&) = default;
	stream_in& operator = (const stream_in&) = default;
	stream_in& operator = (stream_in&&) = default;

	template<class... Args>
	stream_in(Args&&... _args)
		: mView(std::forward<Args>(_args)...)
	{}

	stream_in& read(pointer _buff, size_t _n)
	{
		if (good())
		{
			auto	src		= mView.data() + mCursor;
			mGCount	= std::min(_n, mView.size() - mCursor);
			for (pos_type i = 0; i < mGCount; ++i)
			{
				_buff[i] = src[i];
			}
			mCursor += mGCount;
			if (mGCount < _n)
			{
				setstate(eofbit);
			}
		}
		return	*this;
	}

	std::size_t gcount() const noexcept { return mGCount; }

	explicit operator bool() const { return good(); }

	bool good() const noexcept { return rdstate() == goodbit; }
	bool eof()  const noexcept { return rdstate() & eofbit  ? true : false; }
	bool fail() const noexcept { return rdstate() & failbit ? true : false; }
	bool bad()  const noexcept { return rdstate() & badbit  ? true : false; }

	iostate rdstate() const noexcept { return mStatus; }
	void setstate(iostate _state) noexcept { clear(rdstate()|_state); }
	void clear(iostate _state = goodbit) noexcept { mStatus = _state; }


	pos_type tellg()
	{
		return	good()
				? mCursor
				: pos_type(-1);
	}

	stream_in& seekg(off_type _off, std::ios_base::seekdir _dir)
	{
		if (good())
		{
			size_t	limit	= mView.size();
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
					mCursor	= mView.size() - _off;
					break;
				}

				default:
				{}
			}
			if (mCursor > limit)
			{
				mCursor	= limit;
				setstate(failbit);
			}
		}
		return	*this;
	}

private:
	view<T>			mView;
	iostate			mStatus	= goodbit;
	pos_type		mCursor	= 0;
	std::size_t		mGCount	= 0;
};


template<class T>
struct	io_traits<stream_in<view<T>>>
{
	using	stream_type	= stream_in<view<T>>;
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
		_io.clear();
		seekg(_io, 0, std::ios::end);
		auto	endPos	= _io.tellg();
		_io.clear();
		seekg(_io, 0, std::ios::beg);
		auto	begPos	= _io.tellg();
		_io.clear();
		return	static_cast<uintmax_t>(endPos - begPos);
	}
};




template<class T>
bool seek(stream_in<view<T>>& _in, std::ios::off_type _off, std::ios::seekdir _dir)
{
	_in.seekg(_off, _dir);
	return	true;
}




template<class T>
uintmax_t size(stream_in<view<T>>& _in)
{
	_in.clear();
	seek(_in, 0, std::ios::end);
	auto	endPos	= _in.tellg();
	_in.clear();
	seek(_in, 0, std::ios::beg);
	auto	begPos	= _in.tellg();
	_in.clear();
	return	static_cast<uintmax_t>(endPos - begPos);
}




template<class T>
auto make_stream_in(view<T> _view) -> stream_in<view<T>>
{
	return	stream_in<view<T>>(_view);
}



AMTRS_IO_NAMESPACE_END
#endif

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
#ifndef	__libamtrs__iostream__rstream__hpp
#define	__libamtrs__iostream__rstream__hpp
#include <algorithm>
#include "def.hpp"
AMTRS_IOSTREAM_NAMESPACE_BEGIN


// ****************************************************************************
//! 指定する型が rstream に互換するか調べる。
// ----------------------------------------------------------------------------
//! rstream は std::istream と互換のある、read(), gcount(), seekg(), tellg() を
//! 持ったオブジェクトです。つまり、このユーティリティで std::istream を指定する
//! と true になります。
// ----------------------------------------------------------------------------
template<class T>
struct	is_rstream_compatible;



// ****************************************************************************
//! 様々なオブジェクトに対して、std::istream に近いインターフェースを提供する。
// ----------------------------------------------------------------------------
//! あくまでもインターフェースのエミュレートのみを行い、内部的にはほぼ何もしない。
//! そのため、コンパイラの最適化によってオーバーヘッドはほぼ存在しないはずである。
//! 利用できるのは read(), gcount(), seekg(), tellg(), ステータスの取得系のみ。
// ----------------------------------------------------------------------------
template<class...>
class	rstream;


template<class T>
class	rstream<T*>
{
	using	_		= typename std::enable_if<std::is_arithmetic<T>::value>::type;
public:
	using	pointer		= T*;
	using	iostate		= std::ios_base::iostate;
	using	off_type	= decltype(std::declval<pointer>() - std::declval<pointer>());
	using	pos_type	= off_type;
	using	char_type	= typename std::remove_cv<T>::type;
	static constexpr iostate	goodbit	= std::ios_base::goodbit;
	static constexpr iostate	badbit	= std::ios_base::badbit;
	static constexpr iostate	failbit	= std::ios_base::failbit;
	static constexpr iostate	eofbit	= std::ios_base::eofbit;

	rstream(pointer _begin, pointer _end)
		: mBegin(_begin)
		, mEnd(_end)
		, mCursor(_begin)
	{}

	rstream(pointer _begin, std::size_t _size)
		: rstream(_begin, _begin + _size)
	{}

	template<std::size_t N>
	rstream(T (&_buff)[N])
		: rstream(_buff, N)
	{}


	rstream(const rstream&) = default;
	rstream(rstream&&) = default;
	rstream& operator = (const rstream&) = default;
	rstream& operator = (rstream&&) = default;

	rstream& read(char_type* _buff, std::size_t _n)
	{
		if (good())
		{
			std::size_t	copySize	= capacity();
			if (copySize > _n)
			{
				copySize	= _n;
				setstate(eofbit);
			}
			std::copy_n(mCursor, copySize, _buff);
			mCursor += copySize;
			mGCount	=  copySize;
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
				? (mCursor - mBegin)
				: pos_type(-1);
	}

	rstream& seekg(off_type _off, std::ios_base::seekdir _dir)
	{
		if (good())
		{
			switch (_dir)
			{
				case std::ios_base::beg:
				{
					auto	next	= mBegin + _off;
					if (next < mBegin || next > mEnd)
					{
						setstate(failbit);
					}
					else
					{
						mCursor = next;
					}
					break;
				}

				case std::ios_base::cur:
				{
					auto	next	= mCursor + _off;
					if (next < mBegin || next > mEnd)
					{
						setstate(failbit);
					}
					else
					{
						mCursor = next;
					}
					break;
				}

				case std::ios_base::end:
				{
					auto	next	= mEnd - _off;
					if (next < mBegin || next > mEnd)
					{
						setstate(failbit);
					}
					else
					{
						mCursor = next;
					}
					break;
				}

				default:
				{
					break;
				}
			}
		}
		return	*this;
	}

protected:
	std::size_t capacity() const noexcept { return mEnd - mCursor; }

	pointer		mBegin;
	pointer		mEnd;
	pointer		mCursor;
	iostate		mStatus	= goodbit;
	std::size_t	mGCount	= 0;
};




template<class... StreamT>
class	rstream<std::basic_istream<StreamT...>>
{
public:
	using	_stream_type= std::basic_istream<StreamT...>;
	using	iostate		= std::ios_base::iostate;
	using	off_type	= typename _stream_type::off_type;
	using	pos_type	= typename _stream_type::pos_type;
	using	char_type	= typename _stream_type::char_type;
	static constexpr iostate	goodbit	= std::ios_base::goodbit;
	static constexpr iostate	badbit	= std::ios_base::badbit;
	static constexpr iostate	failbit	= std::ios_base::failbit;
	static constexpr iostate	eofbit	= std::ios_base::eofbit;

	rstream(_stream_type* _stream)
		: mStream(*_stream)
	{}

	rstream(const rstream&) = default;
	rstream(rstream&&) = default;
	rstream& operator = (const rstream&) = default;
	rstream& operator = (rstream&&) = default;

	rstream& read(char_type* _buff, std::size_t _n)
	{
		mStream.read(_buff, _n);
		return	*this;
	}

	std::size_t gcount() const noexcept { return (std::size_t)mStream.gcount(); }

	explicit operator bool() const { return (bool)mStream; }

	bool good() const noexcept { return mStream.good(); }
	bool eof()  const noexcept { return mStream.eof(); }
	bool fail() const noexcept { return mStream.fail(); }
	bool bad()  const noexcept { return mStream.bad(); }

	iostate rdstate() const noexcept { return mStream.rdstate(); }
	void setstate(iostate _state) noexcept { mStream.setstate(_state); }
	void clear(iostate _state = goodbit) noexcept { mStream.clear(_state); }


	pos_type tellg() { return mStream.tellg(); }
	rstream& seekg(off_type _off, std::ios_base::seekdir _dir)
	{
		mStream.seekg(_off, _dir);
		return	*this;
	}

protected:
	_stream_type&	mStream;
	iostate			mStatus	= goodbit;
	std::size_t		mGCount	= 0;
};


template<class... Types>
rstream<Types...>& make_rstream(rstream<Types...>& _io)
{
	return	_io;
}

template<class... Types>
rstream<Types...> make_rstream(rstream<Types...>&& _io)
{
	return	_io;
}


template<class CharT, class TraitsT>
rstream<std::basic_istream<CharT, TraitsT>> make_rstream(std::basic_istream<CharT, TraitsT>& _io)
{
	return	rstream<std::basic_istream<CharT, TraitsT>>(&_io);
}

template<class CharT, class TraitsT>
rstream<std::basic_istream<CharT, TraitsT>> make_rstream(std::basic_istream<CharT, TraitsT>* _io)
{
	return	rstream<std::basic_istream<CharT, TraitsT>>(*_io);
}


AMTRS_IMPLEMENTS_BEGIN(rstream)

template<class T>
struct	is_rstream_compatible
{
private:
	template<class TestT>
	static auto check(TestT&) noexcept
		-> decltype(
			std::declval<TestT&>().read(std::declval<typename TestT::char_type*>(), 1).gcount(),
			std::declval<TestT&>().seekg(std::declval<TestT&>().tellg(), std::ios::beg),
			std::true_type{}
		);

	static auto check(...) noexcept
		-> decltype(
			std::false_type{}
		);

public:	
	using	type	= decltype(check(std::declval<T&>()));
};

AMTRS_IMPLEMENTS_END(rstream)


template<class T>
struct	is_rstream_compatible
		: AMTRS_IMPLEMENTS(rstream)::is_rstream_compatible<T>::type
{};


AMTRS_IOSTREAM_NAMESPACE_END
#endif

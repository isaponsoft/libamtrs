#ifndef	__libamtrs__iostream__wstream__hpp
#define	__libamtrs__iostream__wstream__hpp
#include <iostream>
#include "def.hpp"
AMTRS_IOSTREAM_NAMESPACE_BEGIN

// ****************************************************************************
//! 様々なオブジェクトに対して、std::istream に近いインターフェースを提供する。
// ----------------------------------------------------------------------------
//! あくまでもインターフェースのエミュレートのみを行い、内部的にはほぼ何もしない。
//! そのため、コンパイラの最適化によってオーバーヘッドはほぼ存在しないはずである。
//! 利用できるのは read(), gcount(), seekg(), tellg(), ステータスの取得系のみ。
// ----------------------------------------------------------------------------
template<class...>
class	wstream;


template<class T>
class	wstream<T*>
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

	wstream(pointer _begin, pointer _end)
		: mBegin(_begin)
		, mEnd(_end)
		, mCursor(_begin)
	{}

	wstream(pointer _begin, std::size_t _size)
		: wstream(_begin, _begin + _size)
	{}

	template<std::size_t N>
	wstream(T (&_buff)[N])
		: wstream(_buff, N)
	{}


	wstream(const wstream&) = default;
	wstream(wstream&&) = default;
	wstream& operator = (const wstream&) = default;
	wstream& operator = (wstream&&) = default;

	wstream& write(const char_type* _buff, std::size_t _n)
	{
		if (good())
		{

			std::size_t	copySize	= capacity();
			if (copySize > _n)
			{
				copySize	= _n;
				setstate(badbit);
			}
			std::copy_n(mCursor, copySize, _buff);
			mCursor += copySize;
		}
		return	*this;
	}

	explicit operator bool() const { return good(); }

	bool good() const noexcept { return rdstate() == goodbit; }
	bool eof()  const noexcept { return rdstate() & eofbit  ? true : false; }
	bool fail() const noexcept { return rdstate() & failbit ? true : false; }
	bool bad()  const noexcept { return rdstate() & badbit  ? true : false; }

	iostate rdstate() const noexcept { return mStatus; }
	void setstate(iostate _state) noexcept { clear(rdstate()|_state); }
	void clear(iostate _state = goodbit) noexcept { mStatus = _state; }


	pos_type tellp()
	{
		return	good()
				? (mCursor - mBegin)
				: pos_type(-1);
	}

	wstream& seekp(off_type _off, std::ios_base::seekdir _dir)
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
};


template<class... StreamT>
class	wstream<std::basic_ostream<StreamT...>>
{
public:
	using	_stream_type= std::basic_ostream<StreamT...>;
	using	iostate		= std::ios_base::iostate;
	using	off_type	= typename _stream_type::off_type;
	using	pos_type	= typename _stream_type::pos_type;
	using	char_type	= typename _stream_type::char_type;
	static constexpr iostate	goodbit	= std::ios_base::goodbit;
	static constexpr iostate	badbit	= std::ios_base::badbit;
	static constexpr iostate	failbit	= std::ios_base::failbit;
	static constexpr iostate	eofbit	= std::ios_base::eofbit;

	wstream(_stream_type* _stream)
		: mStream(*_stream)
	{}

	wstream(const wstream&) = default;
	wstream(wstream&&) = default;
	wstream& operator = (const wstream&) = default;
	wstream& operator = (wstream&&) = default;

	wstream& write(const char_type* _buff, std::size_t _n)
	{
		mStream.write(_buff, _n);
		return	*this;
	}

	explicit operator bool() const { return mStream; }

	bool good() const noexcept { return mStream.good(); }
	bool eof()  const noexcept { return mStream.eof(); }
	bool fail() const noexcept { return mStream.fail(); }
	bool bad()  const noexcept { return mStream.bad(); }

	iostate rdstate() const noexcept { return mStream.rdstate(); }
	void setstate(iostate _state) noexcept { mStream.setstate(_state); }
	void clear(iostate _state = goodbit) noexcept { mStream.clear(_state); }


	pos_type tellp() { return mStream.tellp(); }
	wstream& seekp(off_type _off, std::ios_base::seekdir _dir)
	{
		mStream.seekp(_off, _dir);
		return	*this;
	}

protected:
	_stream_type&	mStream;
};




template<class... Types>
wstream<Types...>& make_wstream(wstream<Types...>& _io)
{
	return	_io;
}

template<class... Types>
wstream<Types...> make_wstream(wstream<Types...>&& _io)
{
	return	_io;
}


template<class CharT, class TraitsT>
wstream<std::basic_ostream<CharT, TraitsT>> make_wstream(std::basic_ostream<CharT, TraitsT>& _io)
{
	return	wstream<std::basic_ostream<CharT, TraitsT>>(&_io);
}

template<class CharT, class TraitsT>
wstream<std::basic_ostream<CharT, TraitsT>> make_wstream(std::basic_ostream<CharT, TraitsT>* _io)
{
	return	wstream<std::basic_ostream<CharT, TraitsT>>(*_io);
}

AMTRS_IOSTREAM_NAMESPACE_END
#endif

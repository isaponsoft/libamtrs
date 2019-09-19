#ifndef	__libamtrs__iostream__viewstream__hpp
#define	__libamtrs__iostream__viewstream__hpp
#include <iostream>
#include <type_traits>
#include "def.hpp"
AMTRS_IOSTREAM_NAMESPACE_BEGIN

// ============================================================================
//!	メモリ領域(view)に対する入力ストリームバッファを提供します。
// ----------------------------------------------------------------------------
//!	example)
//! 	viewstreambuf  iv(text.data(), text.size());
//!		std::istream   in(&iv);
//!		in >> str;
// ----------------------------------------------------------------------------
template<class CharT, class TraitsT = std::char_traits<typename std::remove_cv<CharT>::type>>
class	bacic_viewstreambuf;

using	viewstreambuf	= bacic_viewstreambuf<char>;
using	wviewstreambuf	= bacic_viewstreambuf<wchar_t>;

//! 
template<class CharT, class TraitsT>
class	bacic_viewstreambuf
		: public std::basic_streambuf<typename std::remove_cv<CharT>::type, TraitsT>
{
	using	_base_type	= std::basic_streambuf<typename std::remove_cv<CharT>::type, TraitsT>;
public:
	using	pointer		= CharT*;
	using	char_type	= typename _base_type::char_type;
	using	int_type	= typename _base_type::int_type;
	using	traits_type	= typename _base_type::traits_type;

	bacic_viewstreambuf(pointer _begin, pointer _end)
		: mBegin(_begin)
		, mEnd(_end)
		, mInput(_begin)
	{}

	bacic_viewstreambuf(pointer _begin, std::size_t _size)
		: bacic_viewstreambuf(_begin, _begin + _size)
	{}

protected:
	virtual std::streampos seekoff(std::streamoff _off, std::ios_base::seekdir _way, std::ios_base::openmode _which = std::ios_base::in | std::ios_base::out) override
	{
		auto			input	= mInput;
		std::streampos	retval	= std::streampos(-1);
		if (_which & std::ios_base::in)
		{
			retval	= _seekoff(mInput, _off, _way);
			if (retval == std::streampos(-1))
			{
				return	std::streampos(-1);
			}
		}
		if (_which & std::ios_base::out)
		{
			retval	= _seekoff(mOutput, _off, _way);
			if (retval == std::streampos(-1))
			{
				mInput	= input;
				return	std::streampos(-1);
			}
		}
		return	retval;
	}

	// ostream
	virtual int_type overflow(int_type _ch = traits_type::eof()) override
	{
		if constexpr (std::is_const<typename std::remove_pointer<pointer>::type>::value)
		{
			return	traits_type::eof();
		}
		else
		{
			return	(mInput == mEnd)
					? traits_type::eof()
					: traits_type::to_int_type(*mOutput++ = _ch);
		}
	}

	// istream
	virtual int_type uflow() override
	{
		return	(mInput == mEnd)
				? traits_type::eof()
				: traits_type::to_int_type(*mInput++);
	}

	virtual int_type pbackfail(int_type _ch) override
	{
		return	(mInput == mBegin || (_ch != traits_type::eof() && _ch != mInput[-1]))
				? traits_type::eof()
				: traits_type::to_int_type(*--mInput);
	}

	virtual std::streamsize showmanyc() override
	{
		return	mEnd - mInput;
	}

private:
	std::streampos _seekoff(pointer& _cursor, std::streamoff _off, std::ios_base::seekdir _way)
	{
		switch (_way)
		{
			case std::ios_base::beg :
			{
				auto	pos	= mBegin + _off;
				if (pos > mEnd)
				{
					return	std::streampos(-1);
				}
				_cursor = pos;
				return	std::streampos(_cursor - mBegin);
			}

			case std::ios_base::cur :
			{
				auto	pos	= _cursor + _off;
				if (pos > mEnd)
				{
					return	std::streampos(-1);
				}
				_cursor = pos;
				return	std::streampos(_cursor - mBegin);
			}

			case std::ios_base::end :
			{
				auto	pos	= mEnd - _off;
				if (pos > mEnd)
				{
					return	std::streampos(-1);
				}
				_cursor = pos;
				return	std::streampos(_cursor - mBegin);
			}

			default :
			{
				return	std::streampos(-1);
			}
		}
	}

	pointer		mBegin;
	pointer		mEnd;
	pointer		mInput;
	pointer		mOutput;
};



AMTRS_IOSTREAM_NAMESPACE_END
#endif

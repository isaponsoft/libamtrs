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
#ifndef	__libamtrs__iostream__viewstream__hpp
#define	__libamtrs__iostream__viewstream__hpp
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

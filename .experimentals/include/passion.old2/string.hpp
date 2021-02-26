/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__passion__string__hpp
#define	__libamtrs__passion__string__hpp
#include <iosfwd>
#include <amtrs/amtrs.hpp>
#include "def.hpp"
PASSION_NAMESPACE_BEGIN


// ****************************************************************************
//! 共有文字列
// ----------------------------------------------------------------------------
template<class Char, class Traits = std::char_traits<Char>>
class	basic_vmstring
{
public:
	using	value_type		= Char;
	using	char_type		= Char;
	using	traits_type		= Traits;
	using	size_type		= size_t;
	using	view_type		= std::basic_string_view<Char, Traits>;
	using	pointer			= value_type const*;
	using	const_pointer	= value_type const*;

	basic_vmstring() = default;
	basic_vmstring(basic_vmstring const&) = default;
	basic_vmstring(basic_vmstring&&) = default;
	basic_vmstring& operator = (basic_vmstring const&) = default;
	basic_vmstring& operator = (basic_vmstring&&) = default;

	      pointer begin() noexcept { return data(); }
	      pointer end() noexcept { return data() + size(); }
	const_pointer begin() const noexcept { return data(); }
	const_pointer end() const noexcept { return data() + size(); }
	const_pointer data() const noexcept { static char_type const v[1] = {0}; return empty() ? v : reinterpret_cast<const_pointer>(mText.get() + 1); }
	const_pointer c_str() const noexcept { return data(); }
	bool empty() const noexcept { return mText.empty(); }
	size_type size() const noexcept { return empty() ? 0 : mText->length; }
	operator view_type () const noexcept { return view(); }
	bool operator == (basic_vmstring const& _r) const noexcept { return	(view_type)*this == (view_type)_r; }
	bool operator != (basic_vmstring const& _r) const noexcept { return	(view_type)*this != (view_type)_r; }

	basic_vmstring operator + (basic_vmstring const& _r) const
	{
		basic_vmstring	retval;
		retval.resize(size() + _r.size());
		value_type*	d	= const_cast<value_type*>(retval.data());
		if (size())
		{
			std::memcpy(d, data(), size());
			d += size();
		}
		if (_r.size())
		{
			std::memcpy(d, _r.data(), _r.size());
			d += _r.size();
		}
		*d	= 0;
		return	retval;
	}

	basic_vmstring& operator += (basic_vmstring const& _r)
	{
		*this	= *this + _r;
		return	*this;
	}


	basic_vmstring(const_pointer _data, size_type _size)
	{
		if (_size > 0)
		{
			resize(_size);
			value_type*	d	= const_cast<value_type*>(data());
			std::memcpy(d, _data, _size);
			d[_size]	= 0;
		}
	}

	basic_vmstring(view_type _s)
		: basic_vmstring(_s.data(), _s.size())
	{}

	template<size_t L>
	basic_vmstring(value_type const (&_s)[L])
		: basic_vmstring(_s, _s[L] ? L : (L-1))
	{}

	template<class S>
	basic_vmstring(S const& _s)
		: basic_vmstring(_s.data(), _s.size())
	{}

	view_type view() const noexcept { return view_type(data(), size()); }

	void resize(size_t _size)
	{
		if (_size > 0)
		{
			void*	buff	= new int8_t[sizeof(text) + (sizeof(value_type) * _size) + sizeof(value_type)];
			mText			= new (buff) text();
			mText->length	= _size;
			value_type*	d	= const_cast<value_type*>(data());
			d[_size]	= 0;
		}
		else
		{
			mText.clear();
		}
	}

protected:
	struct	text : amtrs::ref_object
	{
		size_type	length;
	};
	mutable amtrs::ref<text>	mText;
};


using	vmstring	= basic_vmstring<char>;



PASSION_NAMESPACE_END
namespace std {

template<class CharT, class Traits = std::char_traits<CharT>>
basic_ostream<CharT, Traits>& operator << (basic_ostream<CharT, Traits>& _io, PASSION_NAMESPACE::basic_vmstring<CharT, Traits> const& _s)
{
	_io << _s.view();
	return	_io;
}

template<class CharT, class Traits>
struct	hash<PASSION_NAMESPACE::basic_vmstring<CharT, Traits>>
{
	size_t operator () (PASSION_NAMESPACE::basic_vmstring<CharT, Traits> const& _s) const noexcept
	{
		size_t	h	= 0x7c1341fc;
		for (auto c : _s)
		{
			h	= h * 31 + c - 0x21;
		}
		return	h;
	}
};

}
#endif

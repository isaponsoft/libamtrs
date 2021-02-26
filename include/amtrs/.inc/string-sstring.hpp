/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__string__sstring__hpp
#define	__libamtrs__string__sstring__hpp
#include "../memory/shared_buffer.hpp"
AMTRS_NAMESPACE_BEGIN
struct	shared_string_header
{
	size_t	hash;
};



//! 共有文字列
//! 文字列を格納するメモリを共有します。一度作られた文字列を変更することはできません。
//! コピーや比較が多いプログラムではstd::stringよりも効率的に使用できる事があります。
template<class CharT, class  Traits = std::char_traits<CharT>, class Alloc = std::allocator<CharT>>
class	basic_sstring
		: public shared_buffer<CharT, shared_string_header, Alloc>
{
public:
	using	super_type				= shared_buffer<CharT, shared_string_header, Alloc>;
	using	traits_type				= Traits;
	using	value_type				= CharT;
	using	allocator_type			= Alloc;

	using	size_type				= size_t;
	using	difference_type			= intptr_t;

	using	reference				= CharT&;
	using	const_reference			= typename std::add_const<CharT>::type&;
	using	pointer					= CharT*;
	using	const_pointer			= typename std::add_const<CharT>::type*;

	using	const_iterator			= typename super_type::const_iterator;
	using	const_reverse_iterator	= typename super_type::const_reverse_iterator;
	using	iterator				= const_iterator;
	using	reverse_iterator		= const_reverse_iterator;

	using	hash_type				= size_t;
	using	view_type				= std::basic_string_view<CharT, Traits>;
	
	static constexpr size_type		npos = view_type::npos;

	basic_sstring() = default;
	basic_sstring(basic_sstring const&) = default;
	basic_sstring(basic_sstring&&) = default;
	basic_sstring& operator = (basic_sstring const&) = default;
	basic_sstring& operator = (basic_sstring&&) = default;

	template<class It>
	basic_sstring(It _first, It _last)
	{
		resize(std::distance(_first, _last));
		std::copy(_first, _last, super_type::data());
		super_type::data()[size()] = 0;
		super_type::header().hash	= compute_hash((view_type)*this);
	}

	basic_sstring(value_type const* _first)
	{
		view_type	v(_first);
		if (v.size())
		{
			resize(v.size());
			std::copy_n(v.data(), v.size(), super_type::data());
			super_type::data()[v.size()] = 0;
			super_type::header().hash	= compute_hash((view_type)*this);
		}
	}

	basic_sstring(value_type const* _first, size_type _size)
	{
		if (_first && _size != 0)
		{
			resize(_size);
			std::copy_n(_first, _size, super_type::data());
			super_type::data()[_size] = 0;
			super_type::header().hash	= compute_hash((view_type)*this);
		}
	}


	template<class R>
	basic_sstring(R _rhs)
	{
		view_type	v(_rhs);
		if (v.size() > 0)
		{
			super_type::operator = (super_type(v.size(), shared_string_header()));
			std::copy(v.begin(), v.end(), super_type::data());
			super_type::data()[size()] = 0;
			super_type::header().hash	= compute_hash((view_type)*this);
		}
	}


	const_pointer c_str() const noexcept { return data(); }
	      pointer data() noexcept { return super_type::empty() ? nullbuff() : super_type::data(); }
	const_pointer data() const noexcept { return super_type::empty() ? nullbuff() : super_type::data(); }
	size_type size() const noexcept { return super_type::size(); }
	bool empty() const noexcept { return size() == 0; }

	explicit operator view_type () const noexcept { return view_type(data(), size()); }

	const_iterator begin() const noexcept { return const_iterator(data()); }
	const_iterator cbegin() const noexcept { return const_iterator(data()); }
	const_iterator end() const noexcept { return const_iterator(data() + size()); }
	const_iterator cend() const noexcept { return const_iterator(data() + size()); }

	const_reverse_iterator rbegin() const noexcept { return const_iterator(data() + size() - 1); }
	const_reverse_iterator rcbegin() const noexcept { return const_iterator(data() + size() - 1); }
	const_reverse_iterator rend() const noexcept { return const_iterator(data() - 1); }
	const_reverse_iterator rcend() const noexcept { return const_iterator(data() - 1); }

	hash_type hash() const noexcept { return empty() ? 0 : super_type::header().hash; }

	template<class V> bool operator >  (V _v) const noexcept { return view_type(*this) >  view_type(_v); }
	template<class V> bool operator >= (V _v) const noexcept { return view_type(*this) >= view_type(_v); }
	template<class V> bool operator <  (V _v) const noexcept { return view_type(*this) <  view_type(_v); }
	template<class V> bool operator <= (V _v) const noexcept { return view_type(*this) <= view_type(_v); }


	template<class V> basic_sstring operator + (V const& _value) const
	{
		auto	val	= view_type(_value);
		basic_sstring	t;
		t.resize(size() + val.size());
		std::copy_n(data(), size(), t.data());
		std::copy_n(val.data(), val.size(), t.data() + size());
		return	t;
	}

	template<class V> basic_sstring& operator += (V const& _value)
	{
		*this	= operator + (_value);
		return	*this;
	}

	bool operator == (basic_sstring const& _rhs) const noexcept
	{
		return	super_type::operator == (_rhs) || ((hash() == _rhs.hash()) && ((view_type)*this == (view_type)_rhs));
	}

	bool operator != (basic_sstring const& _rhs) const noexcept
	{
		return	(hash() != _rhs.hash()) || ((view_type)*this != (view_type)_rhs);
	}

	bool operator == (view_type _r) const noexcept { return size() == _r.size() ? (!std::memcmp(data(), _r.data(), _r.size())) : false; }
	bool operator != (view_type _r) const noexcept { return !(*this == _r); }

	template<class V> bool operator == (V _r) const noexcept { return operator == (view_type(_r)); }
	template<class V> bool operator != (V _r) const noexcept { return operator != (view_type(_r)); }



	const_reference operator [] (size_type _index) const noexcept { return data()[_index]; }
	const_reference front() const noexcept { return *begin(); }
	const_reference back() const noexcept { return *rbegin(); }

	int compare(basic_sstring const& _s) const noexcept { return ((view_type)*this).compare((view_type)_s); }
	int compare(size_type _pos1, size_type _count1, const basic_sstring& _s) const noexcept { return ((view_type)*this).compare(_pos1, _count1, (view_type)_s); }
	int compare(size_type _pos1, size_type _count1, const basic_sstring& _s, size_type _pos2, size_type _count2 = npos) const noexcept { return ((view_type)*this).compare(_pos1, _count1, (view_type)_s, _pos2, _count2); }
	int compare(size_type _pos1, size_type _count1, value_type const* _s) const { return ((view_type)*this).compare(_pos1, _count1, _s); }
	int compare(size_type _pos1, size_type _count1, value_type const* _s, size_type _count2) const { return ((view_type)*this).compare(_pos1, _count1, _s, _count2); }

	basic_sstring substr(size_type _pos = 0, size_type _count = npos) const
	{
		return	basic_sstring(view_type(*this).substr(_pos, _count));
	}


	void resize(typename super_type::size_type _size)
	{
		*static_cast<super_type*>(this) = super_type(_size);
	}

	size_type find(const value_type* _c, size_type _pos, size_type _n) const;
	size_type find(const value_type* _c, size_type _pos = 0) const;
	size_type find(value_type _c, size_type _pos = 0) const { return std::string_view(*this).find_first_of(_c, _pos); }


	size_type rfind(std::string_view s, size_type pos = 0) const;
	size_type rfind(const value_type* _s, size_type _pos = 0) const { return rfind(std::string_view(_s), _pos); }


	size_type find_first_of(const value_type* s,size_type pos = 0) const;
	size_type find_last_of(value_type c, size_type pos = npos) const noexcept { return ((std::string_view)*this).find_last_of(c, pos); }

	basic_sstring& replace(size_type _pos, size_type _size, const value_type* _replacement, std::size_t _replacement_size); 
	basic_sstring& replace(size_type _pos, size_type _size, std::string_view _replacement) { return replace(_pos, _size, _replacement.data(), _replacement.size()); } 


protected:
	static value_type* nullbuff() noexcept
	{
		static value_type	b[1] = {0};
		return	b;
	}

	static hash_type compute_hash(view_type _view)
	{
		return	std::hash<view_type>{}(_view);
	}
};

/*
template<class CharT, class Traits, class Alloc>
inline auto operator + (std::string_view _l, const basic_sstring<CharT, Traits, Alloc>& _r) -> basic_sstring<CharT, Traits, Alloc>
{
	return	basic_sstring<CharT, Traits, Alloc>(_l + *static_cast<const string*>(&_r));
}

template<class CharT, class Traits, class Alloc>
inline auto operator + (const char* _l, const basic_sstring<CharT, Traits, Alloc>& _r) -> basic_sstring<CharT, Traits, Alloc>
{
	return	T(_l + *static_cast<const string*>(&_r));
}

template<class CharT, class Traits, class Alloc>
inline auto operator + (const std::basic_string<char, std::char_traits<char>, Allocator>& _l, const basic_sstring<CharT, Traits, Alloc>& _r) -> basic_sstring<CharT, Traits, Alloc>
{
	return	T(_l + static_cast<const string*>(&_r));
}
*/


template<class CharT, class Traits, class Alloc>
inline auto basic_sstring<CharT, Traits, Alloc>::find_first_of(const value_type* _s, size_type _pos) const -> size_type
{
	if (_pos <= size())
	{
		auto	sz	= traits_type::length(_s);
		if ((_pos + sz) <= size())
		{
			auto	lastPos	= size() - sz;
			for (auto pos = _pos; pos != lastPos; ++pos)
			{
				if (!std::memcmp(data() + pos, _s, sz))
				{
					return	pos;
				}
			}
		}
	}
	return	npos;
}



template<class CharT, class Traits, class Alloc>
inline auto basic_sstring<CharT, Traits, Alloc>::find(const value_type* _s, size_type _pos) const -> size_type
{
	auto	len		= traits_type::length(_s);
	auto	s		= size();
	if (s >= len)
	{
		auto	lastPos	= s - len;
		auto	pos		= _pos;
		while (pos <= lastPos)
		{
			if (!std::memcmp(data() + pos, _s, len))
			{
				return	pos;
			}
			++pos;
		}
	}
	return	npos;
}


template<class CharT, class Traits, class Alloc>
inline auto basic_sstring<CharT, Traits, Alloc>::rfind(std::string_view _s, size_type _pos) const -> size_type
{
	auto	len		= _s.size();
	if (len > size())
	{
		return	npos;
	}
	auto	lastPos	= size() - len;
	auto	pos		= lastPos;
	for (;;)
	{
		if (!std::memcmp(data() + pos, _s.data(), len))
		{
			return	pos;
		}
		if (pos == _pos)
		{
			return	npos;
		}
		--pos;
	}
}


template<class CharT, class Traits, class Alloc>
inline auto basic_sstring<CharT, Traits, Alloc>::replace(size_type _pos, size_type _size, const value_type* _replacement, std::size_t _replacement_size) -> basic_sstring<CharT, Traits, Alloc>&
{
	basic_sstring	t(data(), size());
	*this	= t.replace(_pos, _size, _replacement, _replacement_size);
	return	*this;
}

using	shared_string	= basic_sstring<char>;


AMTRS_NAMESPACE_END
namespace std {

template<class CharT, class Traits, class Traits2, class Alloc>
basic_ostream<CharT, Traits>& operator << (basic_ostream<CharT, Traits>& _os, const AMTRS_NAMESPACE::basic_sstring<CharT, Traits2, Alloc>& _s)
{
	return	_os << _s.c_str();
}


template<class CharT, class  Traits, class Alloc>
struct	hash<amtrs::basic_sstring<CharT, Traits, Alloc>>
{
	auto operator () (amtrs::basic_sstring<CharT, Traits, Alloc> const& _s) const noexcept
	{
		return	_s.hash();
	}
};

}
#endif

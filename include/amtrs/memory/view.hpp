/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__memory__view__hpp
#define	__libamtrs__memory__view__hpp
#include <iosfwd>
#include <string_view>
#include <type_traits>
#include "../amtrs.hpp"
#include "../crypto/hash-csx.hpp"
#include "../io/fwd.hpp"
#include "../typeutil/eval_test.hpp"
AMTRS_NAMESPACE_BEGIN
template<class...>
struct	view_traits;
AMTRS_IMPLEMENTS_BEGIN(view)
	template<class It> struct has_datasizable {
		template<class T> static constexpr auto test(T& o) -> decltype(std::declval<It&>() == (o.data() + o.size()));
	};

	template<class It> struct has_begin_end {
		template<class T> static constexpr auto test(T& o) -> decltype((std::declval<It&>() == o.begin()) && (std::declval<It&>() == o.end()));
	};

	template<class R, class It>	static constexpr bool	is_datasizable_v	= eval_test<R, has_datasizable<It>>::value;
	template<class R, class It>	static constexpr bool	is_begin_end_v		= eval_test<R, has_begin_end<It>>::value && !eval_test<R, has_datasizable<It>>::value;
	template<class R, class It>	static constexpr bool	is_pointable_v		= (std::is_pointer<R>::value && std::is_convertible<R, It>::value && !is_begin_end_v<R, It>);
AMTRS_IMPLEMENTS_END(view)


template<class T, class Iterator = T*, class Traits = view_traits<Iterator>>
struct	view
{
	using	traits_type				= Traits;
	using	value_type				= typename traits_type::value_type;
	using	const_value_type		= typename traits_type::const_value_type;
	using	size_type				= typename traits_type::size_type;
	using	reference				= typename traits_type::reference;
	using	const_reference			= typename traits_type::const_reference;
	using	iterator				= typename traits_type::iterator;
	using	const_iterator			= typename traits_type::const_iterator;

	static constexpr size_type	npos	= static_cast<size_type>(~0);

	iterator		s		= traits_type::init();
	iterator		e		= traits_type::init();

private:

public:
	constexpr view() = default;
	constexpr view(view const&) = default;
	constexpr view(view&&) = default;
	constexpr view& operator = (view const&) = default;
	constexpr view& operator = (view&&) = default;
	constexpr view(std::nullptr_t) noexcept : s(nullptr), e(nullptr) {}

	constexpr view(iterator _s) noexcept : s(_s), e(tail(_s)) {}
	constexpr view(iterator _s, iterator _e) noexcept : s(_s), e(_e) {}
	constexpr view(iterator _s, size_type _n) noexcept : s(_s), e(_s + _n) {}

	template<typename  R>
	constexpr view(R const& _src, std::enable_if_t<AMTRS_IMPLEMENTS(view)::is_datasizable_v<R, iterator>>** _= nullptr) noexcept
		: s(_src.data())
		, e(_src.data() + _src.size())
	{}

	template<typename  R>
	constexpr view(R const& _src, std::enable_if_t<AMTRS_IMPLEMENTS(view)::is_begin_end_v<R, iterator>>** _= nullptr) noexcept
		: s(_src.begin())
		, e(_src.end())
	{}

	static auto eof() noexcept -> const_reference { static value_type c = traits_type::eof(); return c; }

	constexpr auto data() const noexcept -> iterator { return s; }
	constexpr auto back() const noexcept -> reference { return e[-1]; }
	constexpr auto back_at() const noexcept -> const_reference { return empty() ? eof() : e[-1]; }
	constexpr auto begin() const noexcept -> iterator { return s; }
	constexpr auto end() const noexcept -> iterator { return e; }
	constexpr auto front() const noexcept -> reference { return *s; }
	constexpr auto front_at() const noexcept -> const_reference { return empty() ? eof() : *s; }
	constexpr auto size() const noexcept -> size_type { return e - s; }

	constexpr bool empty() const noexcept { return s == e || bad(); }				// 異常終了しているか終端まで読み込んでいるか調べます
	constexpr bool endof() const noexcept { return s == e && !bad(); }			// 正常に終わっている状態で終端まで読み込みが終わっているか調べます
	constexpr bool good() const noexcept { return !bad(); }						// 正常終了しているか調べます
	constexpr bool bad() const noexcept { if constexpr (std::is_pointer<iterator>::value) { return !s || !e; } else { return false; } }						// 異常終了しているか調べます

	constexpr auto operator * () noexcept -> reference { return front(); }
	constexpr auto operator * () const noexcept -> const_reference { return front_at(); }

	constexpr reference operator [] (int _r) noexcept { return data()[_r]; }
	constexpr value_type const& operator [] (int _r) const noexcept { return data()[_r]; }

	// **************************************************************
	// check

	constexpr bool in_bounds(iterator _it) const noexcept { return _it >= s && _it < e; }
	constexpr bool out_bounds(iterator _it) const noexcept { return !in_bounds(_it); }

	constexpr auto hash() const noexcept -> size_type;

	// **************************************************************
	// ccompare

	// 文字列を比較します。
	constexpr auto compare(view _r) const noexcept -> int;

	// 指定した文字列で始まっているかどうか調べます
	constexpr bool starts_with(value_type const* _text) const noexcept;
	constexpr bool starts_with(view _text) const noexcept;

	// 指定した文字列で終わっているかどうか調べます
	constexpr bool ends_with(value_type const* _text) const noexcept;
	constexpr bool ends_with(view _text) const noexcept;

	// 二つの文字列を比較し、一致する部分を返します。
	constexpr auto match_first_of(value_type const* _text) const noexcept -> view;
	constexpr auto match_first_of(view _text) const noexcept -> view;


	// **************************************************************
	// find

	// 指定した文字列が最初に出てくる場所を返します。
	constexpr auto find(view _s) const noexcept -> size_type;

	// 指定した文字列が最後に出てくる場所を返します。
	constexpr auto rfind(view _s) const noexcept -> size_type;


	// 指定した文字が最初に出てくる場所を返します。
	constexpr auto find_first_of(value_type _c) const noexcept -> size_type;

	// 指定した文字の一覧の中から最初に出てくる場所を返します。
	constexpr auto find_first_of(value_type const* _c) const noexcept -> size_type;
	constexpr auto find_first_of(view _c) const noexcept -> size_type;

	// 指定した文字以外の文字が最初に出てくる場所を返します。
	constexpr auto find_first_not_of(value_type _c) const noexcept -> size_type;

	// 指定した文字の一覧の中以外の文字が最初に出てくる場所を返します。
	constexpr auto find_first_not_of(value_type const* _c) const noexcept -> size_type;
	constexpr auto find_first_not_of(view _c) const noexcept -> size_type;

	// 指定した文字が最後に出てくる場所を返します。
	constexpr auto find_last_of(value_type _c) const noexcept -> size_type;

	// 指定した文字の一覧の中から最後に出てくる場所を返します。
	constexpr auto find_last_of(value_type const* _c) const noexcept -> size_type;
	constexpr auto find_last_of(view _c) const noexcept -> size_type;

	// 指定した文字以外の文字が最後に出てくる場所を返します。
	constexpr auto find_last_not_of(value_type _c) const noexcept -> size_type;

	// 指定した文字の一覧の中以外の文字が最後に出てくる場所を返します。
	constexpr auto find_last_not_of(value_type const* _c) const noexcept -> size_type;
	constexpr auto find_last_not_of(view _c) const noexcept -> size_type;


	// **************************************************************
	// subview

	// 改行が出るまでを抜き出します。
	constexpr auto read_line() const noexcept -> view;

	// 指定した位置から後ろを返します。
	template<class F>
	constexpr auto substr(F _first) const noexcept -> view;

	// 指定した位置から指定したサイズ（または終端位置）を返します。
	template<class F, class L>
	constexpr auto substr(F _first, L _last) const noexcept -> view;

	// 指定する文字を前方、後方から取り除きます
	constexpr auto trim(value_type _c) const noexcept -> view { return trim_first(_c).trim_last(_c); }

	// 指定する文字の一覧を前方、後方から取り除きます
	constexpr auto trim(value_type const* _list) const noexcept -> view { return trim(view(_list)); }
	constexpr auto trim(view _list) const noexcept -> view { return trim_first(_list).trim_last(_list); }

	// 指定する文字を前方から取り除きます
	constexpr auto trim_first(value_type _c) const noexcept -> view;

	// 指定する文字の一覧を前方から取り除きます
	constexpr auto trim_first(value_type const* _list) const noexcept -> view;
	constexpr auto trim_first(view _list) const noexcept -> view;

	// 指定する文字を後方から取り除きます
	constexpr auto trim_last(value_type _c) const noexcept -> view;

	// 指定する文字の一覧を後方から取り除きます
	constexpr auto trim_last(value_type const* _list) const noexcept -> view;
	constexpr auto trim_last(view _list) const noexcept -> view;

	// 前方からひとつだけ改行を取り除きます
	constexpr auto trim_first_one_return() const noexcept -> view;


	constexpr bool operator == (std::nullptr_t) const noexcept { return !s || !e; }
	constexpr bool operator != (std::nullptr_t) const noexcept { return s && e; }
	constexpr bool operator == (value_type const* _txt) const noexcept { if (empty()) { return _txt == nullptr || *_txt == '\0' ? true : false; } auto cur = s; while (*_txt) { if (*_txt++ != *cur++) { return false; } } return cur == e; }
	constexpr bool operator != (value_type const* _txt) const noexcept { return !operator == (_txt); }
	constexpr bool operator == (view _r) const noexcept { if (size() != _r.size()) { return false; } auto s1 = s, s2 = _r.s; while (s1 != e) { if (*s1++ != *s2++) { return false; } } return true; }
	constexpr bool operator != (view _r) const noexcept { return !operator == (_r); }
	constexpr bool operator <  (view _r) const noexcept { return compare(_r) <  0; }
	constexpr bool operator <= (view _r) const noexcept { return compare(_r) <= 0; }
	constexpr bool operator >  (view _r) const noexcept { return compare(_r) >  0; }
	constexpr bool operator >= (view _r) const noexcept { return compare(_r) >= 0; }

	// 読み取り開始位置を１文字進めます。終端位置は考慮しません。
	constexpr view& operator ++ () noexcept { ++s; return *this; }	
	constexpr view operator ++ (int) noexcept { view r(*this); ++s; return r; }	

	template<class CTraits>
	friend std::basic_ostream<typename std::remove_const<value_type>::type, CTraits>& operator << (std::basic_ostream<typename std::remove_const<value_type>::type, CTraits>& o, view<T, Iterator, Traits> const& thiz)
	{
		o.write(thiz.data(), thiz.size());
		return	o;
	}

private:
	constexpr iterator pos(value_type* _pos) const noexcept { return _pos < e ? _pos : e; }
	constexpr iterator pos(size_type   _pos) const noexcept { return s + _pos < e ? s + _pos : e; }
	constexpr iterator tail(iterator _p) noexcept { if (_p) {while (*_p) ++_p;} return _p; }
};



template<class T, class Iterator, class Traits>
constexpr auto view<T, Iterator, Traits>::hash() const noexcept -> size_type
{
	return	crypto::csx<size_type>{}.compute(s, e);
}




template<class T, class Iterator, class Traits>
template<class F>
constexpr auto view<T, Iterator, Traits>::substr(F _first) const noexcept -> view
{
	auto	f	= pos(_first);
	return	{f < e ? f : e, e};
}


template<class T, class Iterator, class Traits>
template<class F, class L>
constexpr auto view<T, Iterator, Traits>::substr(F _first, L _last) const noexcept -> view
{
	view	ret;
	ret.e	= e;
	ret.s	= pos(_first);
	ret.e	= ret.pos(_last);
	return	ret;
}


template<class T, class Iterator, class Traits>
constexpr auto view<T, Iterator, Traits>::find(view _s) const noexcept -> size_type
{
	if (size() < _s.size() && _s.empty())
	{
		return	npos;
	}
	auto	end	= e - _s.size();
	for (auto cur = s; cur != e; ++cur)
	{
		auto	s1	= cur;
		auto	s2	= _s.s;
		for (;;)
		{
			if (*s1++ != *s2++)
			{
				break;
			}
			if (s2 == _s.e)
			{
				return	cur - s;
			}
		}
	}
	return	npos;
}


template<class T, class Iterator, class Traits>
constexpr auto view<T, Iterator, Traits>::rfind(view _s) const noexcept -> size_type
{
	if (size() >= _s.size() && !_s.empty())
	{
		auto	end	= s - 1;
		for (auto cur = e - _s.size(); cur != end; --cur)
		{
			auto	s1	= cur;
			auto	s2	= _s.s;
			for (;;)
			{
				if (*s1++ != *s2++)
				{
					break;
				}
				if (s2 == _s.e)
				{
					return	cur - s;
				}
			}
		}
	}
	return	npos;
}


template<class T, class Iterator, class Traits>
constexpr auto view<T, Iterator, Traits>::find_first_of(value_type _c) const noexcept -> size_type
{
	for (int i = 0; i < (e-s); ++i)
	{
		if (s[i] == _c) { return i; }
	}
	return	npos;
}


template<class T, class Iterator, class Traits>
constexpr auto view<T, Iterator, Traits>::find_first_of(value_type const* _list) const noexcept -> size_type
{
	for (int i = 0; i < (e - s); ++i)
	{
		for (auto c = _list; *c; ++c)
		{
			if (s[i] == *c) { return i; }
		}
	}
	return	npos;
}


template<class T, class Iterator, class Traits>
constexpr auto view<T, Iterator, Traits>::find_first_of(view _list) const noexcept -> size_type
{
	for (auto cur = s; cur != e; ++cur)
	{
		if (_list.find_first_of(*cur) != npos) { return cur - s; }
	}
	return	npos;
}


template<class T, class Iterator, class Traits>
constexpr auto view<T, Iterator, Traits>::find_first_not_of(value_type _c) const noexcept -> size_type
{
	for (decltype(e - s) i = 0; i < (e-s); ++i)
	{
		if (s[i] != _c) { return i; }
	}
	return	npos;
}


template<class T, class Iterator, class Traits>
constexpr auto view<T, Iterator, Traits>::find_first_not_of(value_type const* _list) const noexcept -> size_type
{
	for (decltype(e - s) i = 0; i < (e - s); ++i)
	{
		bool	m	= false;
		for (auto c = _list; *c; ++c)
		{
			if (s[i] == *c) { m = true; break; }
		}
		if (!m)
		{
			return	i;
		}
	}
	return	npos;
}


template<class T, class Iterator, class Traits>
constexpr auto view<T, Iterator, Traits>::find_first_not_of(view _list) const noexcept -> size_type
{
	for (auto cur = s; cur != e; ++cur)
	{
		if (_list.find_first_of(*cur) == npos) { return cur - s; }
	}
	return	npos;
}


template<class T, class Iterator, class Traits>
constexpr auto view<T, Iterator, Traits>::find_last_of(value_type _c) const noexcept -> size_type
{
	for (size_type i = (e-s); i > 0; --i)
	{
		if (s[i-1] == _c)
		{
			return	i-1;
		}
	}
	return	npos;
}


template<class T, class Iterator, class Traits>
constexpr auto view<T, Iterator, Traits>::find_last_of(value_type const* _c) const noexcept -> size_type
{
	for (size_type i = (e-s); i > 0; --i)
	{
		auto	f	= _c;
		auto	c	= s[i-1];
		while (*f)
		{
			if (*f++ == c)
			{
				return	i-1;
			}
		}
	}
	return	npos;
}


template<class T, class Iterator, class Traits>
constexpr auto view<T, Iterator, Traits>::find_last_of(view _c) const noexcept -> size_type
{
	for (size_type i = (e-s); i > 0; --i)
	{
		auto	f	= _c;
		auto	c	= s[i-1];
		for (auto cc : _c)
		{
			if (cc == c)
			{
				return	i-1;
			}
		}
	}
	return	npos;
}


template<class T, class Iterator, class Traits>
constexpr auto view<T, Iterator, Traits>::find_last_not_of(value_type _c) const noexcept -> size_type
{
	for (size_type i = (e-s); i > 0; --i)
	{
		if (s[i-1] != _c)
		{
			return	i-1;
		}
	}
	return	npos;
}


template<class T, class Iterator, class Traits>
constexpr auto view<T, Iterator, Traits>::find_last_not_of(value_type const* _list) const noexcept -> size_type
{
	return	find_last_not_of(view(_list));
}


template<class T, class Iterator, class Traits>
constexpr auto view<T, Iterator, Traits>::find_last_not_of(view _list) const noexcept -> size_type
{
	if (!empty())
	{
		for (auto* cur = e; cur != s; --cur)
		{
			if (_list.find_first_of(*(cur-1)) == npos)
			{
				return	(cur - 1 - s);
			}
		}
	}
	return	npos;
}


template<class T, class Iterator, class Traits>
constexpr bool view<T, Iterator, Traits>::starts_with(value_type const* _text) const noexcept
{
	auto	cur = s;
	while (*_text)
	{
		if (cur == e || *_text++ != *cur++)
		{
			return	false;
		}
	}
	return	true;
}


template<class T, class Iterator, class Traits>
constexpr bool view<T, Iterator, Traits>::starts_with(view _text) const noexcept
{
	if (size() < _text.size())
	{
		return	false;
	}
	auto	c1	= s;
	auto	c2	= _text.s;
	while (c2 != _text.e)
	{
		if (*c1++ != *c2++)
		{
			return	false;
		}
	}
	return	true;
}


template<class T, class Iterator, class Traits>
constexpr bool view<T, Iterator, Traits>::ends_with(value_type const* _text) const noexcept
{
	return	ends_with(view(_text));
}


template<class T, class Iterator, class Traits>
constexpr bool view<T, Iterator, Traits>::ends_with(view _text) const noexcept
{
	if (size() < _text.size())
	{
		return	false;
	}
	auto	c1	= e - _text.size();
	auto	c2	= _text.s;
	while (c2 != _text.e)
	{
		if (*c1++ != *c2++)
		{
			return	false;
		}
	}
	return	true;
}

template<class T, class Iterator, class Traits>
constexpr auto view<T, Iterator, Traits>::match_first_of(value_type const* _text) const noexcept -> view
{
	auto	cur = s;
	while (*_text && cur != e && *_text == *cur)
	{
		++_text;
		++cur;
	}
	return	substr(0, cur - s);
}

template<class T, class Iterator, class Traits>
constexpr auto view<T, Iterator, Traits>::match_first_of(view _text) const noexcept -> view
{
	size_type	maxlen	= size() < _text.size() ? size() : _text.size();
	size_type	len		= 0;
	while (len < maxlen && s[len] == _text.s[len])
	{
		++len;
	}
	return	substr(0, len);
}

template<class T, class Iterator, class Traits>
constexpr auto view<T, Iterator, Traits>::trim_first(value_type _c) const noexcept -> view
{
	auto	pos	= find_first_not_of(_c);
	return	pos == npos ? substr(size()) : substr(pos);
}


template<class T, class Iterator, class Traits>
constexpr auto view<T, Iterator, Traits>::trim_first(value_type const* _list) const noexcept -> view
{
	auto	pos	= find_first_not_of(_list);
	return	pos == npos ? substr(size()) : substr(pos);
}


template<class T, class Iterator, class Traits>
constexpr auto view<T, Iterator, Traits>::trim_first(view _list) const noexcept -> view
{
	auto	pos	= find_first_not_of(_list);
	return	pos == npos ? substr(size()) : substr(pos);
}


template<class T, class Iterator, class Traits>
constexpr auto view<T, Iterator, Traits>::trim_last(value_type _c) const noexcept -> view
{
	auto	pos	= find_last_not_of(_c);
	return	pos == npos ? substr(0, 0) : substr(0, pos+1);
}

template<class T, class Iterator, class Traits>
constexpr auto view<T, Iterator, Traits>::trim_last(value_type const* _list) const noexcept -> view
{
	auto	pos	= find_last_not_of(_list);
	return	pos == npos ? substr(0, 0) : substr(0, pos+1);
}


template<class T, class Iterator, class Traits>
constexpr auto view<T, Iterator, Traits>::trim_last(view _list) const noexcept -> view
{
	auto	pos	= find_last_not_of(_list);
	return	pos == npos ? substr(0, 0) : substr(0, pos+1);
}


template<class T, class Iterator, class Traits>
constexpr auto view<T, Iterator, Traits>::compare(view _r) const noexcept -> int
{
	value_type const*		cur1	= s;
	value_type const*		cur2	= _r.s;
	if (size() == _r.size())
	{
		for (size_type i = 0; i < size(); ++i)
		{
			if (cur1[i] != cur2[i])
			{
				return	static_cast<unsigned int>(cur1[i]) - static_cast<unsigned int>(cur2[i]);
			}
		}
		return	0;
	}
	value_type const*		end1	= e;
	value_type const*		end2	= _r.e;
	if (size() > _r.size())
	{
		std::swap(cur1, cur2);
		std::swap(end1, end2);
	}
	while (cur1 != end1)
	{
		unsigned int const	c1	= static_cast<unsigned int>(*cur1++);
		unsigned int const	c2	= static_cast<unsigned int>(*cur2++);
		if (c1 != c2)
		{
			int	r	= (int)c1 - (int)c2;
			return	size() > _r.size() ? -r : r;
		}
	}
	return	size() > _r.size() ? *cur2 : -*cur2;
}


template<class T, class Iterator, class Traits>
constexpr auto view<T, Iterator, Traits>::trim_first_one_return() const noexcept -> view
{
	view	ret(*this);
	auto		c1	= ret.front();
	if (c1 == '\n' || c1 == '\r')
	{
		ret	= ret.substr(1);
		char	c2	= ret.front();
		if (c2 == '\n' || c2 == '\r' && c1 != c2)
		{
			ret	= ret.substr(1);
		}
	}
	return	ret;
}


template<class T, class Iterator, class Traits>
constexpr auto view<T, Iterator, Traits>::read_line() const noexcept -> view
{
	auto	pos	= find_first_of("\r\n");
	if (pos == npos)
	{
		return	*this;
	}
	if (pos < size())
	{
		auto	c	= data()[pos];
		if (c == '\n' || c == '\r')
		{
			++pos;
			if (pos < size())
			{
				auto	c2	= data()[pos];
				if (c2 != c && (c2 == '\n' || c2 == '\r'))
				{
					++pos;
				}
			}
		}
	}

	return	substr(0, pos);
}



template<class T>
struct	view_traits<T*>
{
	using	size_type			= size_t;
	using	char_type			= T;
	using	value_type			= char_type;
	using	const_value_type	= typename std::add_const<value_type>::type;
	using	iterator			= T*;
	using	const_iterator		= const_value_type*;
	using	reference			= value_type&;
	using	const_reference		= const_value_type&;

	static constexpr auto eof() noexcept -> value_type
	{
		return	0;
	}

	static constexpr auto init() noexcept
	{
		return	nullptr;
	}
};


template<class T>
struct	view_traits<T>
{
	using	size_type			= size_t;
	using	char_type			= typename std::remove_reference<decltype(*std::declval<T>())>::type;
	using	value_type			= char_type;
	using	const_value_type	= typename std::add_const<value_type>::type;
	using	iterator			= T;
	using	const_iterator		= const_value_type*;
	using	reference			= value_type&;
	using	const_reference		= const_value_type&;

	static constexpr auto eof() noexcept -> value_type
	{
		return	0;
	}

	static constexpr auto init() noexcept
	{
		return	iterator{};
	}
};



template<class T, class Iterator, class Traits>
struct	io::reader_traits<view<T, Iterator, Traits>>
{
	using	size_type	= reader_base::size_type;
	using	pointer		= reader_base::pointer;
	using	reader_type	= view<T, Iterator, Traits>;

	reader_type	_start;

	reader_traits(reader_type _src)
		: _start(std::move(_src))
	{}

	void seekg(reader_type& _src, std::streamoff _off, std::ios_base::seekdir _dir)
	{
		switch (_dir)
		{
			case std::ios_base::beg :
				_src	= _start.substr((size_t)_off);
				break;
			case std::ios_base::cur :
			{
				auto	pos	= _src.data() + _off;
				if (pos < _start.data())
				{
					_src	= _start;
				}
				else if (pos > _start.data() + _start.size())
				{
					_src	= _start.substr(_start.size());
				}
				else
				{
					_src	= _start.substr(pos);
				}
				break;
			}
			case std::ios_base::end :
				_src	= _start.substr(_start.size() + _off);
				break;
		}
	}

	size_type read(reader_type& _src, pointer _buff, size_type _len)
	{
		if (_len > _src.size())
		{
			_len	= _src.size();
		}
		auto*		dst	= reinterpret_cast<typename std::remove_const<typename reader_type::value_type>::type*>(_buff);
		for (size_type i = 0; i < _len; ++i)
		{
			dst[i] = *_src++;
		}
		return	_len;
	}
};


template<class T, class Iterator, class Traits>
struct	io::writer_traits<view<T, Iterator, Traits>>
{
	using	size_type	= writer_base::size_type;
	using	pointer		= writer_base::pointer;
	using	writer_type	= view<T, Iterator, Traits>;

	writer_type	_start;

	writer_traits(writer_type _src)
		: _start(std::move(_src))
	{}

	void seekp(writer_type& _src, std::streamoff _off, std::ios_base::seekdir _dir)
	{
		switch (_dir)
		{
			case std::ios_base::beg :
			{
				_src	= _start.substr(_off);
				break;
			}
			case std::ios_base::cur :
			{
				auto	pos	= _src.data() + _off;
				if (pos < _start.data())
				{
					_src	= _start;
				}
				else if (pos > _start.data() + _start.size())
				{
					_src	= _start.substr(_start.size());
				}
				else
				{
					_src	= _start.substr(pos);
				}
				break;
			}
			case std::ios_base::end :
			{
				_src	= _start.substr(_start.size() + _off);
				break;
			}
		}

	}

	size_type write(writer_type& _dst, pointer _buff, size_type _len)
	{
		if (_len > _dst.size())
		{
			_len	= _dst.size();
		}
		auto*		src	= reinterpret_cast<typename writer_type::value_type const*>(_buff);
		for (size_type i = 0; i < _len; ++i)
		{
			*_dst++ = *src++;
		}
		return	_len;
	}
};


AMTRS_NAMESPACE_END
#endif

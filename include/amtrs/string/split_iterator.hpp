/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__string__split_iterator__hpp
#define	__libamtrs__string__split_iterator__hpp
#include "text.hpp"
AMTRS_NAMESPACE_BEGIN
template<class OriginT, class DelimiterT>	class	split_iterator;
template<class CharT>						class	basic_string_delimiter;
template<class CharT>						class	basic_chars_delimiter;


// ============================================================================
//! 文字で分割します
// ----------------------------------------------------------------------------
inline auto split_chars(text _string, text _delimiters) -> split_iterator<text, basic_chars_delimiter<char>>;

// ============================================================================
//! 文字列で分割します
// ----------------------------------------------------------------------------
inline auto split_string(text _string, text _delimiter) -> split_iterator<text, basic_string_delimiter<char>>;

// ============================================================================
//! 正規表現で分割します
// ----------------------------------------------------------------------------
//inline auto split_regex(text _string, std::regex& _delimiter) -> split_iterator<text, basic_regex_delimiter<char>>;


// ============================================================================
//! トークン分割を行います。
// ----------------------------------------------------------------------------
//! このクラスを直接利用するのではなく、以下の関数を使ってください。
//! split_chars(text _string, text _delimiters)
//! split_string(text _string, text _delimiter)
//! split_regex(text _string, std::regex& _delimiter)
//!
//! example)
//!		for (auto token : split_chars("aa:bb/cc:dd", ":/")) {}
// ----------------------------------------------------------------------------
template<class OriginT, class DelimiterT>
class	split_iterator;


template<class IteratorT>
struct	iterator_range
{
	using	iterator	= IteratorT;

	template<class Ite>
	iterator_range(Ite&& _first, Ite&& _last)
		: mBegin(_first)
		, mEnd(_last)
	{}

	template<class ContainorT>
	iterator_range(ContainorT&& _containor)
		: iterator_range(_containor.begin(), _containor.end())
	{}

	iterator begin() const noexcept { return mBegin; }
	iterator end() const noexcept { return mEnd; }

protected:
	iterator	mBegin;
	iterator	mEnd;
};

AMTRS_IMPLEMENTS_BEGIN(split_iterator)
template<class...>
struct	split_iterator_cond;

template<class ContainorT>
struct	split_iterator_cond<ContainorT>
{
	using	type	= iterator_range<typename ContainorT::iterator>;
};

template<class CharT>
struct	split_iterator_cond<basic_text<CharT>>
{
	using	type	= basic_text<CharT>;
};
AMTRS_IMPLEMENTS_END(split_iterator)


template<class OriginT, class DelimiterT>
class	split_iterator
{
public:
	using	size_type			= size_t;
	using	value_type			= typename AMTRS_IMPLEMENTS(split_iterator)::split_iterator_cond<OriginT>::type;
	using	pointer				= value_type*;
	using	const_pointer		= value_type const*;
	using	reference			= value_type;
	using	const_reference		= value_type const&;
	using	difference_type		= std::size_t;
    using	iterator_category	= std::input_iterator_tag;

	using	origin_type		= OriginT;
	using	delimiter_type	= DelimiterT;

	using	origin_iterator	= typename origin_type::const_iterator;

	split_iterator() = default;
	split_iterator(split_iterator const&) = default;
	split_iterator(split_iterator&&) = default;
	split_iterator& operator = (split_iterator const&) = default;
	split_iterator& operator = (split_iterator&&) = default;

	split_iterator(origin_iterator _first, origin_iterator _last, delimiter_type&& _delimiter)
		: mCurrent(_first)
		, mEnd(_last)
		, mDelimiter(std::move(_delimiter))
	{
		compute_view();
	}

	split_iterator(origin_iterator _first, origin_iterator _last, const delimiter_type& _delimiter)
		: mCurrent(_first)
		, mEnd(_last)
		, mDelimiter(_delimiter)
	{
		compute_view();
	}

	auto begin() const -> split_iterator { return split_iterator(mCurrent, mEnd, mDelimiter); }
	auto end() const -> split_iterator { return split_iterator(mEnd, mEnd, mDelimiter); }
	auto size() const noexcept -> size_type { size_type s = 0; for (auto& v : *this) ++s; return s; }

	bool operator == (const split_iterator& _r) const noexcept { return	mCurrent == _r.mCurrent; }
	bool operator != (const split_iterator& _r) const noexcept { return mCurrent != _r.mCurrent; }
	auto operator * () const noexcept  -> const_reference { return mCurrentView; }
	auto operator -> () const noexcept -> const_pointer { return &mCurrentView; }

	split_iterator& operator ++ ()
	{
		mCurrent	= mNext;
		compute_view();
		return	*this;
	}

	split_iterator operator ++ (int)
	{
		split_iterator	retval(*this);
		++(*this);
		return	retval;
	}


	auto operator - (const split_iterator& _r) const noexcept -> size_type
	{
		std::size_t				count	= 1;
		split_iterator	c(begin());
		split_iterator	e(end());
		while (c != e)
		{
			++c;
			++count;
		}
		return	count;
	}

	template<class ContainorT>
	ContainorT create_containor() const
	{
		return	ContainorT(begin(), end());
	}

protected:
	// view の範囲を計算します。
	void compute_view()
	{
		if (mCurrent != mEnd)
		{
			auto 	ret		= mDelimiter.find(static_cast<const origin_iterator&>(mCurrent), mEnd);
			auto&	tail	= ret.first;
			auto&	next	= ret.second;
			if (tail-mCurrent == 0)
			{
				mCurrentView	= {&*mCurrent, (size_t)0};
			}
			else
			{
				mCurrentView	= {&*mCurrent, (size_t)(tail-mCurrent)};
			}
			mNext			= next;
		}
	}

	origin_iterator		mCurrent;
	origin_iterator		mEnd;
	text				mCurrentView	= { nullptr };
	origin_iterator		mNext;
	delimiter_type		mDelimiter;
};


template<class CharT>
class	basic_chars_delimiter
{
public:
	using	view_type	= basic_text<CharT>;
	using	char_type	= CharT;
	static constexpr auto	npos = view_type::npos;

	basic_chars_delimiter(basic_chars_delimiter&&) = default;
	basic_chars_delimiter(const basic_chars_delimiter&) = default;
	basic_chars_delimiter& operator = (basic_chars_delimiter&&) = default;
	basic_chars_delimiter& operator = (const basic_chars_delimiter&) = default;

	template<class... A>
	basic_chars_delimiter(A&&... _view)
		: mChars(std::forward<A>(_view)...)
	{}

	template<class IteratorT>
	std::pair<IteratorT, IteratorT> find(IteratorT _first, IteratorT _last) const noexcept
	{
		for (auto cursor = _first; cursor != _last; ++cursor)
		{
			if (mChars.find_first_of(*cursor) != npos)
			{
				return	{cursor, cursor+1};
			}
		}
		return	{_last, _last};
	}

protected:
	view_type	mChars;
};

template<class CharT>
class	basic_string_delimiter
{
public:
	using	view_type	= basic_text<CharT>;
	using	char_type	= CharT;
	static constexpr auto	npos = view_type::npos;

	basic_string_delimiter(basic_string_delimiter&&) = default;
	basic_string_delimiter(const basic_string_delimiter&) = default;
	basic_string_delimiter& operator = (basic_string_delimiter&&) = default;
	basic_string_delimiter& operator = (const basic_string_delimiter&) = default;

	template<class... A>
	basic_string_delimiter(A&&... _view)
		: mView(std::forward<A>(_view)...)
	{}

	template<class IteratorT>
	std::pair<IteratorT, IteratorT> find(IteratorT _first, IteratorT _last) const noexcept
	{
		for (auto cursor = _first; cursor != _last; ++cursor)
		{
			if (start_with(cursor, _last))
			{
				return	{cursor, cursor + mView.size()};
			}
		}
		return	{_last, _last};
	}

protected:
	template<class IteratorT>
	bool start_with(IteratorT _first, IteratorT _last) const noexcept
	{
		for (auto c : mView)
		{
			if (_first == _last || *_first++ != c)
			{
				return	false;
			}
		}
		return	true;
	}

	view_type	mView;
};
/*
template<class CharT>
class	basic_regex_delimiter
{
public:
	using	view_type	= basic_text<CharT>;
	using	char_type	= CharT;
	static constexpr auto	npos = view_type::npos;

	basic_regex_delimiter(basic_regex_delimiter&&) = default;
	basic_regex_delimiter(const basic_regex_delimiter&) = default;

	basic_regex_delimiter(std::regex& _regex)
		: mRegex(_regex)
	{}

	template<class IteratorT>
	std::pair<IteratorT, IteratorT> find(IteratorT _first, IteratorT _last) const noexcept
	{
		std::match_results<IteratorT>	match;
		if (std::regex_search(_first, _last, match, mRegex))
		{
			auto	tail	= _first + match.position();
			return	{tail, tail + match.length()};
		}
		return	{_last, _last};
	}

protected:
	std::regex&	mRegex;
};
*/


// ============================================================================
//! 文字で分割します
// ----------------------------------------------------------------------------
auto split_chars(text _string, text _delimiters)
	-> split_iterator<text, basic_chars_delimiter<char>>
{
	return	split_iterator<text, basic_chars_delimiter<char>>(_string.begin(), _string.end(), _delimiters);
}


// ============================================================================
//! 文字列で分割します
// ----------------------------------------------------------------------------
auto split_string(text _string, text _delimiter)
	-> split_iterator<text, basic_string_delimiter<char>>
{
	return	split_iterator<text, basic_string_delimiter<char>>(_string.begin(), _string.end(), _delimiter);
}


// ============================================================================
//! 正規表現で分割します
// ----------------------------------------------------------------------------
/*
auto split_regex(text _string, std::regex& _delimiter)
	-> split_iterator<text, basic_regex_delimiter<char>>
{
	return	split_iterator<text, basic_regex_delimiter<char>>(_string.begin(), _string.end(), _delimiter);
}
*/

AMTRS_NAMESPACE_END
namespace std {


template<class OriginT, class DelimiterT>
auto to_address(amtrs::split_iterator<OriginT, DelimiterT> const& _si)
{
	return	(*_si).data();
}


}
#endif

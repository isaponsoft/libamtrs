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
#ifndef	__libamtrs__string__split_iterator__hpp
#define	__libamtrs__string__split_iterator__hpp
#include <string>
#include <string_view>
#include <regex>
#include "def.hpp"
AMTRS_NAMESPACE_BEGIN

// ============================================================================
//! トークン分割を行います。
// ----------------------------------------------------------------------------
//! このクラスを直接利用するのではなく、以下の関数を使ってください。
//! split_chars(std::string_view _string, std::string_view _delimiters)
//! split_string(std::string_view _string, std::string_view _delimiter)
//! split_regex(std::string_view _string, std::regex& _delimiter)
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

template<class CharT, class TraitsT>
struct	split_iterator_cond<std::basic_string_view<CharT, TraitsT>>
{
	using	type	= std::basic_string_view<CharT, TraitsT>;
};
AMTRS_IMPLEMENTS_END(split_iterator)


template<class OriginT, class DelimiterT>
class	split_iterator
{
public:
	using	value_type			= typename AMTRS_IMPLEMENTS(split_iterator)::split_iterator_cond<OriginT>::type;
	using	reference			= value_type;
	using	const_reference		= value_type;
	using	difference_type		= std::size_t;
    using	iterator_category	= std::input_iterator_tag;

	using	origin_type		= OriginT;
	using	delimiter_type	= DelimiterT;

	using	origin_iterator	= typename origin_type::const_iterator;


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

	split_iterator begin() const
	{
		return	split_iterator(mCurrent, mEnd, mDelimiter);
	}

	split_iterator end() const
	{
		return	split_iterator(mEnd, mEnd, mDelimiter);
	}

	bool operator == (const split_iterator& _r) const noexcept
	{
		return	mCurrent == _r.mCurrent;
	}

	bool operator != (const split_iterator& _r) const noexcept
	{
		return	mCurrent != _r.mCurrent;
	}

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

	value_type operator * () const noexcept
	{
		return	mCurrentView;
	}

	const value_type* operator -> () const noexcept
	{
		return	&mCurrentView;
	}

	std::size_t operator - (const split_iterator& _r) const noexcept
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
			auto [tail, next] = mDelimiter(static_cast<const origin_iterator&>(mCurrent), mEnd);
			if (tail-mCurrent == 0)
			{
				mCurrentView	= {nullptr, 0};
			}
			else
			{
				mCurrentView	= {&*mCurrent, std::size_t(tail-mCurrent)};
			}
			mNext			= next;
		}
	}

	origin_iterator		mCurrent;
	origin_iterator		mEnd;
	std::string_view	mCurrentView;
	origin_iterator		mNext;
	delimiter_type		mDelimiter;
};


template<class CharT, class TraitsT = std::char_traits<CharT>>
class	basic_chars_delimiter
{
public:
	using	view_type	= std::basic_string_view<CharT, TraitsT>;
	using	char_type	= CharT;
	static constexpr auto	npos = view_type::npos;

	basic_chars_delimiter(basic_chars_delimiter&&) = default;
	basic_chars_delimiter(const basic_chars_delimiter&) = default;

	template<class... A>
	basic_chars_delimiter(A&&... _view)
		: mChars(std::forward<A>(_view)...)
	{}

	template<class IteratorT>
	std::pair<IteratorT, IteratorT> operator () (IteratorT _first, IteratorT _last) const noexcept
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

template<class CharT, class TraitsT = std::char_traits<CharT>>
class	basic_string_delimiter
{
public:
	using	view_type	= std::basic_string_view<CharT, TraitsT>;
	using	char_type	= CharT;
	static constexpr auto	npos = view_type::npos;

	basic_string_delimiter(basic_string_delimiter&&) = default;
	basic_string_delimiter(const basic_string_delimiter&) = default;

	template<class... A>
	basic_string_delimiter(A&&... _view)
		: mView(std::forward<A>(_view)...)
	{}

	template<class IteratorT>
	std::pair<IteratorT, IteratorT> operator () (IteratorT _first, IteratorT _last) const noexcept
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

template<class CharT, class TraitsT = std::char_traits<CharT>>
class	basic_regex_delimiter
{
public:
	using	view_type	= std::basic_string_view<CharT, TraitsT>;
	using	char_type	= CharT;
	static constexpr auto	npos = view_type::npos;

	basic_regex_delimiter(basic_regex_delimiter&&) = default;
	basic_regex_delimiter(const basic_regex_delimiter&) = default;

	basic_regex_delimiter(std::regex& _regex)
		: mRegex(_regex)
	{}

	template<class IteratorT>
	std::pair<IteratorT, IteratorT> operator () (IteratorT _first, IteratorT _last) const noexcept
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




inline auto split_chars(std::string_view _string, std::string_view _delimiters)
	-> split_iterator<std::string_view, basic_chars_delimiter<char>>
{
	return	split_iterator<std::string_view, basic_chars_delimiter<char>>(_string.begin(), _string.end(), _delimiters);
}


inline auto split_string(std::string_view _string, std::string_view _delimiter)
	-> split_iterator<std::string_view, basic_string_delimiter<char>>
{
	return	split_iterator<std::string_view, basic_string_delimiter<char>>(_string.begin(), _string.end(), _delimiter);
}


inline auto split_regex(std::string_view _string, std::regex& _delimiter)
	-> split_iterator<std::string_view, basic_regex_delimiter<char>>
{
	return	split_iterator<std::string_view, basic_regex_delimiter<char>>(_string.begin(), _string.end(), _delimiter);
}


AMTRS_NAMESPACE_END
#endif

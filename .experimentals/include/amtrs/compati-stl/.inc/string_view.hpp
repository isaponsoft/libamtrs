/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__compati__stl__string_view__hpp
#define	__libamtrs__compati__stl__string_view__hpp
#include <cstddef>
namespace std {

// Emscripten などの string_view が未実装なプラットフォームで使用する
// basic_string_view の互換ライブラリです。
template<class CharT, class Traits = std::char_traits<CharT>>
class	basic_string_view
{
public:
	using	char_type		= CharT;
	using	size_type		= size_t;
	using	iterator		= char_type*;
	using	const_iterator	= char_type const*;

	static constexpr size_type	npos	= static_cast<size_type>(-1);


	basic_string_view(const basic_string<CharT, Traits>& _str)
		: txt(_str.data())
		, len(_str.size())
	{}

	basic_string_view() = default;
	constexpr basic_string_view(const basic_string_view&) = default;
	constexpr basic_string_view(const char* _beg, std::size_t _sz) : txt(_beg), len(_sz) {}
	constexpr basic_string_view(const char* _beg) : txt(_beg), len(_beg ? strlen(_beg) : 0) {}
	constexpr basic_string_view& operator = (const basic_string_view&) = default;
	constexpr basic_string_view& operator = (basic_string_view&&) = default;
	constexpr bool operator == (const basic_string_view& _b) const noexcept { return len == _b.len && memcmp(txt, _b.txt, len) == 0; }
	constexpr bool operator != (const basic_string_view& _b) const noexcept { return !(*this == _b); }
	constexpr std::size_t size() const noexcept { return len; }
	constexpr std::size_t length() const noexcept { return len; }
	constexpr bool empty() const noexcept { return size() == 0; }
	constexpr auto begin() noexcept { return txt; }
	constexpr auto end() noexcept { return txt + len; }
	constexpr auto begin() const noexcept { return txt; }
	constexpr auto end() const noexcept { return txt + len; }
	constexpr auto cbegin() const noexcept { return txt; }
	constexpr auto cend() const noexcept { return txt + len; }
	constexpr const char* data() const noexcept { return txt; }

	char_type const& front() const noexcept { return *begin(); }
	char_type const& back() const noexcept { return *(data() + size() + 1); }

	template<class I>
	char_type const& operator [] (const I& _index) const noexcept { return data()[(size_type)_index]; }

	basic_string_view substr(size_t _pos) const noexcept
	{
		return	basic_string_view(data() + _pos, this->len);
	}

	basic_string_view substr(size_t _pos, size_t _len) const noexcept
	{
		return	basic_string_view(data() + _pos, _len);
	}

	size_type find_first_of(char_type _c) const noexcept
	{
		for (auto it = begin(); it != end(); ++it)
		{
			if (*it == _c)
			{
				return	it - begin();
			}
		}
		return	npos;
	}

	size_type find_first_of(char_type _c, size_type _pos) const noexcept
	{
		for (auto it = begin() + _pos; it != end(); ++it)
		{
			if (*it == _c)
			{
				return	it - begin();
			}
		}
		return	npos;
	}

	size_type find_last_of(char_type _c) const noexcept
	{
		for (auto it = end()-1; it != begin()-1; --it)
		{
			if (*it == _c)
			{
				return	it - begin();
			}
		}
		return	npos;
	}

	size_type find_last_of(char_type _c, size_type _pos) const noexcept
	{
		for (auto it = begin() + _pos - 1; it != begin()-1; --it)
		{
			if (*it == _c)
			{
				return	it - begin();
			}
		}
		return	npos;
	}

	void swap(basic_string_view& b) noexcept { std::swap(txt, b.txt); std::swap(len, b.len); }

protected:
	const char*		txt;
	size_t			len;
};

using	string_view	= basic_string_view<char>;


template<class CharT, class Traits>
struct	hash<basic_string_view<CharT, Traits>>
{
	size_t operator () (const basic_string_view<CharT, Traits>& _s) noexcept
	{
		return	hash<basic_string<CharT, Traits>>{}(basic_string<CharT, Traits>(_s.data(), _s.size()));
	}
};

}
#endif

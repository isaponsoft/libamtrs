/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__string__constring__hpp
#define	__libamtrs__string__constring__hpp
#include <iterator>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include "def.hpp"
AMTRS_NAMESPACE_BEGIN

//! constexpr で演算可能な静的な文字列です。
//! example)
//! 	constexpr auto	hello	= make_constring("hello ");
//! 	constexpr auto	world	= make_constring("world.");
template<std::size_t Size>
struct	constring
{
	class iterator: public std::iterator<std::input_iterator_tag, char>
	{
	public:
		using	reference	= const char&;

	    explicit iterator(const char* _pos = 0) : m_pos(_pos) {}
	    iterator& operator++ () noexcept { ++m_pos; return *this;}
	    iterator operator++ (int) noexcept { iterator retval = *this; ++(*this); return retval; }
	    bool operator == (iterator _other) const noexcept { return m_pos == _other.m_pos; }
	    bool operator != (iterator _other) const noexcept { return !(*this == _other); }
	    reference operator* () const { return *m_pos; }
	private:
		const char*		m_pos;
	};

	iterator begin() const noexcept { return iterator(&m_string[0]); }
	iterator end() const noexcept { return iterator(&m_string[Size]); }

	constexpr constring(const char (&_s)[Size+1]) noexcept
		: constring(_s, std::make_index_sequence<Size>())
	{}

	template<std::size_t... I>
	constexpr constring(const char* _s, std::index_sequence<I...>) noexcept
		: m_string{getchar<I>(_s)..., 0}
	{}

	template<class L, class R, std::size_t... I1, std::size_t... I2>
	constexpr constring(L _a, R _b, std::index_sequence<I1...>, std::index_sequence<I2...>) noexcept
		: m_string{getchar<I1>(_a)..., getchar<I2>(_b)..., 0}
	{}

	constexpr const char* c_str() const noexcept { return m_string; }
	constexpr const char* data() const noexcept { return m_string; }

	constexpr std::size_t size() const noexcept { return Size; }

	template<class I>
	constexpr char operator [] (I _index) noexcept { return m_string[_index]; }

	template<std::size_t RS>
	constexpr constring<Size + RS> operator + (const constring<RS>& _r) const noexcept
	{
		return	constring<Size + RS>(*this, _r, std::make_index_sequence<Size>(), std::make_index_sequence<RS>());
	}

	constexpr std::string_view view() const noexcept { return { m_string, size() }; }
	std::string string() const { return { m_string, size() }; }

	constexpr explicit operator std::string_view() const noexcept { return view(); }
	explicit operator std::string() const noexcept { return string(); }

	constexpr bool empty() const noexcept { return Size == 1; }

	template<std::size_t RS>
	constexpr bool operator == (const constring<RS>& _r) const noexcept
	{
		if (Size != RS)
		{
			return	false;
		}
		for (std::size_t i = 0; i < Size; ++i)
		{
			if (m_string[i] != _r.m_string[i])
			{
				return	false;
			}
		}
		return	true;
	}

	template<std::size_t RS>
	constexpr bool operator != (const constring<RS>& _r) const noexcept
	{
		return	!(*this == _r);
	}

private:
	template<std::size_t I>
	static constexpr char getchar(const char* _string) noexcept { return _string[I]; }

	template<std::size_t I, std::size_t S>
	static constexpr char getchar(const constring<S>& _string) noexcept { return _string.m_string[I]; }

	char	m_string[Size + 1];

	template<std::size_t S>
	friend	struct	constring;
};

template<std::size_t N>
constexpr constring<N-1> make_constring(char const (&_tx)[N]) noexcept
{
	return	constring<N-1>(_tx);
}

template<std::size_t N1, std::size_t N2>
constexpr constring<N1+N2> make_constring(const constring<N1>& _a, const constring<N2>& _b) noexcept
{
	return	constring<N1+N2>(_a, _b, std::make_index_sequence<N1>(), std::make_index_sequence<N2>());
}


AMTRS_NAMESPACE_END
#endif

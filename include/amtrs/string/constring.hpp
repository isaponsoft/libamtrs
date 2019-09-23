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

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
#ifndef	__libamtrs__string__string_view__h
#define	__libamtrs__string__string_view__h
#include "def.hpp"
#ifdef	_MSC_VER
#pragma warning(push)
#pragma warning(disable:4190)
#endif
AMTRS_EXTERN_C_BEGIN

/* std::string_view for c language.
 *
 * std::string_view とほぼ同等の機能を持つ文字列のビューです。
 */
typedef	struct	_amtrs_string_view
{
	const char*		txt;
	size_t			len;


	#ifdef	__cplusplus
	_amtrs_string_view() = default;
	constexpr _amtrs_string_view(const _amtrs_string_view&) = default;
	constexpr _amtrs_string_view(const char* _beg, std::size_t _sz) : txt(_beg), len(_sz) {}
	constexpr _amtrs_string_view(std::string_view _v) : txt(_v.data()), len(_v.size()) {}
	constexpr _amtrs_string_view(const char* _beg) : txt(_beg), len(_beg ? strlen(_beg) : 0) {}
	constexpr _amtrs_string_view& operator = (std::string_view _r) { txt = _r.data(); len = _r.size(); return *this; }
	constexpr _amtrs_string_view& operator = (const _amtrs_string_view&) = default;
	constexpr _amtrs_string_view& operator = (_amtrs_string_view&&) = default;
	constexpr bool operator == (const _amtrs_string_view& _b) const noexcept { return len == _b.len && memcmp(txt, _b.txt, len) == 0; }
	constexpr bool operator != (const _amtrs_string_view& _b) const noexcept { return !(*this == _b); }
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
	_amtrs_string_view substr(size_t _pos) const noexcept;
	_amtrs_string_view substr(size_t _pos, size_t _len) const noexcept;
	constexpr operator std::string_view () const noexcept { return {txt, len}; }
	void swap(_amtrs_string_view& b) noexcept { std::swap(txt, b.txt); std::swap(len, b.len); }
	#endif	//cplusplus
} amtrs_string_view;


inline extern amtrs_string_view amtrs_string_view_make(const char* txt)
{
	amtrs_string_view	retval;
	retval.txt	= txt;
	retval.len	= strlen(txt);
	return	retval;
}


inline extern amtrs_string_view amtrs_string_view_substr(amtrs_string_view _view, size_t _pos, size_t _len)
{
	amtrs_string_view	retval	= _view;
	size_t				rpos	= _pos + _len;
	if (rpos > retval.len)
	{
		if (_pos >= retval.len)
		{
			retval.txt	+= retval.len;
			retval.len	=  0;
		}
		else
		{
			retval.txt	+= _pos;
			retval.len	-= _pos;
		}
	}
	else
	{
		retval.txt	+= _pos;
		retval.len	=  _len;
	}
	return	retval;
}


inline extern size_t amtrs_string_view_compute_hash(amtrs_string_view _view)
{
	size_t			hash	= 0;
	const char*		cursor	= _view.txt;
	const char*		end		= _view.txt + _view.len;
	while (cursor != end)
	{
		hash = hash * 31 - 0x1f + *cursor++;
	}
	return	hash;
}


inline extern size_t amtrs_string_view_compute_hash_continue(size_t _hash, amtrs_string_view _view)
{
	size_t			hash	= _hash;
	const char*		cursor	= _view.txt;
	const char*		end		= _view.txt + _view.len;
	while (cursor != end)
	{
		hash = hash * 31 - 0x1f + *cursor++;
	}
	return	hash;
}


inline extern amtrs_bool amtrs_string_view_equals(amtrs_string_view _a, amtrs_string_view _b)
{
	size_t		i;
	size_t		s;
	const char*	a;
	const char*	b;
	if (_a.len != _b.len)
	{
		return	amtrs_false;
	}
	a	= _a.txt;
	b	= _b.txt;
	s	= _a.len / sizeof(void*);
	if (s > 0)
	{
		for (i = 0; i < s; ++i)
		{
			if (((const void**)a)[i] != ((const void**)b)[i])
			{
				return	amtrs_false;
			}
		}
		a	+= s * sizeof(void*);
		b	+= s * sizeof(void*);
	}
	s	= _a.len % sizeof(void*);
	for (i = 0; i < s; ++i)
	{
		if (((const int8_t*)a)[i] != ((const int8_t*)b)[i])
		{
			return	amtrs_false;
		}
	}
	return	amtrs_true;
}


//! _a が _b で始まるか調べる。
inline extern amtrs_bool amtrs_string_view_starts_with(amtrs_string_view _a, amtrs_string_view _b)
{
	if (_a.len >= _b.len)
	{
		return	memcmp(_a.txt, _b.txt, _b.len) == 0;
	}
	return	amtrs_false;
}


//! _a が _b で終わるか調べる。
inline extern amtrs_bool amtrs_string_view_ends_with(amtrs_string_view _a, amtrs_string_view _b)
{
	if (_a.len >= _b.len)
	{
		return	memcmp(_a.txt + _a.len - _b.len, _b.txt, _b.len) == 0;
	}
	return	amtrs_false;
}

//! _a の最後尾から _b が現れる位置を検索する。
inline extern const char* amtrs_string_view_rfind(amtrs_string_view _a, amtrs_string_view _b)
{
	if (_a.len >= _b.len)
	{
		amtrs_string_view	work;
		work.txt	= _a.txt + _a.len - _b.len;
		work.len	= _b.len;
		for (;;)
		{
			if (amtrs_string_view_equals(work, _b))
			{
				return	work.txt;
			}
			if (work.txt == _a.txt)
			{
				break;
			}
			--work.txt;
		}
	}
	return	amtrs_null;
}


#ifdef	__cplusplus
inline _amtrs_string_view _amtrs_string_view::substr(size_t _pos) const noexcept
{
	return	amtrs_string_view_substr(*this, _pos, this->len);
}

inline _amtrs_string_view _amtrs_string_view::substr(size_t _pos, size_t _len) const noexcept
{
	return	amtrs_string_view_substr(*this, _pos, _len);
}
#endif

AMTRS_EXTERN_C_END
#ifdef	_MSC_VER
#pragma warning(pop)
#endif
#endif

/*
 * Copyright (c) 2019 Isao Shibuya (isapon)
 * Released under the MIT license
 * https://opensource.org/licenses/mit-license.php
 */
#ifndef	__passion__string_view__h
#define	__passion__string_view__h
#include <string.h>
#ifdef	_MSC_VER
#pragma	warning(push)
#pragma	warning(disable:4190)
#endif
PASSION_EXTERN_C_BEGIN

/*
 * std::string_view like.
 */
struct	passion_string_view
{
	const char*		_begin;
	size_t			_size;

	#ifdef	__cplusplus
	inline passion_string_view() = default;
	constexpr passion_string_view(const passion_string_view&) = default;
	constexpr passion_string_view(const char* _beg, std::size_t _sz) : _begin(_beg), _size(_sz) {}
	constexpr passion_string_view(std::string_view _v) : _begin(_v.data()), _size(_v.size()) {}
	~passion_string_view() = default;
	constexpr passion_string_view(const char* _beg) : _begin(_beg), _size(_beg ? strlen(_beg) : 0) {}
	constexpr passion_string_view& operator = (const passion_string_view&) = default;
	constexpr passion_string_view& operator = (passion_string_view&&) = default;
	constexpr std::size_t size() const noexcept { return _size; }
	constexpr std::size_t length() const noexcept { return _size; }
	constexpr bool empty() const noexcept { return size() == 0; }
	constexpr auto begin() noexcept { return _begin; }
	constexpr auto end() noexcept { return _begin + _size; }
	constexpr auto begin() const noexcept { return _begin; }
	constexpr auto end() const noexcept { return _begin + _size; }
	constexpr auto cbegin() const noexcept { return _begin; }
	constexpr auto cend() const noexcept { return _begin + _size; }
	constexpr const char* data() const noexcept { return _begin; }
	constexpr operator std::string_view () const noexcept { return {_begin, _size}; }
	void swap(passion_string_view& b) noexcept { std::swap(_begin, b._begin); std::swap(_size, b._size); }
	constexpr bool operator == (const passion_string_view& _b) const noexcept { return _size == _b._size && memcmp(_begin, _b._begin, _size) == 0; }
	constexpr bool operator != (const passion_string_view& _b) const noexcept { return !(*this == _b); }
	#endif	//cplusplus
};


inline extern struct passion_string_view passion_string_view_make(const char* _begin)
{
	struct passion_string_view	retval;
	retval._begin	= _begin;
	retval._size	= strlen(_begin);
	return	retval;
}


inline extern passion_bool passion_string_view_equals(struct passion_string_view _a, struct passion_string_view _b)
{
	size_t		i;
	size_t		s;
	const char*	a;
	const char*	b;
	if (_a._size != _b._size)
	{
		return	passion_false;
	}
	a	= _a._begin;
	b	= _b._begin;
	s	= _a._size / sizeof(void*);
	if (s > 0)
	{
		for (i = 0; i < s; ++i)
		{
			if (((const void**)a)[i] != ((const void**)b)[i])
			{
				return	passion_false;
			}
		}
		a	+= s * sizeof(void*);
		b	+= s * sizeof(void*);
	}
	s	= _a._size % sizeof(void*);
	for (i = 0; i < s; ++i)
	{
		if (((const int8_t*)a)[i] != ((const int8_t*)b)[i])
		{
			return	passion_false;
		}
	}
	return	passion_true;
}


PASSION_EXTERN_C_END
#ifdef	_MSC_VER
#pragma	warning(pop)
#endif
#endif

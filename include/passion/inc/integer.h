/*
 * Copyright (c) 2019 Isao Shibuya (isapon)
 * Released under the MIT license
 * https://opensource.org/licenses/mit-license.php
 */
#ifndef	__passion__integer__h
#define	__passion__integer__h
#include <assert.h>
#ifdef	__cplusplus
#include <amtrs/string/constring.hpp>
#endif

/*
 * passion virtual machine's integer.
 */
struct	passion_int
{
#ifdef	__cplusplus
	static constexpr auto signature() { return amtrs::make_constring("int"); }

	passion_int() = default;
	passion_int(const passion_int&) = default;
	passion_int(passion_int&&) = default;
	passion_int(std::intmax_t _) noexcept : v(_) {}
	passion_int& operator = (std::intmax_t _) noexcept { v = _; return *this; }
	passion_int& operator = (const passion_int&) = default;
	passion_int& operator = (passion_int&&) = default;
	operator std::intmax_t () const noexcept { return v; }

	template<class T> passion_int operator +  (T b) { return v + b; }
	template<class T> passion_int operator -  (T b) { return v - b; }
	template<class T> passion_int operator *  (T b) { return v * b; }
	template<class T> passion_int operator /  (T b) { return v / b; }
	template<class T> passion_int operator == (T b) { return v == b; }
	template<class T> passion_int operator != (T b) { return v != b; }
	template<class T> passion_int operator << (T b) { return v << b; }
	template<class T> passion_int operator >> (T b) { return v >> b; }
	inline passion_int operator +  (const passion_int& b) { return v +  (std::intmax_t)b; }
	inline passion_int operator -  (const passion_int& b) { return v -  (std::intmax_t)b; }
	inline passion_int operator *  (const passion_int& b) { return v *  (std::intmax_t)b; }
	inline passion_int operator /  (const passion_int& b) { return v /  (std::intmax_t)b; }
	inline passion_int operator == (const passion_int& b) { return v == (std::intmax_t)b; }
	inline passion_int operator != (const passion_int& b) { return v != (std::intmax_t)b; }
	inline passion_int operator << (const passion_int& b) { return v << (std::intmax_t)b; }
	inline passion_int operator >> (const passion_int& b) { return v >> (std::intmax_t)b; }


private:
#endif

	intmax_t	v;
};
static_assert(sizeof(struct passion_int) == sizeof(intmax_t), "");



#ifdef	__cplusplus
namespace psn {
struct	vmfunction;
using	vmobj		= passion_object;
using	vmint_t		= passion_int;
} // namespace psn
#endif	// __cplusplus

#endif

/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__graphics__def__hpp
#define	__libamtrs__graphics__def__hpp
#include "../amtrs.hpp"
#include "../logging/@"
AMTRS_NAMESPACE_BEGIN

constexpr float	pi = 3.14159265358979323846264338327950288f;

inline double degrees_to_radians(double _degrees)
{
	return	pi * (_degrees / 180.0);
}

template<typename Type>
inline float degrees_to_radians(Type _degrees)
{
	return	static_cast<float>(degrees_to_radians(static_cast<double>(_degrees)));
}

inline double radians_to_degrees(double _radians)
{
	return	180 * (_radians / pi);
}

template<typename Type>
inline float radians_to_degrees(Type _radians)
{
	return	static_cast<float>(radians_to_degrees(static_cast<double>(_radians)));
}

AMTRS_NAMESPACE_END
#endif

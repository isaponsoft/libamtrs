﻿/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__geometry__hpp
#define	__libamtrs__geometry__hpp
#include "amtrs.hpp"
#include <cfloat>
#include <cmath>


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

// keep order.
#include ".inc-geometory/geometry-vec.hpp"
#include ".inc-geometory/geometry-matrix.hpp"
#include ".inc-geometory/geometry-size.hpp"
#include ".inc-geometory/geometry-rect.hpp"
#include ".inc-geometory/geometry-aabb.hpp"
#include ".inc-geometory/geometry-obb.hpp"
#include ".inc-geometory/geometry-box.hpp"
#include ".inc-geometory/geometry-line_segment.hpp"
#include ".inc-geometory/geometry-collision.hpp"
#include ".inc-geometory/geometry-distance.hpp"
#include ".inc-geometory/geometry-quaternion.hpp"
#include ".inc-geometory/geometry-transform.hpp"
#endif

/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__graphics__collision__hpp
#define	__libamtrs__graphics__collision__hpp
AMTRS_NAMESPACE_BEGIN


// ==============================================================
//! aabb3 & aabb3
// --------------------------------------------------------------
template<class T>
constexpr bool collision(basic_aabb<vec3<T>> _a, basic_aabb<vec3<T>> _b, bool _normalize = true) noexcept
{
	if (_normalize)
	{
		_a	= _a.normalize();
		_b	= _b.normalize();
	}
	if (_a.p1.x > _b.p2.x)	return	false;
	if (_a.p2.x < _b.p1.x)	return	false;
	if (_a.p1.y > _b.p2.y)	return	false;
	if (_a.p2.y < _b.p1.y)	return	false;
	if (_a.p1.z > _b.p2.z)	return	false;
	if (_a.p2.z < _b.p1.z)	return	false;
	return	true;
}


// ==============================================================
//! line_segment3 & line_segment3
// --------------------------------------------------------------
//! _eps
//!		許容する誤差
// --------------------------------------------------------------
template<class T>
constexpr bool collision(basic_line_segment<vec3<T>> const& _a, basic_line_segment<vec3<T>> const& _b, T _eps = FLT_EPSILON) noexcept
{
	auto	crs12	= cross((_a.p2 - _a.p1), (_b.p2 - _b.p1));
	if (crs12 == 0.0f)
	{
		return	false;		// 平行
	}
	auto			v		= _b.p1 - _a.p1;
	auto			crs1	= cross(v, (_a.p2 - _a.p1));
	auto			crs2	= cross(v, (_b.p2 - _b.p1));
	auto			t1		= crs2 / crs12;
	auto			t2		= crs1 / crs12;
	if ((t1 + _eps) < 0 || (t1 - _eps) > 1 || (t2 + _eps) < 0 || (t2 - _eps) > 1)
	{
		return	false;
	}
	return	true;
}


// ==============================================================
//! obb と obb の接触を調べる
// --------------------------------------------------------------
template<class T, class Grater = std::greater<T>>
constexpr bool intersects(basic_obb<T> const& _a, basic_obb<T> const& _b, T _epsilon = FLT_EPSILON) noexcept
{
	// = abs(dot(axis, plane))
	auto	_dot	= [](auto _axis, auto _pln) { return std::abs(dot(_axis, _pln)); };

	vec3<T>	ds	= _b.c - _a.c;		// distance
	vec3<T>	ax	= _a.u[0] * _a.e.x;	// a: x line segments
	vec3<T>	ay	= _a.u[1] * _a.e.y;	// a: y line segments
	vec3<T>	az	= _a.u[2] * _a.e.z;	// a: z line segments
	vec3<T>	bx	= _b.u[0] * _b.e.x;	// b: x line segments
	vec3<T>	by	= _b.u[1] * _b.e.y;	// b: y line segments
	vec3<T>	bz	= _b.u[2] * _b.e.z;	// b: z line segments

	// intersepts separated plane.
	auto	isp	= [=](auto p)
	{
		auto	l	= _dot(ds, p);
		auto	a	= _dot(ax, p) + _dot(ay, p) + _dot(az, p);
		auto	b	= _dot(bx, p) + _dot(by, p) + _dot(bz, p);
		return	Grater{}(l, a + b + _epsilon);
	};

	if (isp(_a.u[0]) || isp(_a.u[1]) || isp(_a.u[2]))	return	false;
	if (isp(_b.u[0]) || isp(_b.u[1]) || isp(_b.u[2]))	return	false;

	if (isp(cross(_a.u[0], _b.u[0])) || isp(cross(_a.u[0], _b.u[1])) || isp(cross(_a.u[0], _b.u[2])))	return	false;
	if (isp(cross(_a.u[1], _b.u[0])) || isp(cross(_a.u[1], _b.u[1])) || isp(cross(_a.u[1], _b.u[2])))	return	false;
	if (isp(cross(_a.u[2], _b.u[0])) || isp(cross(_a.u[2], _b.u[1])) || isp(cross(_a.u[2], _b.u[2])))	return	false;

	return	true;
}




AMTRS_NAMESPACE_END
#endif

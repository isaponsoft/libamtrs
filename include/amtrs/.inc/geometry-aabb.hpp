/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__graphics__aabb__hpp
#define	__libamtrs__graphics__aabb__hpp
AMTRS_NAMESPACE_BEGIN


//! Axis Aligned Bounding Box : 軸に沿ったバウンディングボックス
template<class VecT>
struct	basic_aabb
{
	using	vector_type	= VecT;

	vector_type	p1;
	vector_type	p2;

	basic_aabb() = default;
	basic_aabb(basic_aabb const&) = default;
	basic_aabb& operator = (basic_aabb const&) = default;

	constexpr basic_aabb(vector_type o, vector_type e)
		: p1(o)
		, p2(e)
	{}

	constexpr void normalize();
};


template<class VecT>
constexpr void basic_aabb<VecT>::normalize()
{
	if (p1.x > p2.x)	std::swap(p1.x, p2.x);
	if (p1.y > p2.y)	std::swap(p1.y, p2.y);
	if constexpr (VecT::col_size >= 3)
	{
		if (p1.z > p2.z)	std::swap(p1.z, p2.z);
	}
}


AMTRS_NAMESPACE_END
#endif

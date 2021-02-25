/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__graphics__obb__hpp
#define	__libamtrs__graphics__obb__hpp
AMTRS_NAMESPACE_BEGIN



//! Oriented Bounding Box : 有向境界ボックス
template<class T>
struct	basic_obb
{
	using	value_type	= T;
	using	vector_type	= vec3<T>;
	using	size_type	= size3<T>;

	vector_type	c;				//!< 中心位置
	vector_type	u[3];			//!< 方向ベクトル
	vector_type	e;				//!< 長さ（半径）

	constexpr basic_obb() = default;
	constexpr basic_obb(basic_obb const&) = default;
	constexpr basic_obb& operator = (basic_obb const&) = default;

	constexpr basic_obb(vector_type _pos, size_type _range);
	constexpr basic_obb(vector_type _pos, size_type _range, mat33<T> const& _mat);
	constexpr basic_obb(vector_type _pos, size_type _range, mat44<T> const& _mat);


#if	0
	auto closest_pt_point(vector_type& _q, vector_type const& _p, basic_obb const& _b) const noexcept -> vector_type;

	// ==============================================================
	//! 線分との衝突判定を行います。
	// --------------------------------------------------------------
	//! 衝突している場合は true を返します。
	// --------------------------------------------------------------
	bool collision_line_segment(vector_type _pos1, vector_type _pos2) const noexcept
	{
		vector_type		m = (_pos1 + _pos2) * 0.5f;
		vector_type		d = _pos2 - m;
		m	= m - c;
		m	= vector_type(dot(u[0], m), dot(u[1], m), dot(u[2], m));
		d	= vector_type(dot(u[0], d), dot(u[1], d), dot(u[2], d));

		auto	adx = std::abs(d.x);
		if (std::abs(m.x) > ob_b.e.x + adx)
		{
			return	false;
		}
		auto	ady = std::abs(d.y);
		if (std::abs(m.y) > ob_b.e.y + ady)
		{
			return	false;
		}
		auto	adz = std::abs(d.z);
		if (std::abs(m.z) > ob_b.e.z + adz)
		{
			return	false;
		}

		adx	+= 1.175494e-37; 
		ady	+= 1.175494e-37; 
		adz	+= 1.175494e-37;
		if (std::abs(m.y * d.z - m.z * d.y) > ob_b.e.y * adz + ob_b.e.z * ady
		 || std::abs(m.z * d.x - m.x * d.z) > ob_b.e.x * adz + ob_b.e.z * adx
		 || std::abs(m.x * d.y - m.y * d.x) > ob_b.e.x * ady + ob_b.e.y * adx)
		{
			return	false;
		}
		return	true;
	}
#endif
};


template<class T>
constexpr basic_obb<T>::basic_obb(vector_type _pos, size_type _range)
	: basic_obb(_pos, _range, mat44<T>::identify())
{}


template<class T>
constexpr basic_obb<T>::basic_obb(vector_type _pos, size_type _range, mat33<T> const& _mat)
	: c(_pos)
	, u{{_mat._11, _mat._12, _mat._13}, {_mat._21, _mat._22, _mat._23}, {_mat._31, _mat._32, _mat._33}}
	, e(_range)
{}


template<class T>
constexpr basic_obb<T>::basic_obb(vector_type _pos, size_type _range, mat44<T> const& _mat)
	: basic_obb
		( _pos + vector_type(_mat._41, _mat._42, _mat._43)
		, _range
		, mat33<float>(_mat._11, _mat._12, _mat._13, _mat._21, _mat._22, _mat._23, _mat._31, _mat._32, _mat._33)
		)
{}


#if	0
template<class T>
auto basic_obb<T>::closest_pt_point(vector_type& _q, vector_type const& _p, basic_obb const& _b) const noexcept -> vector_type
{
	vector_type	d	= _p - _b.c;
	vector_type	q	= _b.c;
	for (int i = 0; i < 3; +i)
	{
		value_type	dist	= dot(d, _b.u[i]);
		if (dist > _b.e[i])
		{
			dist = _b.e[i];
		}
		if (dist < -_b.e[i])
		{
			dist = -_b.e[i];
		}
		q	+= _b.u[i] * dist;
	}
	return	q;
}
#endif

AMTRS_NAMESPACE_END
#endif

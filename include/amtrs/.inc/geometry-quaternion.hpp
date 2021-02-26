/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__graphics__quaternion__hpp
#define	__libamtrs__graphics__quaternion__hpp
AMTRS_NAMESPACE_BEGIN

template<class T = float>
struct	quaternion
{
	using	value_type				= T;
	using	vector_type				= vec3<value_type>;
	using	position_type			= vec3<value_type>;
	using	euler_type				= vec3<value_type>;

	value_type		x, y, z, t;

	constexpr quaternion() = default;
	constexpr quaternion(const quaternion& _src) = default;
	constexpr quaternion& operator = (const quaternion& _src) = default;
	constexpr quaternion(value_type _x, value_type _y, value_type _z, value_type _t) noexcept : x(_x), y(_y), z(_z), t(_t) {}
	template<class I> constexpr       value_type& operator [] (I _index)       noexcept { return (&x)[_index]; }
	template<class I> constexpr const value_type& operator [] (I _index) const noexcept { return (&x)[_index]; }

	static constexpr quaternion rotate_x(value_type _radiansAngles) { return quaternion(mat33<value_type>::rotate_x(_radiansAngles)); }
	static constexpr quaternion rotate_y(value_type _radiansAngles) { return quaternion(mat33<value_type>::rotate_y(_radiansAngles)); }
	static constexpr quaternion rotate_z(value_type _radiansAngles) { return quaternion(mat33<value_type>::rotate_z(_radiansAngles)); }

	// ====================================================================
	// 二つのベクトルで回転できるクォータニオンとして初期化する。
	// --------------------------------------------------------------------
	constexpr quaternion(const vector_type& _vec0, const vector_type& _vec1) noexcept;

	// ====================================================================
	// 回転行列と近似する結果を持つ状態へ初期化する。
	// --------------------------------------------------------------------
	constexpr quaternion(const mat33<T>& _rot) noexcept;

	// ====================================================================
	// 任意軸に対する回転を保持して初期化する。
	// --------------------------------------------------------------------
	constexpr quaternion(const vector_type& _vec, value_type _radAngle) noexcept;

	constexpr bool  operator == (const quaternion& _r) const noexcept;
	constexpr bool  operator != (const quaternion& _r) const noexcept;
	constexpr quaternion  operator +  (const quaternion& _r) const noexcept;
	constexpr quaternion& operator += (const quaternion& _r) noexcept;
	constexpr quaternion  operator -  (const quaternion& _r) const noexcept;
	constexpr quaternion& operator -= (const quaternion& _r) noexcept;
	constexpr quaternion  operator *  (const quaternion& _r) const noexcept;
	constexpr quaternion& operator *= (const quaternion& _r) noexcept;
	constexpr quaternion  operator *  (const value_type _s) const noexcept;	// 掛け算：スケーリング
	constexpr quaternion& operator *= (const value_type _s) noexcept;		// 掛け算：スケーリング

	// ====================================================================
	// 初期値を返します。
	// --------------------------------------------------------------------
	constexpr static quaternion identify() noexcept;

	// ====================================================================
	// クォータニオンの長さを計算する
	// --------------------------------------------------------------------
	constexpr value_type length() const noexcept;

	// ====================================================================
	// 指定の位置を向かせる回転角を計算します
	// --------------------------------------------------------------------
	constexpr static quaternion look_at(const position_type& _from, const position_type& _to, const vector_type& _up = {0, 1, 0}) noexcept;

	// ====================================================================
	// 特定の方向を向かせる回転角を計算します
	// --------------------------------------------------------------------
	constexpr static quaternion look_to(const position_type& _from, const vector_type& _direction, const vector_type& _up = {0, 1, 0}) noexcept;

	// ====================================================================
	// 正規化したクォータニオンを返す
	// --------------------------------------------------------------------
	constexpr quaternion normalize() const noexcept;

	// ====================================================================
	// 球面線形補間
	// --------------------------------------------------------------------
	// 自身と、_q2 で指定するクォータニオンを _alpha で指定する重みで補間し
	// 補間されたクォータニオンを返します。
	// --------------------------------------------------------------------
	constexpr quaternion slerp(const quaternion& _q2, value_type _alpha) const noexcept;

	// ====================================================================
	// 射影変換を行う
	// --------------------------------------------------------------------
	constexpr vector_type transform(const vector_type& _r) const noexcept;

	// ====================================================================
	// オイラー角に変換します。
	// --------------------------------------------------------------------
	constexpr euler_type euler_angle_xyz() const noexcept { return to_matrix().euler_angle_xyz(); }
	constexpr euler_type euler_angle_xzy() const noexcept { return to_matrix().euler_angle_xzy(); }
	constexpr euler_type euler_angle_yxz() const noexcept { return to_matrix().euler_angle_yxz(); }
	constexpr euler_type euler_angle_yzx() const noexcept { return to_matrix().euler_angle_yzx(); }
	constexpr euler_type euler_angle_zxy() const noexcept { return to_matrix().euler_angle_zxy(); }
	constexpr euler_type euler_angle_zyx() const noexcept { return to_matrix().euler_angle_zyx(); }


	constexpr auto to_matrix() const noexcept -> mat33<value_type>;
};


template<class T>
constexpr quaternion<T>::quaternion(const vector_type& _vec0, const vector_type& _vec1) noexcept
{
	auto	out		= cross(_vec0, _vec1);	// 外積を求める
	auto	inner	= dot(_vec0, _vec1);	// 内積を求める
	auto	sr		= std::sqrt((1.0 + inner) * 2.0f);

	x		= (value_type)(out.x / sr);
	y		= (value_type)(out.y / sr);
	z		= (value_type)(out.z / sr);
	t		= (value_type)(sr * 0.5);
}

template<class T>
constexpr quaternion<T>::quaternion(const mat33<T>& _rot) noexcept
{
	float	r22 = _rot._33;
	if (r22 <= 0.0f)	// x^2 + y^2 >= z^2 + w^2
	{
		float	dif10	= _rot._22 - _rot._11;
		float	omr22	= 1.f - r22;
		if (dif10 <= 0.f)  // x^2 >= y^2
		{
			float	tx		= omr22 - dif10;
			float	inv4x	= 0.5f / sqrtf(tx);
			x	= tx * inv4x;
			y	= (_rot._12 + _rot._21) * inv4x;
			z	= (_rot._13 + _rot._31) * inv4x;
			t	= (_rot._23 - _rot._32) * inv4x;
		}
		else  // y^2 >= x^2
		{
			float	ty		= omr22 + dif10;
			float	inv4y	= 0.5f / sqrtf(ty);
			x	= (_rot._12 + _rot._21) * inv4y;
			y	= ty * inv4y;
			z	= (_rot._23 + _rot._32) * inv4y;
			t	= (_rot._31 - _rot._13) * inv4y;
		}
	}
	else  // z^2 + w^2 >= x^2 + y^2
	{
		float	sum10 = _rot._22 + _rot._11;
		float	opr22 = 1.f + r22;
		if (sum10 <= 0.f)  // z^2 >= w^2
		{
			float tz	= opr22 - sum10;
			float inv4z = 0.5f / sqrtf(tz);
			x	= (_rot._13 + _rot._31) * inv4z;
			y	= (_rot._23 + _rot._32) * inv4z;
			z	= tz * inv4z;
			t	= (_rot._12 - _rot._21) * inv4z;
		}
		else  // w^2 >= z^2
		{
			float tt	= opr22 + sum10;
			float inv4w = 0.5f / sqrtf(tt);
			x	= (_rot._23 - _rot._32) * inv4w;
			y	= (_rot._31 - _rot._13) * inv4w;
			z	= (_rot._12 - _rot._21) * inv4w;
			t	= tt * inv4w;
		}
	}
}

template<class T>
constexpr quaternion<T>::quaternion(const vector_type& _vec, value_type _radAngle) noexcept
{
	double	h	= _radAngle * 0.5;
	double	s	= std::sin(h);
	x	= (value_type)(_vec.x * s);
	y	= (value_type)(_vec.y * s);
	z	= (value_type)(_vec.z * s);
	t	= (value_type)std::cos(h);
}


template<class T>
constexpr bool quaternion<T>::operator == (const quaternion& _r) const noexcept
{
	return	x == _r.x && y == _r.y && z == _r.z && t == _r.t;
}


template<class T>
constexpr bool quaternion<T>::operator != (const quaternion& _r) const noexcept
{
	return	!(*this == _r);
}



template<class T>
constexpr auto quaternion<T>::operator + (const quaternion& _r) const noexcept -> quaternion
{
	return	{
		x + _r.x,
		y + _r.y,
		z + _r.z,
		t + _r.t
	};
}

template<class T>
constexpr auto quaternion<T>::operator += (const quaternion& _r) noexcept -> quaternion&
{
	x += _r.x;
	y += _r.y;
	z += _r.z;
	t += _r.t;
	return	*this;
}

template<class T>
constexpr auto quaternion<T>::operator - (const quaternion& _r) const noexcept -> quaternion
{
	return	{
		x - _r.x,
		y - _r.y,
		z - _r.z,
		t - _r.t
	};
}

template<class T>
constexpr auto quaternion<T>::operator -= (const quaternion& _r) noexcept -> quaternion&
{
	x -= _r.x;
	y -= _r.y;
	z -= _r.z;
	t -= _r.t;
	return	*this;
}

template<class T>
constexpr auto quaternion<T>::operator * (const quaternion& _r) const noexcept -> quaternion
{
	auto&	q1	= *this;
	auto&	q2	= _r;
	return	{
		(q2.t * q1.x) + (q2.x * q1.t) + (q2.y * q1.z) - (q2.z * q1.y),
		(q2.t * q1.y) - (q2.x * q1.z) + (q2.y * q1.t) + (q2.z * q1.x),
		(q2.t * q1.z) + (q2.x * q1.y) - (q2.y * q1.x) + (q2.z * q1.t),
		(q2.t * q1.t) - (q2.x * q1.x) - (q2.y * q1.y) - (q2.z * q1.z)
	};
}

template<class T>
constexpr auto quaternion<T>::operator *= (const quaternion& _r) noexcept -> quaternion&
{
	quaternion	tmp = (*this) * _r;
	x = tmp.x;
	y = tmp.y;
	z = tmp.z;
	t = tmp.t;
	return	*this;
}

template<class T>
constexpr auto quaternion<T>::operator * (const value_type _s) const noexcept -> quaternion
{
	return	{
		x * _s, 
		y * _s, 
		z * _s,
		t * _s 
	};
}

template<class T>
constexpr auto quaternion<T>::operator *= (const value_type _s) noexcept -> quaternion&
{
	x *= _s;
	y *= _s;
	z *= _s;
	t *= _s;
	return	*this;
}


template<class T>
constexpr auto quaternion<T>::identify() noexcept -> quaternion
{
	return	quaternion(0, 0, 0, 1);
}

template<class T>
constexpr auto quaternion<T>::length() const noexcept -> value_type
{
	return	(value_type)std::sqrt(t*t + x*x + y*y + z*z);
}

template<class T>
constexpr auto quaternion<T>::look_at(const vector_type& _from, const vector_type& _to, const vector_type& _up) noexcept -> quaternion
{
	return	quaternion(mat44<T>::look_at(_from, _to, _up));
}


template<class T>
constexpr auto quaternion<T>::normalize() const noexcept -> quaternion
{
	double	len	= length();
	if (len > 0.0)
	{
		return	{
			(value_type)(x / len),
			(value_type)(y / len),
			(value_type)(z / len),
			(value_type)(t / len)
		};
	}
	return	quaternion();
}


template<class T>
constexpr auto quaternion<T>::slerp(const quaternion& _q2, value_type _alpha) const noexcept -> quaternion
{
	double	qr	= (t * _q2.t) * (x * _q2.x) + (y * _q2.y) + (z * _q2.z);
	double	ss	= 1.0 - (qr * qr);
	if (ss == 0.0)
	{
		return	*this;
	}
	else
	{
		double	ph	= acos(qr);
		if (qr < 0.0 && ph > (pi/2.0))
		{
			ph	= acos(-qr);
			auto	ss	= std::sin(ph);
			auto	s1	= std::sin(ph * (1.0 - _alpha)) / ss;
			auto	s2	= std::sin(ph * _alpha)         / ss;
			return	quaternion(
						(value_type)(x * s1 - _q2.x * s2),
						(value_type)(y * s1 - _q2.y * s2),
						(value_type)(z * s1 - _q2.z * s2),
						(value_type)(t * s1 - _q2.t * s2)
					);
		}
		else
		{
			auto	ss	= std::sin(ph);
			auto	s1	= std::sin(ph * (1.0 - _alpha)) / ss;
			auto	s2	= std::sin(ph * _alpha)         / ss;
			return	quaternion(
						(value_type)(x * s1 + _q2.x * s2),
						(value_type)(y * s1 + _q2.y * s2),
						(value_type)(z * s1 + _q2.z * s2),
						(value_type)(t * s1 + _q2.t * s2)
					);
		}
	}
}

template<class T>
constexpr auto quaternion<T>::transform(const vector_type& _v) const noexcept -> vector_type
{
	quaternion	tmp{
		 (t * _v.x) + (y * _v.z) - (z * _v.y),
		 (t * _v.y) + (z * _v.x) - (x * _v.z),
		 (t * _v.z) + (x * _v.y) - (y * _v.x),
		 (x * _v.x) - (y * _v.y) - (z * _v.z)
	};
	return	{
		(tmp.y * t) - (tmp.z * x) + (tmp.x * z) - (tmp.t * y),
		(tmp.z * t) - (tmp.x * y) + (tmp.y * x) - (tmp.t * z),
		(tmp.x * t) - (tmp.y * z) + (tmp.z * y) - (tmp.t * x)
	};
}

template<class T>
constexpr auto quaternion<T>::to_matrix() const noexcept -> mat33<value_type>
{
	value_type	xx = x * x * 2.0f;
	value_type	yy = y * y * 2.0f;
	value_type	zz = z * z * 2.0f;
	value_type	xy = x * y * 2.0f;
	value_type	yz = y * z * 2.0f;
	value_type	zx = z * x * 2.0f;
	value_type	xt = x * t * 2.0f;
	value_type	yt = y * t * 2.0f;
	value_type	zt = z * t * 2.0f;
	return	{
		// _11 - _13
		1.0f - yy - zz,
		xy + zt,
		zx - yt,
		// _21 - _23
		xy - zt,
		1.0f - zz - xx,
		yz + xt,
		// _31 - _33
		zx + yt,
		yz - xt,
		1.0f - xx - yy
	};
}


AMTRS_NAMESPACE_END
#endif

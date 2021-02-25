/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__graphics__matrix__hpp
#define	__libamtrs__graphics__matrix__hpp
AMTRS_NAMESPACE_BEGIN
#define		l	,		// 行列を見やすくするための無駄なマクロ


template<std::size_t RowSize, std::size_t ColSize>
struct	matrix_tag
{
	static constexpr std::size_t	row_size	= RowSize;
	static constexpr std::size_t	col_size	= ColSize;
	using	vector_tag_type	= vector_tag<col_size>;
};


template<class T, class Tag, class MatrixT>
struct	_matrix_impliments;


template<class LVT, class LTag, class LCT, class RVT, class RTag, class RCT>
constexpr auto operator * (const _vector_impliments<LVT, LTag, LCT>& _vec, const _matrix_impliments<RVT, RTag, RCT>& _mtx) noexcept
{
	using	vec_type	= typename _vector_impliments<LVT, LTag, LCT>::vector_type;
	using	mat_type	= typename _matrix_impliments<RVT, RTag, RCT>::matrix_type;
	vec_type	retval	= vec_type::zero();
	for (int c = 0; c < vec_type::col_size; ++c)
	{
		for (int r = 0; r < mat_type::row_size; ++r)
		{
			if (r < vec_type::col_size)
			{
				retval[c] += _vec[r] * _mtx(r, c);
			}
			else
			{
				retval[c] += _mtx(r, c);
			}
		}
	}
	return	retval;
}
#if 0
template<class LVT, class LTag, class LCT, class RVT, class RTag, class RCT>
auto operator * (const _vector_impliments<LVT, LTag, LCT>& _vec, const _matrix_impliments<RVT, RTag, RCT>& _mtx) noexcept
{
	using	vec_type	= typename _vector_impliments<LVT, LTag, LCT>::vector_type;
	using	mat_type	= typename _matrix_impliments<RVT, RTag, RCT>::matrix_type;
	vec_type	retval	= vec_type::zero();
	for (int c = 0; c < vec_type::col_size; ++c)
	{
		for (int r = 0; r < mat_type::row_size; ++r)
		{
			if constexpr (vec_type::col_size < mat_type::row_size)
			{
				retval[c] += _mtx(r, c);
			}
			else
			{
				retval[c] += _vec[r] * _mtx(r, c);
			}
		}
	}
	return	retval;
}
#endif

template<class T>
struct	mat33 : _matrix_impliments<T, matrix_tag<3, 3>, mat33<T>>
{
	using	value_type	= T;
	using	_impl_type	= _matrix_impliments<T, matrix_tag<3, 3>, mat33<T>>;

	value_type		_11, _12, _13;
	value_type		_21, _22, _23;
	value_type		_31, _32, _33;

	using	_impl_type::operator [];
	using	_impl_type::operator ();
	using	_impl_type::operator *;
	using	_impl_type::operator *=;
	

	constexpr mat33() = default;
	constexpr mat33(const mat33&) = default;
	constexpr mat33& operator = (const mat33&) = default;

	constexpr mat33(
		value_type _v11, value_type _v12, value_type _v13,
		value_type _v21, value_type _v22, value_type _v23,
		value_type _v31, value_type _v32, value_type _v33) noexcept;


	// ====================================================================
	// 拡大縮小を生成する。
	// --------------------------------------------------------------------
	static constexpr mat33 scale(const value_type _s) noexcept;
	static constexpr mat33 scale(const vec3<T>& _s) noexcept;
	static constexpr mat33 scale(const value_type _sx, const value_type _sy, const value_type _sz) noexcept;
};


template<class T = float>
struct	mat44 : _matrix_impliments<T, matrix_tag<4, 4>, mat44<T>>
{
	using	value_type	= T;
	using	_impl_type	= _matrix_impliments<T, matrix_tag<4, 4>, mat44<T>>;

	value_type		_11, _12, _13, _14;
	value_type		_21, _22, _23, _24;
	value_type		_31, _32, _33, _34;
	value_type		_41, _42, _43, _44;

	using	_impl_type::operator [];
	using	_impl_type::operator ();
	using	_impl_type::operator *;
	using	_impl_type::operator *=;
	

	constexpr mat44() = default;
	constexpr mat44(const mat44&) = default;
	constexpr mat44& operator = (const mat44&) = default;

	constexpr mat44(
		value_type _v11, value_type _v12, value_type _v13, value_type _v14,
		value_type _v21, value_type _v22, value_type _v23, value_type _v24,
		value_type _v31, value_type _v32, value_type _v33, value_type _v34,
		value_type _v41, value_type _v42, value_type _v43, value_type _v44) noexcept;

	constexpr mat44(const mat33<value_type>& _m) noexcept
		: mat44(_m._11, _m._12, _m._13, 0,
		        _m._21, _m._22, _m._23, 0,
		        _m._31, _m._32, _m._33, 0,
		        0,      0,      0,      1)
	{}

	//! ====================================================================
	//! 移動成分を計算して取り出す。
	//! --------------------------------------------------------------------
	constexpr vec3<value_type> translate() const noexcept
	{
		return	{ _41, _42, _43 };
	}

	static constexpr mat44 translate(vec3<value_type> _pos) noexcept
	{
		mat44	retval	= _impl_type::identify();
		retval._41	= _pos.x;
		retval._42	= _pos.y;
		retval._43	= _pos.z;
		retval._44	= 1.0f;
		return	retval;
	}

	static constexpr mat44 translate(value_type _x, value_type _y, value_type _z) noexcept
	{
		mat44	retval	= _impl_type::identify();
		retval._41	= _x;
		retval._42	= _y;
		retval._43	= _z;
		retval._44	= 1.0f;
		return	retval;
	}

	//! ====================================================================
	//! 移動成分を丸めます
	//! --------------------------------------------------------------------
	constexpr mat44& translate_round()
	{
		_41	= std::round(_41);
		_42	= std::round(_42);
		_43	= std::round(_43);
		return	*this;
	}

	//! ====================================================================
	//! スケール成分を計算して取り出す。
	//! --------------------------------------------------------------------
	constexpr vec3<value_type> scale() const noexcept
	{
		return	{
			vec3<float>(_11 * _11, _11 * _12, _11 * _13).length(),
			vec3<float>(_12 * _21, _12 * _22, _12 * _23).length(),
			vec3<float>(_13 * _31, _13 * _32, _13 * _33).length()
		};
	}


	static constexpr mat44 scale(vec3<value_type> _scale) noexcept
	{
		mat44	retval	= _impl_type::identify();
		retval._11	= _scale.x;
		retval._22	= _scale.y;
		retval._33	= _scale.z;
		retval._44	= 1;
		return	retval;
	}

	static constexpr mat44 scale(vec4<value_type> _scale) noexcept
	{
		mat44	retval	= _impl_type::identify();
		retval._11	= _scale.x;
		retval._22	= _scale.y;
		retval._33	= _scale.z;
		retval._44	= _scale.w;
		return	retval;
	}

	static constexpr mat44 scale(value_type _x, value_type _y, value_type _z, value_type _w = 1.0f) noexcept
	{
		mat44	retval	= _impl_type::identify();
		retval._11	= _x;
		retval._22	= _y;
		retval._33	= _z;
		retval._44	= _w;
		return	retval;
	}

	//! ====================================================================
	//! 透視変換行列を生成する。
	//! --------------------------------------------------------------------
	//! _center にはビューポートの中央に相当する座標を指定する。スクリーン座標系と同じ
	//! にするのであれば、 _canvas_size.width/2, _canvas_size.height/2 を
	//! 設定すると良い。
	//! 例えば、 ortho({0,0}, {640,480}) であれば、 {0,0} にくるプリミティブが
	//! ビューポート中央に表示されることとなる。
	// --------------------------------------------------------------------
	static constexpr mat44 ortho(vec2<value_type> _center, size2<value_type> _canvas_size, value_type _near = 0, value_type _far = 1) noexcept
	{
		value_type	x		= _center.x;
		value_type	y		= _center.y;
		value_type	width	= _canvas_size.width;
		value_type	height	= _canvas_size.height;
		value_type	w		= value_type(2.0) / width;
		value_type	h		= value_type(2.0) / height;
		value_type	d		= 1.0f / (_far - _near);
		return	{
			 w       l   0      l  0  l 0, 
			 0       l   h      l  0  l 0,
			 0       l   0      l  d  l 0,
			-(x * w) l -(y * h) l  0  l 1
		};
	}

	static constexpr mat44 perspectiveRH(value_type _near, value_type _far, value_type _degreeFovY, value_type _aspect) noexcept
	{
		const value_type	fovy	= _aspect < 1.0f
									? degrees_to_radians(_degreeFovY) / _aspect
									: degrees_to_radians(_degreeFovY);
		const value_type	range	= _far / (_near - _far);
		const value_type	yscale	= (value_type)(1.0f / (tan(fovy / 2.0f)));
		const value_type	xscale	= yscale / _aspect;
		return	{
			xscale l 0       l 0             l  0,
			0      l yscale  l 0             l  0,
			0      l 0       l range         l -1,
			0      l 0       l _near * range l  0
		};
	}

	static constexpr mat44 perspectiveLH(value_type _near, value_type _far, value_type _degreeFovY, value_type _aspect) noexcept
	{
		const value_type	fovy	= _aspect < 1.0f
									? degrees_to_radians(_degreeFovY) / _aspect
									: degrees_to_radians(_degreeFovY);
		const value_type	yscale	= (value_type)(1.0f / (tan(fovy / 2.0f)));
		const value_type	xscale	= yscale / _aspect;
		const value_type	length	= _far - _near;
	#if 1
		// 手前が 0、奥が 1
		const value_type	range	= _far / length;
		return	{
			xscale l 0       l  0             l 0,
			0      l yscale  l  0             l 0,
			0      l 0       l  range         l 1,
			0      l 0       l -_near * range l 0
		};
	#else
		// 手前が 1、奥が 0
		return	{
			xscale l 0       l  0                              l 0,
			0      l yscale  l  0                              l 0,
			0      l 0       l  (_far + _near) / length        l 1,
			0      l 0       l -(2.0f * _far * _near) / length l 0
		};
	#endif
	}

};


template<class T>
constexpr mat33<T>::mat33(
	value_type _v11, value_type _v12, value_type _v13,
	value_type _v21, value_type _v22, value_type _v23,
	value_type _v31, value_type _v32, value_type _v33) noexcept
	: _11(_v11), _12(_v12), _13(_v13)
	, _21(_v21), _22(_v22), _23(_v23)
	, _31(_v31), _32(_v32), _33(_v33)
{}


template<class T>
constexpr mat44<T>::mat44(
	value_type _v11, value_type _v12, value_type _v13, value_type _v14,
	value_type _v21, value_type _v22, value_type _v23, value_type _v24,
	value_type _v31, value_type _v32, value_type _v33, value_type _v34,
	value_type _v41, value_type _v42, value_type _v43, value_type _v44) noexcept
	: _11(_v11), _12(_v12), _13(_v13), _14(_v14)
	, _21(_v21), _22(_v22), _23(_v23), _24(_v24)
	, _31(_v31), _32(_v32), _33(_v33), _34(_v34)
	, _41(_v41), _42(_v42), _43(_v43), _44(_v44)
{}




template<class T, class Tag, class MatrixT>
struct	_matrix_impliments : Tag
{
	using	value_type			= T;
	using	matrix_type			= MatrixT;
	using	euler_angle_type	= vec3<value_type>;
	using	iterator			= value_type*;
	using	const_iterator		= const value_type*;

	constexpr       value_type& operator [] (int _index)       noexcept { return mt()[_index]; }
	constexpr const value_type& operator [] (int _index) const noexcept { return mt()[_index]; }

	constexpr       value_type& operator () (int _row, int _col)       noexcept { return mt()[MatrixT::col_size * _row + _col]; }
	constexpr const value_type& operator () (int _row, int _col) const noexcept { return mt()[MatrixT::col_size * _row + _col]; }


	static constexpr MatrixT identify() noexcept
	{
		MatrixT	retval;
		for (int r = 0; r < MatrixT::row_size; ++r)
		{
			for (int c = 0; c < MatrixT::col_size; ++c)
			{
				retval(r, c) = (r == c) ? value_type(1) : value_type(0);
			}
		}
		return	retval;
	}


	constexpr MatrixT transpose() const noexcept
	{
		MatrixT	retval;
		for (int r = 0; r < MatrixT::row_size; ++r)
		{
			for (int c = 0; c < MatrixT::col_size; ++c)
			{
				retval(c, r) = (*this)(r, c);
			}
		}
		return	retval;
	}

	static constexpr MatrixT zero() noexcept
	{
		MatrixT	retval;
		for (int r = 0; r < MatrixT::row_size; ++r)
		{
			for (int c = 0; c < MatrixT::col_size; ++c)
			{
				retval(r, c) = 0;
			}
		}
		return	retval;
	}



	static constexpr MatrixT rotate_x(value_type _radAngle) noexcept
	{
		auto	S	= std::sin(_radAngle);
		auto	C	= std::cos(_radAngle);
		MatrixT	retval	= identify();
		retval(1,1)	=  C;
		retval(1,2)	=  S;
		retval(2,1)	= -S;
		retval(2,2)	=  C;
		return	retval;
	}

	static constexpr MatrixT rotate_y(value_type _radAngle) noexcept
	{
		auto	S	= std::sin(_radAngle);
		auto	C	= std::cos(_radAngle);
		MatrixT	retval	= identify();
		retval(0,0)	=  C;
		retval(0,2)	= -S;
		retval(2,0)	=  S;
		retval(2,2)	=  C;
		return	retval;
	}

	static constexpr MatrixT rotate_z(value_type _radAngle) noexcept
	{
		auto	S	= std::sin(_radAngle);
		auto	C	= std::cos(_radAngle);
		MatrixT	retval	= identify();
		retval(0,0)	=  C;
		retval(0,1)	= -S;
		retval(1,0)	=  S;
		retval(1,1)	=  C;
		return	retval;
	}

	static constexpr MatrixT rotate_xyz(const vec3<T>& _s) noexcept
	{
		return	rotate_x(_s.x) * rotate_y(_s.y) * rotate_z(_s.z);
	}

	static constexpr MatrixT rotate_xzy(const vec3<T>& _s) noexcept
	{
		return	rotate_x(_s.x) * rotate_z(_s.z) * rotate_y(_s.y);
	}

	static constexpr MatrixT rotate_yxz(const vec3<T>& _s) noexcept
	{
		return	rotate_y(_s.y) * rotate_x(_s.x) * rotate_z(_s.z);
	}

	static constexpr MatrixT rotate_yzx(const vec3<T>& _s) noexcept
	{
		return	rotate_y(_s.y) * rotate_z(_s.z) * rotate_x(_s.x);
	}

	static constexpr MatrixT rotate_zxy(const vec3<T>& _s) noexcept
	{
		return	rotate_z(_s.z) * rotate_x(_s.x) * rotate_y(_s.y);
	}

	static constexpr MatrixT rotate_zyx(const vec3<T>& _s) noexcept
	{
		return	rotate_z(_s.z) * rotate_y(_s.y) * rotate_x(_s.x);
	}

	static constexpr MatrixT rotate_xyz(const value_type _radX, const value_type _radY, const value_type _radZ) noexcept
	{
		return	rotate_xyz(vec3<T>(_radX, _radY, _radZ));
	}

	static constexpr MatrixT rotate_xzy(const value_type _radX, const value_type _radY, const value_type _radZ) noexcept
	{
		return	rotate_xzy(vec3<T>(_radX, _radY, _radZ));
	}

	static constexpr MatrixT rotate_yxz(const value_type _radX, const value_type _radY, const value_type _radZ) noexcept
	{
		return	rotate_yxz(vec3<T>(_radX, _radY, _radZ));
	}

	static constexpr MatrixT rotate_yzx(const value_type _radX, const value_type _radY, const value_type _radZ) noexcept
	{
		return	rotate_yzx(vec3<T>(_radX, _radY, _radZ));
	}

	static constexpr MatrixT rotate_zxy(const value_type _radX, const value_type _radY, const value_type _radZ) noexcept
	{
		return	rotate_zxy(vec3<T>(_radX, _radY, _radZ));
	}

	static constexpr MatrixT rotate_zyx(const value_type _radX, const value_type _radY, const value_type _radZ) noexcept
	{
		return	rotate_zyx(vec3<T>(_radX, _radY, _radZ));
	}

	template<class R>
	constexpr MatrixT operator * (R&& _rhs) const noexcept
	{
		return	mul(std::forward<typename std::remove_reference<R>::type>(_rhs));
	}

	template<class R>
	constexpr MatrixT& operator *= (R&& _rhs) noexcept
	{
		return	static_cast<MatrixT&>(*this) = *this * std::forward<R>(_rhs);
	}

	constexpr euler_angle_type euler_angle_xyz() noexcept
	{
		auto&	m(static_cast<MatrixT&>(*this));
		return	(std::abs(m._13) < 0.99999f)
				? euler_angle_type(std::atan2( m._23, m._33) l std::asin(-m._13)            l std::atan2( m._12, m._11))
				: euler_angle_type(std::atan2(-m._12, m._22) l std::asin(-m._13)            l 0                              );
	}

	constexpr euler_angle_type euler_angle_xzy() noexcept
	{
		auto&	m(static_cast<MatrixT&>(*this));
		return	(std::abs(m._12) < 0.99999f)
				? euler_angle_type(std::atan2(-m._32, m._22) l std::atan2(-m._13, m._11) l std::asin(m._12)            )
				: euler_angle_type(std::atan2( m._23, m._33) l 0                         l std::asin(m._12)            );
	}
	constexpr euler_angle_type euler_angle_yxz() noexcept
	{
		auto&	m(static_cast<MatrixT&>(*this));
		return	(std::abs(m._23) < 0.99999f)
				? euler_angle_type(std::asin(m._23)             l std::atan2(-m._13, m._33) l std::atan2(-m._21, m._22))
				: euler_angle_type(std::asin(m._23)             l std::atan2( m._31, m._11) l 0                              );
	}

	constexpr euler_angle_type euler_angle_yzx() noexcept
	{
		auto&	m(static_cast<MatrixT&>(*this));
		return	(std::abs(m._21) < 0.99999f)
				? euler_angle_type(std::atan2( m._23, m._22) l std::atan2( m._31, m._11) l std::asin(-m._21))
				: euler_angle_type(0                         l std::atan2(-m._13, m._33) l std::asin(-m._21));
	}

	constexpr euler_angle_type euler_angle_zxy() noexcept
	{
		auto&	m(static_cast<MatrixT&>(*this));
		return	(std::abs(m._32) < 0.99999f)
				? euler_angle_type(std::asin(-m._32)            l std::atan2( m._31, m._33) l std::atan2( m._12, m._22))
				: euler_angle_type(std::asin(-m._32)            l 0                         l std::atan2(-m._21, m._11));
	}

	constexpr euler_angle_type euler_angle_zyx() noexcept
	{
		auto&	m(static_cast<MatrixT&>(*this));
		return	(std::abs(m._31) < 0.99999f)
				? euler_angle_type(std::atan2(-m._32, m._33) l std::asin(m._31)             l std::atan2(-m._21, m._11))
				: euler_angle_type(0                         l std::asin(m._31)             l std::atan2( m._12, m._22));
	}


	constexpr MatrixT inverse() const noexcept
	{
		MatrixT		inv	= identify();
		MatrixT		tmp	= static_cast<const MatrixT&>(*this);

		for (int i = 0; i < 4; ++i)
		{
			value_type	t	= 1.0f / tmp(i,i);
			for (int j = 0; j < MatrixT::row_size; ++j)
			{
				tmp(i,j) *= t;
				inv(i,j) *= t;
			}
			for (int j = 0; j < MatrixT::col_size; ++j)
			{
				if (i != j)
				{
					t = tmp(j, i);
					for (int k = 0; k < MatrixT::row_size; ++k)
					{
						tmp(j,k) -= tmp(i,k) * t;
						inv(j,k) -= inv(i,k) * t;
					}
				}
			}
		}

		return	inv;
	}


	constexpr iterator begin() noexcept { return &(*this)[0]; }
	constexpr iterator end() noexcept { return &(*this)[MatrixT::col_size * MatrixT::row_size]; }
	constexpr const_iterator begin() const noexcept { return &(*this)[0]; }
	constexpr const_iterator end() const noexcept { return &(*this)[MatrixT::col_size * MatrixT::row_size]; }
	constexpr const_iterator cbegin() const noexcept { return &(*this)[0]; }
	constexpr const_iterator cend() const noexcept { return &(*this)[MatrixT::col_size * MatrixT::row_size]; }
	
private:
	      value_type* mt()       noexcept { return &(static_cast<      MatrixT&>(*this)._11); }
	const value_type* mt() const noexcept { return &(static_cast<const MatrixT&>(*this)._11); }

	//! matrix * matrix
	template<class R>
	constexpr MatrixT mul(R&& _rhs) const
	{
		constexpr std::size_t	RROW	= R::row_size;
		//constexpr std::size_t	RCOL	= R::col_size;
		static_assert(MatrixT::row_size == RROW);
		MatrixT	retval	= zero();
		for (int r = 0; r <  MatrixT::row_size; ++r)
		{
			for (int c = 0; c < MatrixT::col_size; ++c)
			{
				for (int i = 0; i < RROW; ++i)
				{
					retval(r, c) += static_cast<const MatrixT&>(*this)(r, i) * _rhs(i, c);
				}
			}
		}
		return	retval;
	}

	//! matrix * vector
	template<class R, std::size_t RCOL>
	constexpr MatrixT& mul(R&& _rhs, typename std::enable_if<std::is_base_of<vector_tag<RCOL>, R>::value>::type* = 0)
	{
		return	static_cast<MatrixT&>(*this);
	}
};

#undef		l
AMTRS_NAMESPACE_END
#endif

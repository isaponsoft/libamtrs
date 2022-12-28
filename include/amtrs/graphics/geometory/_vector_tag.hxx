/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__graphics__geometory__vector_tag__hpp
#define	__libamtrs__graphics__geometory__vector_tag__hpp
#include <cmath>
#include "../../amtrs.hpp"
AMTRS_GRAPHICS_NAMESPACE_BEGIN
AMTRS_IMPLEMENTS_BEGIN(vec)


template<std::size_t Size>
struct	vector_tag
{
	static constexpr std::size_t	col_size	= Size;
};


template<class T, class Tag, class VecT>
struct	_vector_impliments : Tag
{
	using	value_type		= T;
	using	vector_type		= VecT;
	using	iterator		= value_type*;
	using	const_iterator	= const value_type*;

	static constexpr std::size_t	col_size	= Tag::col_size;

	constexpr       value_type& operator [] (int _index)       noexcept { return reinterpret_cast<      value_type*>(this)[_index]; }
	constexpr const value_type& operator [] (int _index) const noexcept { return reinterpret_cast<const value_type*>(this)[_index]; }


	constexpr vector_type operator - () const noexcept
	{
		vector_type	retval;
		for (int c = 0; c < vector_type::col_size; ++c)
		{
			retval[c] = -(*this)[c];
		}
		return	retval;
	}


	constexpr vector_type operator + (const vector_type& _r) const noexcept
	{
		vector_type	retval;
		for (int c = 0; c < vector_type::col_size; ++c)
		{
			retval[c] = (*this)[c] + _r[c];
		}
		return	retval;
	}

	constexpr vector_type operator + (value_type _v) const noexcept
	{
		vector_type	retval;
		for (int c = 0; c < vector_type::col_size; ++c)
		{
			retval[c] = (*this)[c] + _v;
		}
		return	retval;
	}

	constexpr vector_type& operator += (const vector_type& _r) noexcept
	{
		for (int c = 0; c < vector_type::col_size; ++c)
		{
			(*this)[c] += _r[c];
		}
		return	*static_cast<vector_type*>(this);
	}

	constexpr vector_type& operator += (value_type _v) noexcept
	{
		for (int c = 0; c < vector_type::col_size; ++c)
		{
			(*this)[c] += _v;
		}
		return	*static_cast<vector_type*>(this);
	}

	constexpr vector_type operator - (const vector_type& _r) const noexcept
	{
		vector_type	retval;
		for (int c = 0; c < vector_type::col_size; ++c)
		{
			retval[c] = (*this)[c] - _r[c];
		}
		return	retval;
	}

	constexpr vector_type operator - (value_type _v) const noexcept
	{
		vector_type	retval;
		for (int c = 0; c < vector_type::col_size; ++c)
		{
			retval[c] = (*this)[c] - _v;
		}
		return	retval;
	}

	constexpr vector_type& operator -= (const vector_type& _r) noexcept
	{
		for (int c = 0; c < vector_type::col_size; ++c)
		{
			(*this)[c] -= _r[c];
		}
		return	*static_cast<vector_type*>(this);
	}

	constexpr vector_type& operator -= (value_type _v) noexcept
	{
		for (int c = 0; c < vector_type::col_size; ++c)
		{
			(*this)[c] -= _v;
		}
		return	*static_cast<vector_type*>(this);
	}

	constexpr vector_type operator * (const vector_type& _r) const noexcept
	{
		vector_type	retval;
		for (int c = 0; c < vector_type::col_size; ++c)
		{
			retval[c] = (*this)[c] * _r[c];
		}
		return	retval;
	}

	constexpr vector_type operator * (value_type _v) const noexcept
	{
		vector_type	retval;
		for (int c = 0; c < vector_type::col_size; ++c)
		{
			retval[c] = (*this)[c] * _v;
		}
		return	retval;
	}

	constexpr vector_type& operator *= (const vector_type& _r) noexcept
	{
		for (int c = 0; c < vector_type::col_size; ++c)
		{
			(*this)[c] *= _r[c];
		}
		return	*static_cast<vector_type*>(this);
	}

	constexpr vector_type& operator *= (value_type _v) noexcept
	{
		for (int c = 0; c < vector_type::col_size; ++c)
		{
			(*this)[c] *= _v;
		}
		return	*static_cast<vector_type*>(this);
	}

	constexpr vector_type operator / (const vector_type& _r) const noexcept
	{
		vector_type	retval;
		for (int c = 0; c < vector_type::col_size; ++c)
		{
			retval[c] = (*this)[c] / _r[c];
		}
		return	retval;
	}

	constexpr vector_type operator / (value_type _v) const noexcept
	{
		vector_type	retval;
		for (int c = 0; c < vector_type::col_size; ++c)
		{
			retval[c] = (*this)[c] / _v;
		}
		return	retval;
	}

	constexpr vector_type& operator /= (const vector_type& _r) noexcept
	{
		for (int c = 0; c < vector_type::col_size; ++c)
		{
			(*this)[c] /= _r[c];
		}
		return	*static_cast<vector_type*>(this);
	}

	constexpr vector_type& operator /= (value_type _v) noexcept
	{
		for (int c = 0; c < vector_type::col_size; ++c)
		{
			(*this)[c] /= _v;
		}
		return	*static_cast<vector_type*>(this);
	}

	constexpr bool operator == (const vector_type& _r) const noexcept
	{
		for (int c = 0; c < vector_type::col_size; ++c)
		{
			if ((*this)[c] != _r[c])
			{
				return	false;
			}
		}
		return	true;
	}

	constexpr bool operator != (const vector_type& _r) const noexcept
	{
		return	!(*this == _r);
	}

	static constexpr vector_type one() noexcept
	{
		vector_type	retval;
		for (int c = 0; c < vector_type::col_size; ++c)
		{
			retval[c] = 1;
		}
		return	retval;
	}

	static constexpr vector_type zero() noexcept
	{
		vector_type	retval;
		for (int c = 0; c < vector_type::col_size; ++c)
		{
			retval[c] = 0;
		}
		return	retval;
	}


	constexpr value_type length() const noexcept
	{
		value_type	a	= 0;
		for (int c = 0; c < vector_type::col_size; ++c)
		{
			a += (*this)[c] * (*this)[c];
		}
		return	 value_type(sqrt(a));
	}

	constexpr vector_type normalize() const noexcept
	{
		vector_type	ret;
		value_type	inv	= static_cast<value_type>(1.0) / length();
		for (int c = 0; c < vector_type::col_size; ++c)
		{
			ret[c]	= (*this)[c] * inv;
		}
		return	ret;
	}

	constexpr vector_type round() noexcept
	{
		vector_type	retval;
		for (int c = 0; c < vector_type::col_size; ++c)
		{
			retval[c]	= std::round((*this)[c]);
		}
		return	retval;
	}

	iterator begin() noexcept { return &(*this)[0]; }
	iterator end() noexcept { return &(*this)[vector_type::col_size]; }
	const_iterator begin() const noexcept { return &(*this)[0]; }
	const_iterator end() const noexcept { return &(*this)[vector_type::col_size]; }
	const_iterator cbegin() const noexcept { return &(*this)[0]; }
	const_iterator cend() const noexcept { return &(*this)[vector_type::col_size]; }
};


AMTRS_IMPLEMENTS_END(vec)
AMTRS_GRAPHICS_NAMESPACE_END
#endif

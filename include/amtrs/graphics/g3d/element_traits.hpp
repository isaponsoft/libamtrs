/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__g3d__element_traits__hpp
#define	__libamtrs__g3d__element_traits__hpp
#include "../color.hpp"
#include "../geometory/matrix.hpp"
#include "../geometory/vec.hpp"
#include "type.hpp"
AMTRS_G3D_NAMESPACE_BEGIN
using namespace graph;



template<class...>
struct	element_traits;

template<>
struct	element_traits<vec2<float>>
{
	static constexpr element_type	value_type	= element_type::float_value;
	static constexpr std::size_t	value_count	= 2;
};

template<>
struct	element_traits<vec3<float>>
{
	static constexpr element_type	value_type	= element_type::float_value;
	static constexpr std::size_t	value_count	= 3;
};

template<>
struct	element_traits<vec4<float>>
{
	static constexpr element_type	value_type	= element_type::float_value;
	static constexpr std::size_t	value_count	= 4;
};

template<>
struct	element_traits<rgba<float>>
{
	static constexpr element_type	value_type	= element_type::float_value;
	static constexpr std::size_t	value_count	= 4;
};

template<>
struct	element_traits<mat44<float>>
{
	static constexpr element_type	value_type	= element_type::float_value;
	static constexpr std::size_t	value_count	= 16;
};


template<>
struct	element_traits<textureset>
{
	static constexpr element_type	value_type	= element_type::texture2d_value;
	static constexpr std::size_t	value_count	= 1;
};

template<class T>
struct	element_traits<ref<T>>
		: element_traits<T*>
{};

template<class T>
struct	element_traits<T&>
		: element_traits<T>
{};

template<class T>
struct	element_traits<const T>
		: element_traits<T>
{};

template<class T>
struct	element_traits<const T&>
		: element_traits<T>
{};


AMTRS_G3D_NAMESPACE_END
#endif

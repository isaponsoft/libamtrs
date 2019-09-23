/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__g3d__element__hpp
#define	__libamtrs__g3d__element__hpp
#include <string>
#include "def.hpp"
#include "texture.hpp"
AMTRS_G3D_NAMESPACE_BEGIN

enum class	element_type
{
	float_value,
	texture2d_value,
};


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
struct	element_traits<texture*>
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

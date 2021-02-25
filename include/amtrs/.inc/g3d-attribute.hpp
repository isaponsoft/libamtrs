/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__g3d__attribute__hpp
#define	__libamtrs__g3d__attribute__hpp
AMTRS_G3D_NAMESPACE_BEGIN


enum class	primitive_type
{
	triangles,
	triangle_strip,
	triangle_fan,
};



class	textureset;

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
struct	element_traits<textureset*>
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




struct	attribute
{
	const char*		name;
	element_type	type;
	std::size_t		count;

	template<class ElementT>
	static constexpr attribute make(const char* _name) noexcept
	{
		using	type	= element_traits<ElementT>;
		return {
			_name,
			type::value_type,
			type::value_count
		};
	}
};



AMTRS_G3D_NAMESPACE_END
#endif

/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__g3d__vulkan__attribute__hpp
#define	__libamtrs__g3d__vulkan__attribute__hpp
AMTRS_NAMESPACE_BEGIN
template<class T>	struct	rgb;
template<class T>	struct	rgba;
template<class T>	struct	vec2;
template<class T>	struct	vec3;
template<class T>	struct	vec4;
AMTRS_NAMESPACE_END
AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN



template<class...>
struct	attribute_traits;


struct	vertex_input_attribute
{
	VkFormat	format;
	uint32_t	offset;

	vertex_input_attribute() = default;
	vertex_input_attribute(vertex_input_attribute const&) = default;

	template<class T, class C>
	vertex_input_attribute(T C::* _offset, VkFormat _format)
		: format(_format)
		, offset(static_cast<uint32_t>( reinterpret_cast<uintptr_t>((void*)& (reinterpret_cast<C*>(nullptr)->*_offset) )))
	{}

	template<class T, class C>
	vertex_input_attribute(T C::* _offset)
		: format(attribute_traits<T>::format)
		, offset(static_cast<uint32_t>( reinterpret_cast<uintptr_t>((void*)& (reinterpret_cast<C*>(nullptr)->*_offset) )))
	{}
};


inline vkarray<VkVertexInputAttributeDescription> make_vertex_input_attribute(std::initializer_list<vertex_input_attribute> _desc)
{
	vkarray<VkVertexInputAttributeDescription>	retval(_desc.size());
	auto		dest		= retval.data();
	uint32_t	location	= 0;
	for (auto& desc : _desc)
	{
		dest->binding	= 0;
		dest->location	= location;
		dest->format 	= desc.format;
		dest->offset 	= desc.offset;
		++dest;
	}
	return	retval;
}


template<class T>
vkarray<VkVertexInputAttributeDescription> make_vertex_input_attribute()
{
	return	T::make_input_attribute([](std::initializer_list<vertex_input_attribute> _desc)
			{
				return	make_vertex_input_attribute(_desc);
			});
}



template<>
struct	attribute_traits<vec2<float>>
{
	using	value_type	= vec2<float>;
	static constexpr VkFormat	format	= VK_FORMAT_R32G32_SFLOAT;
};


template<>
struct	attribute_traits<vec3<float>>
{
	using	value_type	= vec3<float>;
	static constexpr VkFormat	format	= VK_FORMAT_R32G32B32_SFLOAT;
};


template<>
struct	attribute_traits<vec4<float>>
{
	using	value_type	= vec4<float>;
	static constexpr VkFormat	format	= VK_FORMAT_R32G32B32A32_SFLOAT;
};


template<>
struct	attribute_traits<rgb<float>>
{
	using	value_type	= rgb<float>;
	static constexpr VkFormat	format	= VK_FORMAT_R32G32B32_SFLOAT;
};


template<>
struct	attribute_traits<rgba<float>>
{
	using	value_type	= rgba<float>;
	static constexpr VkFormat	format	= VK_FORMAT_R32G32B32A32_SFLOAT;
};



AMTRS_G3D_VKUTIL_NAMESPACE_END
#endif

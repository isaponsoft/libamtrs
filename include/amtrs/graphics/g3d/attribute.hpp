/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__g3d__attribute__hpp
#define	__libamtrs__g3d__attribute__hpp
#include "../color.hpp"
#include "../geometory/matrix.hpp"
#include "../geometory/vec.hpp"
#include "element_traits.hpp"
#include "type.hpp"
AMTRS_G3D_NAMESPACE_BEGIN
using namespace graph;


#define	AMTRS_G3D_ATTRIBUTE(_name)	amtrs::g3d::attribute::make<decltype(_name)>(#_name)


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

	inline size_t stride() const noexcept
	{
		return	(type == g3d::element_type::texture2d_value)		? sizeof(void*)
			:	(type == g3d::element_type::float_value)			? sizeof(float) * count
			:	0;
	}
};



//! 
inline size_t stride(g3d::attribute const& _attr)
{
	return	(_attr.type == g3d::element_type::texture2d_value)		? sizeof(void*)
		:	(_attr.type == g3d::element_type::float_value)			? sizeof(float) * _attr.count
		:	0;
}

//! 全体の大きさを計算します。
inline size_t stride(view<g3d::attribute const> const& _range)
{
	size_t	s	= 0;
	for (auto& a : _range)
	{
		s	+=	stride(a);
	}
	return	s;
}

AMTRS_G3D_NAMESPACE_END
#endif

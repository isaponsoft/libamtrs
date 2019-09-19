/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__g3d__attribute__hpp
#define	__libamtrs__g3d__attribute__hpp
#include <string>
#include "def.hpp"
#include "element.hpp"
AMTRS_G3D_NAMESPACE_BEGIN



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

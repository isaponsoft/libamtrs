/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__gui__themes__hpp
#define	__libamtrs__gui__themes__hpp
#include "../memory/ref_object.hpp"
#include "../graphics/color.hpp"
AMTRS_NAMESPACE_BEGIN

class	themes : public ref_object
{
public:
	enum class	themes_type
	{
		sysdefault	= 0,
		lightmode	= 1,
		darkmode	= 2
	};

	using	color_type	= graph::rgba<uint8_t>;

	static constexpr themes_type	sysdefault	= themes_type::sysdefault;
	static constexpr themes_type	lightmode	= themes_type::lightmode;
	static constexpr themes_type	darkmode	= themes_type::darkmode;

	static auto create(themes_type _type = sysdefault) -> ref<themes>;

	// return
	//	lightmode or darkmode
	static auto current() -> themes_type;

	themes_type type() const noexcept;
	color_type foreground();
	color_type background();
};

AMTRS_NAMESPACE_END
#endif

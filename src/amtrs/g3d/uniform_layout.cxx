/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/graphics/g3d.hpp>
#include <amtrs/system/log.hpp>
AMTRS_G3D_NAMESPACE_BEGIN



uniform_layout::layout::layout(shader_stage _stage, uint32_t _location, shared_buffer<attribute> _attr)
	: stage(_stage)
	, location(_location)
	, attributes(std::move(_attr))
{
#if		AMTRS_DEBUG
	if (attributes.empty())
	{
		AMTRS_WARN_LOG("uniform_layout has not attributes.");
		return;
	}
#endif

	// テクスチャは単体でのみ指定できる
	if (attributes.size() == 1 && (*attributes.begin()).type == element_type::texture2d_value)
	{
		type	= format_types::texture_value;
		return;
	}

	// contents_value の正当性を検証
	for (auto& attr : attributes)
	{
		if (attr.type == element_type::texture2d_value)
		{
			AMTRS_WARN_LOG("material_layout attribute struct is not support texture.");
			return;
		}
	}
	type	= format_types::constants_value;
}





AMTRS_G3D_NAMESPACE_END

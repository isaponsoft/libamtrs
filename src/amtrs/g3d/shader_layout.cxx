/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/graphics/g3d.hpp>
#include <amtrs/system/log.hpp>
AMTRS_G3D_NAMESPACE_BEGIN


shader_layout::shader_layout(shared_buffer<g3d::attribute> _input, shared_buffer<layoutdata> _uniforms)
{
	mInstance		= new instance_type();
	mInstance->attributes	= std::move(_input);
	mInstance->uniforms		= std::move(_uniforms);
}


auto shader_layout::stride() const noexcept -> size_t
{
	size_t	s	= 0;
	for (auto& u : mInstance->uniforms)
	{
		for (auto& a : u.attributes)
		{
			s += a.stride();
		}
	}
	return	s;
}


AMTRS_G3D_NAMESPACE_END

/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__graphics__g3d__shader_layout__hpp
#define	__libamtrs__graphics__g3d__shader_layout__hpp
#include "device.hpp"
#include "shadermodule.hpp"
#include "uniform_layout.hpp"
AMTRS_G3D_NAMESPACE_BEGIN


//! パイプラインで使用するデータのレイアウトを示します。
class	shader_layout
{
public:
	using	layoutdata	= uniform_layout::layout;

	struct	instance_type : ref_object
	{
		ref<ref_object>						realized;
		shared_buffer<g3d::attribute>		attributes;
		shared_buffer<layoutdata>			uniforms;
	};

	shader_layout() = default;
	shader_layout(shader_layout const&) = default;
	shader_layout(shader_layout&&) = default;
	shader_layout& operator = (shader_layout const&) = default;
	shader_layout& operator = (shader_layout&&) = default;


	shader_layout(shared_buffer<g3d::attribute> _input, shared_buffer<layoutdata> _uniforms);

	auto inputformat() const noexcept -> shared_buffer<g3d::attribute> const&
	{
		return	mInstance->attributes;
	}

	layoutdata const* uniform(shader_stage _stage) const noexcept
	{
		for (auto& u : mInstance->uniforms)
		{
			if (u.stage == _stage)
			{
				return	&u;
			}
		}
		return	nullptr;
	}

	auto uniforms() const noexcept -> shared_buffer<layoutdata> const&
	{
		return	mInstance->uniforms;
	}


	auto stride() const noexcept -> size_t;
	bool empty() const noexcept { return mInstance.empty(); }
protected:
	ref<instance_type>	mInstance;

	template<class, class>
	friend	struct	realizer;
};


AMTRS_G3D_NAMESPACE_END
#endif

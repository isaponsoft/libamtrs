/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/graphics/g3d/shadermodule.hpp>
AMTRS_G3D_NAMESPACE_BEGIN



shaderset::shaderset(shader_layout _format, std::initializer_list<shadermodule> _shaders)
{
	mInstance			= new instance_type();
	mInstance->layout	= std::move(_format);
	for (auto& s : _shaders)
	{
		mInstance->modules[(int)s.stage()] = std::move(s);
	}
}


shaderset::shaderset(std::initializer_list<shadermodule> _shaders)
{
	size_t									uniformSize	= 0;
	shared_buffer<attribute>				attributes;
	shared_buffer<uniform_layout::layout>	uniforms;

	mInstance			= new instance_type();
	for (auto& s : _shaders)
	{
		if (s.stage() == shader_stage::vertex_stage)
		{
			attributes	= s.attributes();
		}

		for (auto& l : s.uniforms().layouts())
		{
			++uniformSize;
		}
		mInstance->modules[(int)s.stage()] = std::move(s);
	}

	// Genereate layout
	uniforms			= shared_buffer<uniform_layout::layout>(uniformSize);
	auto*	dst			= uniforms.data();
	for (auto& s : _shaders)
	{
		for (auto& l : s.uniforms().layouts())
		{
			*dst++	= l;
		}
	}
	mInstance->layout	= g3d::shader_layout(std::move(attributes), std::move(uniforms));
}



shadermodule::shadermodule(shader_stage _stage)
{
	mInstance			= new instance_type();
	mInstance->stage	= _stage;
}


shadermodule shadermodule::create_vertex(shared_buffer<attribute> _attributes,  uniform_layout _uniforms)
{
	shadermodule	ret;
	ret.mInstance					= new instance_type();
	ret.mInstance->stage			= shader_stage::vertex_stage;
	ret.mInstance->attributes		= std::move(_attributes);
	ret.mInstance->uniforms			= std::move(_uniforms);
	ret.mInstance->uniforms.stage(ret.mInstance->stage);
	return	ret;
}


shadermodule shadermodule::create_fragment(uniform_layout _uniforms)
{
	shadermodule	ret;
	ret.mInstance					= new instance_type();
	ret.mInstance->stage			= shader_stage::fragment_stage;
	ret.mInstance->uniforms			= std::move(_uniforms);
	ret.mInstance->uniforms.stage(ret.mInstance->stage);
	return	ret;
}


AMTRS_G3D_NAMESPACE_END

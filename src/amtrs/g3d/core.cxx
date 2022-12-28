/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/graphics/g3d.hpp>
#include <amtrs/system/log.hpp>
AMTRS_G3D_NAMESPACE_BEGIN


void commandlist::execute(commandexecuter& _exec)
{
	for (auto& cmdbuf : mBuffer)
	{
		auto	data	= cmdbuf.data();
		auto	end		= data + cmdbuf.header().usingsize;
		while (data < end)
		{
			auto	idx		= *reinterpret_cast<std::type_index*>(data);
			auto	params	= data + sizeof(std::type_index);
			#define	__AMTRS_G3DCL(_type)	\
				else if (idx == typeid(commandexecuter::_type))\
				{\
					_exec.exec(*reinterpret_cast<commandexecuter::_type*>(params));\
					data += align<commandexecuter::_type>();\
				}
//					AMTRS_WARN_LOG("%s", typeid(commandexecuter::_type).name());\

			if (idx == typeid(subcommand))
			{
				reinterpret_cast<subcommand*>(params)->cmd->execute(_exec);
				data += static_cast<uint32_t>(align<subcommand>());
			}
			__AMTRS_G3DCL(constants_update)
			__AMTRS_G3DCL(vertexbuffer_update)
			__AMTRS_G3DCL(textureset_update)
			__AMTRS_G3DCL(shadermodule_update)
			__AMTRS_G3DCL(renderstatus_push)
			__AMTRS_G3DCL(renderpass_begin)
			__AMTRS_G3DCL(renderpass_end)
			__AMTRS_G3DCL(pipeline_change)
			__AMTRS_G3DCL(viewport_scissor)
			__AMTRS_G3DCL(uniform_bind)
			__AMTRS_G3DCL(draw_vertex)
			__AMTRS_G3DCL(begin)
			__AMTRS_G3DCL(end)
			__AMTRS_G3DCL(present)
			#undef	__AMTRS_G3DCL
			else
			{
				AMTRS_WARN_LOG("Unknown command");
				data	= end;
			}
		}
	}
}


void commandlist::clear()
{
	for (auto& cmdbuf : mBuffer)
	{
		auto	data	= cmdbuf.data();
		auto	end		= data + cmdbuf.header().usingsize;
		while (data < end)
		{
			auto	idx		= *reinterpret_cast<std::type_index*>(data);
			auto	params	= data + sizeof(std::type_index);
			#define	__AMTRS_G3DCL(_type)	else if (idx == typeid(commandexecuter::_type)) { using param_type = commandexecuter::_type; reinterpret_cast<param_type*>(params)->~param_type(); data += align<commandexecuter::_type>(); }
			if (idx == typeid(subcommand))
			{
				reinterpret_cast<subcommand*>(params)->~subcommand();
				data += static_cast<uint32_t>(align<subcommand>());
			}
			__AMTRS_G3DCL(constants_update)
			__AMTRS_G3DCL(vertexbuffer_update)
			__AMTRS_G3DCL(textureset_update)
			__AMTRS_G3DCL(shadermodule_update)
			__AMTRS_G3DCL(renderstatus_push)
			__AMTRS_G3DCL(renderpass_begin)
			__AMTRS_G3DCL(renderpass_end)
			__AMTRS_G3DCL(pipeline_change)
			__AMTRS_G3DCL(viewport_scissor)
			__AMTRS_G3DCL(uniform_bind)
			__AMTRS_G3DCL(draw_vertex)
			__AMTRS_G3DCL(begin)
			__AMTRS_G3DCL(end)
			__AMTRS_G3DCL(present)
			#undef	__AMTRS_G3DCL
			else
			{
				AMTRS_WARN_LOG("Unknown command");
				data	= end;
			}
		}
		cmdbuf.clear();
	}
	mBuffer.clear();
}




framebuffer::framebuffer(textureset _color, textureset _depth, textureset _stencil)
{
	mInstance			= new instance_type();
	mInstance->color	= std::move(_color);
	mInstance->depth	= std::move(_depth);
	mInstance->stencil	= std::move(_stencil);
}

void framebuffer::reset()
{
	if (mInstance)
	{
		mInstance->realized	= nullptr;
	}
}

textureset::textureset(size2<unsigned int> _size, format_types _fmt, uint32_t _usage)
{
	mInstance			= new instance_type();
	mInstance->size		= std::move(_size);
	mInstance->format	= std::move(_fmt);
	mInstance->usage	= std::move(_usage);
}



constants::constants(shader_layout _layout, uint32_t _index)
{
	mInstance			= new instance_type();
	mInstance->layout	= std::move(_layout);
	mInstance->index	= std::move(_index);
}



vertexbuffer::vertexbuffer(size_t _size)
{
	mInstance			= new instance_type();
	mInstance->size		= _size;
}


imagesampler::imagesampler(samplerdata _data)
{
	mInstance			= new instance_type();
	mInstance->data		= std::move(_data);
}


uniformset::uniformset(shader_layout _layout)
{
	auto	uniforms	= _layout.uniforms();
	mInstance			= new instance_type();
	mInstance->layout	= std::move(_layout);
	mInstance->binds	= shared_buffer<ref<instance_type::binddata>>(uniforms.size());
}


uniformset& uniformset::bind(uint32_t _index, constants _uniform)
{
	using	datatype	= instance_type::constants_data;
	datatype*	data;
	if (!mInstance->binds[_index])
	{
		data						= new datatype();
		mInstance->binds[_index]	= data;
	}
	else
	{
		data						= static_cast<datatype*>(mInstance->binds[_index].get());
	}
	data->val1			= std::move(_uniform);
	mInstance->modified	= true;
	return	*this;
}


uniformset& uniformset::bind(uint32_t _index, textureset _texture, imagesampler _sampler)
{
	using	datatype	= instance_type::tetxure_data;
	datatype*	data;
	if (!mInstance->binds[_index])
	{
		data						= new datatype();
		mInstance->binds[_index]	= data;
	}
	else
	{
		data						= static_cast<datatype*>(mInstance->binds[_index].get());
	}
	data->val1			= std::move(_texture);
	data->val2			= std::move(_sampler);
	mInstance->modified	= true;
	return	*this;
}


pipeline::pipeline(shaderset _material, renderstate const& _rs)
{
	mInstance	= new instance_type();
	mInstance->rs	= std::move(_rs);
	mInstance->mat	= std::move(_material);
}


AMTRS_G3D_NAMESPACE_END

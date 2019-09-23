/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef __libamtrs__g3dex__component__shader__hpp
#define __libamtrs__g3dex__component__shader__hpp
#include <map>
#include <string>
#include "../../utility/singleton.hpp"
#include "../device.hpp"
#include "def.hpp"
AMTRS_G3DEX_COMPONENT_NAMESPACE_BEGIN

struct	shader_base
{
	using	shader_type	= g3d::shader;

	shader_base()
		: mShaderCache(get_singleton_instance<shader_cache>())
	{}


	template<class FuncT>
	shader_type* shader(const char* _name, FuncT&& _func)
	{
		if (mShader.empty())
		{
			std::string	key(_name);
			auto	it	= mShaderCache->find(key);
			if (it == mShaderCache->end() || !it->second->is_enable())
			{
				mShader	= _func();
				mShaderCache->emplace(key, mShader);
			}
			else
			{
				mShader	= it->second;
			}
		}
		return	mShader.get();
	}

private:
	struct	shader_cache
			: ref_object
			, std::map<std::string, ref<shader_type>>
	{};

	ref<shader_type>	mShader;
	ref<shader_cache>	mShaderCache;
};




template<class VertexT, class UniformT>
struct	shader
		: protected shader_base
{
	using	shader_type	= g3d::shader;
	using	vertex		= VertexT;
	using	uniform		= UniformT;

	g3d::shader* get_shader(g3d::device* _device, std::string_view _vs, std::string_view _fs)
	{
		return	shader_base::shader(typeid(*this).name(), [&]()
				{
					return	_device->create_shader
							(
								vertex::attribute_info,
								uniform::uniform_info,
								_vs, _fs
							);
				});
	}
};

AMTRS_G3DEX_COMPONENT_NAMESPACE_END
#endif


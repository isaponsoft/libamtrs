/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef __libamtrs__g3dex__component__texture__hpp
#define __libamtrs__g3dex__component__texture__hpp
#include "../../g3d/texture.hpp"
#include "../../ref_object.hpp"
#include "def.hpp"
AMTRS_G3DEX_COMPONENT_NAMESPACE_BEGIN

struct	texture
{
	using	texture_type	= g3d::texture;

	template<class NodeT>
	struct	compo
	{
		using	texture_type	= g3d::texture;

		compo()
		{}

		compo(texture_type* _texture, bool _update_size = false)
		{
			set_texture(_texture, _update_size);
		}

		texture_type* texture() const noexcept { return mTexture.get(); }
		void set_texture(texture_type* _texture, bool _update_size = true) noexcept
		{
			mTexture = _texture;
			if (_texture && _update_size)
			{
				static_cast<NodeT*>(this)->set_size(typename NodeT::size_type(_texture->size()));
			}
		}

	private:
		ref<texture_type>	mTexture;
	};


	template<class NodeT>
	struct	dispa
	{
	};
};

AMTRS_G3DEX_COMPONENT_NAMESPACE_END
#endif


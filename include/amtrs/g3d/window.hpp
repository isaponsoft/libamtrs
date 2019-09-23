/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__g3d__window__hpp
#define	__libamtrs__g3d__window__hpp
#include <functional>
#include "../window/@"
#include "def.hpp"
#include "device.hpp"
AMTRS_G3D_NAMESPACE_BEGIN

enum class	api_type
{
	api_auto,
	api_opengles_2_0,
	api_vulkan,
	api_metal,
	api_direct3d11,
	api_direct3d12,
};

struct	create_params
{
	api_type		type		= api_type::api_auto;
	rgba<int8_t>	colorDepth	= {8, 8, 8, 8};
	int8_t			depthBit	= 16;
	int8_t			stencilBit	= 8;
};



class	window
		: public amtrs::window
{
public:
	using	size_type	= amtrs::window::size_type;

	static ref<window> create(size_type _size, const create_params* _cp = nullptr);

	virtual g3d::device* get_g3d_device() = 0;
};



AMTRS_G3D_NAMESPACE_END

#if		AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_WIN32
#include "window-win32-opengl.hpp"
#endif

#if		AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_ANDROID
#include "window-android-opengl.hpp"
#include "window-android-vulkan.hpp"
#endif

#if		AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_IOS
#include "window-ios-opengl.hpp"
#include "window-ios-metal.hpp"
#endif

#endif

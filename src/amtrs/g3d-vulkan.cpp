/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#if		AMTRS_VULKAN_USE


#include <amtrs/amtrs.hpp>
#include <thread>
#include <amtrs/g3d.hpp>
#include <amtrs/.driver/g3d-vulkan.hpp>


#if		AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_WIN32
#else
#include <dlfcn.h>
#endif


// 関数テーブル
#define	__AMTRS__VULKAN_FUNC(_name)	PFN_##_name	_name;
#include <amtrs/.driver/g3d-vulkan-api-function_list.hpp>
#undef	__AMTRS__VULKAN_FUNC

AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN



vkdevice::vkdevice(window* _window, std::function<surface_func> _surface)
	: super_type(api_type::api_vulkan)
	, mWindow(_window)
	, mSurface(std::move(_surface))
{
#if	AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_WIN32
	// 動的リンクを行うので関数テーブルを初期化
	HMODULE	libvk	= LoadLibraryA("vulkan-1.dll");
	AMTRS_WARN_LOG("vulkan-1.dll => %p", libvk);

	#define	__AMTRS__VULKAN_FUNC(_name)	\
		_name	= reinterpret_cast<PFN_##_name>(GetProcAddress(libvk, #_name));\
		if (!_name) { AMTRS_WARN_LOG(#_name " : not found"); }\
	//	else { AMTRS_WARN_LOG(#_name " : found"); }

#else
	// 動的リンクを行うので関数テーブルを初期化
	void*	libvk	= dlopen("libvulkan.so", RTLD_NOW|RTLD_LOCAL);
	AMTRS_WARN_LOG("libvulkan.so => %p", libvk);

	#define	__AMTRS__VULKAN_FUNC(_name)	\
		_name	= reinterpret_cast<PFN_##_name>(dlsym(libvk, #_name));\
		if (!_name) { AMTRS_WARN_LOG(#_name " : not found"); }\
	//	else { AMTRS_WARN_LOG(#_name " : found"); }

#endif
	#include <amtrs/.driver/g3d-vulkan-api-function_list.hpp>
	#undef	__AMTRS__VULKAN_FUNC

	AMTRS_WARN_LOG("Vulkan function listup finish");
}

vkdevice::~vkdevice()
{
	clean();
}



static	size2<int>	gSizeExperimental;


void vkdevice::resize(size2<int> _size)
{
	gSizeExperimental = _size;
}


void vkdevice::execute(ref<commandlist> _commands)
{
	if (!is_context_enable()) { return; }

	if (mGraphics.empty())
	{
		initdata	id;
		mSurface(mWindow, id);
		mGraphics		= vkgraphics::create(std::move(id.instance), id.surface);
		mGraphics->change_surface(id.surface);
		mGraphics->next_image();
	}
	else
	{
		do
		{
			mGraphics->next_image();
			if (vklasterr.good())
			{
				break;
			}
	        if (vklasterr == VK_ERROR_OUT_OF_DATE_KHR)
			{
				mGraphics->reset_framebuffer({(uint32_t)gSizeExperimental.width, (uint32_t)gSizeExperimental.height});
			}
	        else if (vklasterr == VK_SUBOPTIMAL_KHR)
			{
			}
			else if (vklasterr == VK_ERROR_SURFACE_LOST_KHR)
			{
				mGraphics->destroy_surface();
				initdata	id;
				mSurface(mWindow, id);
				mGraphics->change_surface(id.surface);
			}
		} while (!vklasterr.good());
	}



	mProcessing.store(true);
	vklasterr.clear();

	auto&	frame	= mGraphics->current_image();
	mExecutor.initialize();
	mExecutor.lastlist	= std::move(_commands);
	mExecutor.graphics	= mGraphics;

	mExecutor.cmdbuff	= vkref<VkCommandBuffer>::create(frame.pool);
	mExecutor.lastlist->execute(mExecutor);

	vklasterr.clear();
	mProcessing.store(false);
}



void vkdevice::present()
{
}


std::string vkdevice::name() const
{
	return	"vulkan";
}

void vkdevice::active()
{
}

void vkdevice::deactive()
{
}

auto vkdevice::size() const -> size_type
{
	return	mWindow->size();
}


void vkdevice::clean()
{
	while (is_processing())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	mGraphics.clear();
}


bool vkdevice::is_context_enable() const
{
	return	true;//mGraphics != nullptr;
}

bool vkdevice::is_processing()
{
	return	mProcessing.load();
}


AMTRS_G3D_VKUTIL_NAMESPACE_END

#endif

/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__driver__g3d__vulkan__vkdevice__hpp
#define	__libamtrs__driver__g3d__vulkan__vkdevice__hpp
#include <functional>
AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN



// ============================================================================
//! VULKAN DEVICE
// ----------------------------------------------------------------------------
class	vkdevice
		: public ::amtrs::g3d::device
{
public:
	struct	initdata
	{
		vkref<VkInstance>	instance;
		vkref<VkSurfaceKHR>	surface;
	};


	using	surface_func	= void(window*, initdata&);
	using	super_type		= ::amtrs::g3d::device;

	vkdevice(window* _window, std::function<surface_func> _surface);
	~vkdevice();


	virtual void execute(ref<commandlist> _commands) override;


	virtual void present() override;

	virtual std::string name() const override;

	virtual void active() override;

	virtual void deactive() override;

	virtual bool is_context_enable() const override;

	virtual size_type size() const override;

	virtual void reset() override { clean(); };

	void clean();

	void resize(size2<int> _size);


	//! 現在処理を実行中かどうか調べます。
	//! この関数が true の時はGPUを開放したりしないでください。
	bool is_processing();

	window*						mWindow;
	ref<vkgraphics>				mGraphics;
	vkexecuter					mExecutor;
	std::function<surface_func>	mSurface;
	std::atomic<bool>			mProcessing;
};


AMTRS_G3D_VKUTIL_NAMESPACE_END
#endif

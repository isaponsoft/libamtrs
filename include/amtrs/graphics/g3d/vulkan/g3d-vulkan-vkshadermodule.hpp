/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__driver__g3d__vulkan__vkshadermodule__hpp
#define	__libamtrs__driver__g3d__vulkan__vkshadermodule__hpp
AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN


//! シェーダーモジュール
class	vkshadermodule
		: public ref_object
{
public:
	static ref<vkshadermodule> realize(shadermodule& _shader, vkref<VkDevice> const& _device)
	{
		return	realizing<vkshadermodule>(_shader, _device);
	}

	void initialize(shadermodule::instance_type* _owner, vkref<VkDevice> const& _device)
	{
		mDevice	= _device;
	}

	void update(void const* _data, size_t _size)
	{
		mModule	= vkref<VkShaderModule>::create(mDevice, _data, _size);
	}

	vkref<VkDevice>			mDevice;
	vkref<VkShaderModule>	mModule;
};



AMTRS_G3D_VKUTIL_NAMESPACE_END
#endif

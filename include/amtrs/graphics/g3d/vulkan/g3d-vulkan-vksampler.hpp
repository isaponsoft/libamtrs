/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__driver__g3d__vulkan__vksampler__hpp
#define	__libamtrs__driver__g3d__vulkan__vksampler__hpp
AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN




class	vksampler
		: public ref_object
{
public:
	static ref<vksampler> realize(imagesampler& _sampler, vkref<VkDevice> const& _device)
	{
		return	realizing<vksampler>(_sampler, _device);
	}

	void initialize(imagesampler::instance_type* _owner, vkref<VkDevice> const& _device)
	{
		mSampler	= vkref<VkSampler>::create(_device);
	}

	vkref<VkSampler>		mSampler;
};

AMTRS_G3D_VKUTIL_NAMESPACE_END
#endif

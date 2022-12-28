/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__g3d__vulkan_to_string__hpp
#define	__libamtrs__g3d__vulkan_to_string__hpp
namespace std
{
	std::string to_string(VkDescriptorType _value);
	std::string to_string(VkShaderStageFlagBits _value);
	std::string to_string(VkResult _value);
	std::string to_string(VkFormat _value);
}
#endif

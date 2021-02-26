/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__g3d__vulkan__enum__hpp
#define	__libamtrs__g3d__vulkan__enum__hpp
AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN



template<class T, class SizeT = uint32_t, class Getter>
vkarray<T> vkenum(Getter&& _getter)
{
	SizeT		count	= 0;

	vklasterr	= _getter(&count, nullptr);
	if (vklasterr)
	{
		return	{};
	}

	vkarray<T>	retval(static_cast<size_t>(count));
	vklasterr	= _getter(&count, retval.data());
	if (vklasterr)
	{
		return	{};
	}

	return	retval;
}



AMTRS_G3D_VKUTIL_NAMESPACE_END
#endif

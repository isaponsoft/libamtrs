# Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved.
# Use of this source code is governed by a BSD-style  license that
# can be found in the LICENSE file.
if (NOT __amtrs__vulkan__lib)
	find_path(__amtrs__vulkan__inc NAMES vulklan/vulklan.h)
	if (__amtrs__vulkan__inc)
		set(__amtrs__vulkan__inc	${__amtrs__vulkan__inc}	CACHE STRING "vulkan_lib" FORCE)
	elseif (NOT "$ENV{VULKAN_SDK}" STREQUAL "")
		set(__amtrs__vulkan__inc	"$ENV{VULKAN_SDK}/include"	CACHE STRING "vulkan_lib" FORCE)
	elseif (NOT "$ENV{VK_SDK_PATH}" STREQUAL "")
		set(__amtrs__vulkan__inc	"$ENV{VK_SDK_PATH}/include"	CACHE STRING "vulkan_lib" FORCE)
	endif ()
endif()


if (__amtrs__vulkan__inc)
	target_include_directories(${AMTRS_LIBRARY}	PUBLIC ${__amtrs__vulkan__inc})
endif()

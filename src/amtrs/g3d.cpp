/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/amtrs.hpp>
#include "g3d/core.cxx"
#include "g3d/shader_layout.cxx"
#include "g3d/shader.cxx"
#include "g3d/shadermodule.cxx"
#include "g3d/uniform_layout.cxx"

#include AMTRS_PLATFORM_SUBCODE(amtrs-g3d)
#include AMTRS_PLATFORM_SUBCODE(amtrs-g3d-opengl)

// VULKAN
#include AMTRS_PLATFORM_SUBCODE(amtrs-g3d-vulkan)
#include "g3d/vulkan.cxx"
#include "g3d/vulkan-to_string.cxx"


#if		AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_WIN32
#include AMTRS_PLATFORM_SUBCODE(amtrs-g3d-d3d12)
#endif


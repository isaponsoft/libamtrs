/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__src__driver__g3d__hpp
#define	__libamtrs__src__driver__g3d__hpp
#include <amtrs/graphics/g3d.hpp>
#include "amtrs-gui-window.hpp"
AMTRS_OS_NAMESPACE_BEGIN
auto g3d_d3d12_create(g3d::device::createparams& _cp)  -> ref<g3d::device>;
auto g3d_opengl_create(g3d::device::createparams& _cp) -> ref<g3d::device>;
auto g3d_vulkan_create(g3d::device::createparams& _cp) -> ref<g3d::device>;
AMTRS_OS_NAMESPACE_END
#endif

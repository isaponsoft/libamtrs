/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__gframe__texture_manager__hpp
#define	__gframe__texture_manager__hpp
#include <amtrs/g3d/@>
#include <amtrs/graphics/@>
#include <amtrs/utility/@>
#include "def.hpp"
GFRAME_NAMESPACE_BEGIN
using namespace amtrs;


ref<g3d::texture> load_texture(const std::string& _filename);



GFRAME_NAMESPACE_END
#endif

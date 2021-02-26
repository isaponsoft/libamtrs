/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libgframe__main__hpp
#define	__libgframe__main__hpp
#include <string>
#include <amtrs/application/@>
#include <amtrs/node/@>
#include "module.hpp"
GFRAME_NAMESPACE_BEGIN

auto get_fileloader() -> amtrs::filesystem::fileloader*;
auto load_texture(std::string const& _filename) -> ref<g3d::texture>;

amtrs::ref<amtrs::application> gframe_application();


GFRAME_NAMESPACE_END
#endif

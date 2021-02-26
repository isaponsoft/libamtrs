/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__gframe__load_string__hpp
#define	__gframe__load_string__hpp
#include <string>
#include <vector>
#include "def.hpp"
GFRAME_NAMESPACE_BEGIN
using namespace amtrs;


std::string load_string(std::string const& _filename);
std::vector<char> load_byte(std::string const& _filename);


GFRAME_NAMESPACE_END
#endif

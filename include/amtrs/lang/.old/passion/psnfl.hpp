/* Copyright (c) 2022, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__lang__passion__psnfl__hpp
#define	__libamtrs__lang__passion__psnfl__hpp
#include "../util/callback.hpp"
#include "node.hpp"
PASSION_NAMESPACE_BEGIN


// parse psnfl (passion functional language)
auto parse_psnfl(psnfl_status& _status, text& _src)  -> simplearray<node>;

// parse psnfl one node
auto parse_psnfl_node(psnfl_status& _status, text& _src)  -> node;



PASSION_NAMESPACE_END
#endif

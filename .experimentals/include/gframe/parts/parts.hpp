/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libgframe__parts__parts__hpp
#define	__libgframe__parts__parts__hpp
#include <string>
#include <amtrs/node/@>
#include "../module.hpp"
GFRAME_NAMESPACE_BEGIN

class	parts
		: public amtrs::node
{
public:
	using	selector_type	= std::string;
};


GFRAME_NAMESPACE_END
#endif

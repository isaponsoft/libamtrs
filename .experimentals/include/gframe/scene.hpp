/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libgframe__scene__hpp
#define	__libgframe__scene__hpp
#include <string>
#include <amtrs/node/scene_node.hpp>
#include "module.hpp"
GFRAME_NAMESPACE_BEGIN

class	scene
		: public amtrs::scene_node<node>
{
public:
	using	super_type		= scene_node<node>;
	using	selector_type	= std::string;

	scene();
	virtual ~scene();
	virtual void on_enter() override;
	virtual void on_invisible() override;
	virtual void on_visible() override;


	static scene* current();
};


GFRAME_NAMESPACE_END
#endif

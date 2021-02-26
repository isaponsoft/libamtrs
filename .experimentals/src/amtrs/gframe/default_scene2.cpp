/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/application/@>
#include <amtrs/chrono/@>
#include <amtrs/g3d/@>
#include <amtrs/node/@>
#include <gframe/@>
#include <app-config.hpp>		// Application configuration.
GFRAME_NAMESPACE_BEGIN
using namespace	amtrs;

class	default_title_scene2
		: public scene
{
public:
	using	super_type	= scene_node<node>;

	virtual void on_visible() override
	{
		super_type::on_visible();
	}

	virtual void on_invisible() override
	{
		super_type::on_invisible();
	}
};




GFRAME_NAMESPACE_END





using namespace gframe;
module<scene>	__default_title_scene2("default::title2", []() -> ref<scene>
{
	return	new default_title_scene2();
});

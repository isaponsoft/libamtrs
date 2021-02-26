/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/application/@>
#include <amtrs/chrono/@>
#include <amtrs/g3d/@>
#include <amtrs/node/scene_node_stack.hpp>
#include <gframe/@>
#include <app-config.hpp>		// Application configuration.
GFRAME_NAMESPACE_BEGIN
using namespace	amtrs;

class	default_main_scene
		: public scene
{
public:
	using	super_type	= scene;

	default_main_scene()
	{
		AMTRS_DEBUG_LOG("default_main_scene::default_main_scene()");
	}

	virtual void on_enter() override
	{
		super_type::on_enter();
		this->schedule_once([this](float)
		{
			auto	t	= module<scene>::create({"title", "default::title"});
			t->set_size(this->size());
			super_type::get_scene_stack()->jump(t.get());
		});
	}

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
module<scene>	__default_main_scene("default::main", []() -> ref<scene>
{
	return	new default_main_scene();
});

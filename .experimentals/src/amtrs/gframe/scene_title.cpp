/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
/********************************************************************
	ごくごく単純なタイトル画面。
 --------------------------------------------------------------------
	
	STARTボタンを押すと module<scene> : game を呼び出します。


 ********************************************************************/
#include <amtrs/application/@>
#include <amtrs/chrono/@>
#include <amtrs/g3d/@>
#include <amtrs/node/scene_node_stack.hpp>
#include <gframe/@>
#include <app-config.hpp>		// Application configuration.
GFRAME_NAMESPACE_BEGIN
using namespace	amtrs;

class	default_title_scene
		: public scene
{
public:
	using	super_type	= scene;

	virtual void on_enter() override
	{
		//AMTRS_DEBUG_LOG("[%p] default_title_scene::on_created() %s", static_cast<node*>(this), typeid(super_type).name());
		super_type::on_enter();
		std::string	filename	= "title/bg.jpg";
		if (!get_fileloader()->exists(filename))
		{
			filename	= "title/bg.png";
			if (!get_fileloader()->exists(filename))
			{
				filename	= {};
			}
		}

		if (filename.empty())
		{
			AMTRS_DEBUG_LOG("default title ::on_created() can't load title/bg.[jpg|png]");
		}
		else
		{
			AMTRS_DEBUG_LOG("default title ::on_created() load BG \"%s\"", filename.c_str());
			mBg	= new sprite_node<node>(load_texture(filename));
			mBg->set_priority(-10);
			add_child(mBg);
		}


		mBtnStart		= ui::button::create({ 100, 100, 0 }, "START", "title/button_active.png", {1,1,1,1});
		mBtnContinue	= ui::button::create({ 100, 200, 0 }, "CONTINUE", "title/button_nonactive.png", {1,1,1,1});
	
/*
		auto	p	= gframe::module<gframe::panel>::create("panel::footer");
		this->add_child(p);

		mFooter	= p;
*/
		mBtnStart->set_trigger_callback([this](auto pos)
		{
			auto	next	= module<scene>::create({"game"});
			super_type::get_scene_stack()->jump(next.get());
//			mFooter->show();
		});

		mBtnContinue->set_trigger_callback([](auto pos)
		{
//			mFooter->hidden();
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

//	gframe::panel*	mFooter;

	sprite_node<node>*				mBg				= nullptr;
	ui::button*						mBtnStart		= nullptr;
	ui::button*						mBtnContinue	= nullptr;
};

GFRAME_NAMESPACE_END





using namespace gframe;
module<scene>	__default_title_scene("default::title", []() -> ref<scene>
{
	return	new default_title_scene();
});

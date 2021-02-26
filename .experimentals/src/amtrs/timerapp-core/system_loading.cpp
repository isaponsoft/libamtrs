/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <algorithm>
#include <random>
#include <amtrs/node/node.hpp>
#include <amtrs/node/anchor_node.hpp>
#include <amtrs/node/back_key_node.hpp>
#include <amtrs/node/fill_node.hpp>
#include <amtrs/node/fillwrap_node.hpp>
#include <amtrs/node/input_untransparentable_node.hpp>
#include <gframe/@>

#include <timerapp-core/application.hpp>
#include <timerapp-core/contents.hpp>
#include <timerapp-core/system.hpp>

using namespace amtrs;
using namespace gframe;
using namespace contents;



class	loading_contents
		: public input_untransparentable_node<back_key_node<fillwrap_node<updatable_node<fill_node<node>>>>>
{
public:
	using	super_type	= input_untransparentable_node<back_key_node<fillwrap_node<updatable_node<fill_node<node>>>>>;

	static inline loading_contents*	sThis;

	loading_contents()
	{
		sThis	= this;

		this->set_color(get_application_config()->background_color);

		auto	i	= new anchor_node<sprite_node<node>>(::gframe::load_texture("uiparts/loader-4-fill.png"));
		i->set_pivot({0.5f, 0.5f, 0});
		i->set_anchor({anchor_node_base::center, anchor_node_base::center, anchor_node_base::none});
		this->add_child(i);

		auto	c	= get_application_config()->background_color;
		i->set_color({1-c.r, 1-c.g, 1-c.b, 1});

		mIndicator	= i;

		this->schedule_once([this]()
		{
			this->set_enable(false);
		});
	}


	virtual bool on_back() override
	{
		return	true;
	}

	void on_update(float _delta) override
	{
		super_type::on_update(_delta);
		if (this->is_enable())
		{
			auto	r	= mIndicator->rotate();
			mIndicator->set_rotate(r * quaternion<value_type>::rotate_z(2.0f * 3.14f * 2 * _delta));
		}
	}

	sprite_node<node>*	mIndicator	= nullptr;
};


ref<node> contents::create_loading()
{
	return	new loading_contents();
}



void contents::loading(bool _enable)
{
	loading_contents::sThis->parent()->schedule_once([_enable]()
	{
		loading_contents::sThis->set_enable(_enable);
	});
}

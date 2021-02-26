/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <timerapp-core/node.hpp>

#include <amtrs/chrono/time.hpp>
#include <amtrs/node/anchor_node.hpp>
#include <amtrs/node/fill_node.hpp>
#include <amtrs/node/fillwrap_node.hpp>
#include <amtrs/node/flexbox_node.hpp>
#include <amtrs/node/linerlayout_node.hpp>
#include <amtrs/node/padding_node.hpp>
#include <amtrs/node/scrollbox_node.hpp>
#include <amtrs/node/string_node.hpp>

#include <timerapp-core/application.hpp>
#include <timerapp-core/contents.hpp>
#include <timerapp-core/manager/schedule.hpp>
#include <timerapp-core/manager/schedule_downloader.hpp>

using namespace ::amtrs;
using namespace ::gframe;
using namespace ::contents;



static amtrs::ref<amtrs::node> create_selector(std::string _title, amtrs::ref<amtrs::g3d::texture> _icon, std::function<void()> _trigger)
{
	auto	fnt		= ::get_font();
	auto	sz		= (float)fnt->size();

	auto*	item	= new button_node<horizontalwrap_node<children_verticalwrap_node<node>>>();
	item->set_padding({sz*2, sz*1.5f, 0}, {sz*2, sz*1.5f, 0});

	auto	icon	= new anchor_node<sprite_node<node>>(std::move(_icon));
	icon->set_anchor({anchor_node_base::none, 0.5f, anchor_node_base::none});
	icon->set_pivot({0.5f, 0.5f, 0});
	icon->set_position({sz, 0, 0});
	icon->set_color(get_application_config()->foreground_color);

	auto	txt		= new anchor_node<string_node<node>>(::get_font());
	txt->set_anchor({anchor_node_base::none, 0.5f, anchor_node_base::none});
	txt->set_pivot({0, 0.5f, 0});
	txt->set_string(std::move(_title));
	txt->set_position({sz * 3.0f, 0, 0});
	txt->set_color(get_application_config()->foreground_color);

	item->add_child(txt);
	item->set_trigger_callback([cb = std::move(_trigger)](auto pos)
	{
		cb();
	});
	item->add_child(icon);

	return	item;
}



class	saveto_list
		: public fillwrap_node<padding_node<flexbox_node<scrollbox_node<fill_node<contents::contents_base>>>>>
{
public:
	using	super_type	= fillwrap_node<padding_node<flexbox_node<scrollbox_node<fill_node<contents::contents_base>>>>>;

	virtual std::string title() override
	{
		return	u8"保存先選択";
	}


	template<class B>
	saveto_list(std::string const& _current, B&& _callback)
		: mCurrent(_current)
		, mCallback(std::move(_callback))
	{
		this->set_color(get_application_config()->settings.background_color);
		this->set_name("savetolist");
		this->set_justify(flexbox_node_base::justify_content_type::space_around);
		refresh();
	}


	void refresh()
	{
		ref<g3d::texture>	on	= ::gframe::load_texture("uiparts/check-line.png");
		ref<g3d::texture>	off	= {};

		{
			auto	n	= create_selector(schedule_manager::calendar_display_name(""), mCurrent == "" ? on : off, [=]()
			{
				mCallback("");
				this->pop();
			});
			this->add_item(n, use_scheduler{});
		}

		schedule_downloader::enumrate_source([&](auto src)
		{
			if (src->calendar && src->calendar->writable())
			{
				auto	n	= create_selector(src->title, mCurrent == src->identify ? on : off, [=]()
				{
					mCallback(src->identify);
					this->pop();
				});
				this->add_item(n, use_scheduler{});
			}
		});
	}


protected:
	std::string									mCurrent;
	std::function<void(std::string const&)>		mCallback;
};



amtrs::ref<contents::contents_base> contents::create_calendar_selector(std::string const& _current, std::function<void(std::string const&)> _callback)
{
	return	new saveto_list(_current, std::move(_callback));
}

/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/node/anchor_node.hpp>
#include <amtrs/node/back_key_node.hpp>
#include <amtrs/node/fill_node.hpp>
#include <amtrs/node/flexbox_node.hpp>
#include <amtrs/node/scene_node_stack.hpp>
#include <amtrs/node/string_node.hpp>
#include <amtrs/chrono/calendar.hpp>
#include <amtrs/chrono/time.hpp>

#include <timerapp-core/application.hpp>
#include <timerapp-core/contents.hpp>
#include <timerapp-core/system.hpp>

#include <timerapp-core/manager/notification.hpp>
#include <timerapp-core/manager/purchase.hpp>
#include <timerapp-core/manager/schedule_downloader.hpp>
#include <timerapp-core/manager/sound_manager.hpp>

using namespace amtrs;
using namespace gframe;



amtrs::ref<amtrs::font> get_font(font_type _type)
{
	static amtrs::ref<amtrs::font>	fonts[(int)font_type::__ft_max];
	if (!fonts[(int)_type])
	{
		fonts[(int)_type]	= get_application_config()->create_font(_type);
	}
	return	fonts[(int)_type];
}


amtrs::ref<amtrs::node> create_button(int _button_id, std::string _caption, std::function<void(int)> _callback)
{
	auto	btn	= get_application_config()->create_button(_button_id);
	btn->set_trigger_callback([id = _button_id, callback = std::move(_callback)](auto pos)
	{
		callback(id);
	});
	auto	cap	= new anchor_node<string_node<node>>(get_font(font_type::ft_button));
	cap->set_string(std::move(_caption));
	cap->set_pivot({0.5f, 0.5f, 0});
	cap->set_anchor({anchor_node_base::center, anchor_node_base::center, anchor_node_base::none});
	btn->add_child(cap);
	return	btn;
}


amtrs::ref<amtrs::node> create_bottom_menu(std::vector<std::string> const& _menus, int _backs_id, std::function<void(node*, int)> _callback)
{
	auto	menu	= new anchor_node<horizontalwrap_node<children_verticalwrap_node<back_key_node<flexbox_node<fill_node<node>>>>>>();
	if (_backs_id >= 0)
	{
		menu->set_back_callback([menu, _backs_id, callback = _callback]()
		{
			callback(menu, _backs_id);
			return	true;
		});
	}
	menu->set_anchor({anchor_node_base::none, anchor_node_base::maximam, anchor_node_base::none});
	menu->set_justify(flexbox_node_base::justify_content_type::space_around);

	menu->set_anchor({anchor_node_base::none, 1, anchor_node_base::none});
	menu->set_pivot({0,1,0});
	menu->set_color(get_application_config()->background_color);//{0,0,0,0.8f});
	menu->set_padding({20,20,0}, {20,60,0});

	int	index	= 0;
	for (auto& m : _menus)
	{
		if (!m.empty())
		{
			menu->add_item(create_button(index, m, [menu, callback = _callback](int _index)
			{
				callback(menu, _index);
			}));
		}
		++index;
	}
	return	menu;
}


static application_configs	sConfig;



application_configs const* get_application_config()
{
	return	&sConfig;
}



void gframe_application_created()
{
}


void gframe_application_resume()
{
	alarm_notification_manager::clean();
}


void gframe_application_pause()
{
	alarm_notification_manager::setup();
}


void gframe_application_stop()
{
}


void system_intialize()
{
	AMTRS_DEBUG_LOG("system_intialize() --------------------------------------------------------- debug");

	locale::initialize();
	locale::add_locale_text("locale/basic.%s");
	locale::add_locale_text("locale/timer-core.%s");
	locale::add_locale_text("locale/appli.%s");
	schedule_downloader::initialize();
	bg::initialize("bg/config.json");
	sound::initialize("voice/config.json");
	sound_manager::initialize();

	purchase::initialize("shop/product.info");


	save::storage::set_current(new save::storage(amtrs::application::get_application()->files_dir() + "/save.bin"));
	save::storage::current()->load();


	application::get_application()->set_sleep_enable(!save::storage::current()->setting.energy.none_sleep());


	// モジュールの初期化
	sConfig.background_color			= {1, 1, 1, 0.5f};
	sConfig.sound						= [](sound_types) -> std::string { return ""; };
	sConfig.create_font					= [](font_type) -> amtrs::ref<amtrs::font> { return amtrs::font::create(nullptr, 60, {255,255,255,255}); };
	sConfig.create_button				= [](int id) -> amtrs::ref<amtrs::button_node<amtrs::node>>
	{
		auto	btn	= new children_wrap_node<button_node<node>>();
		btn->set_padding({30, 20, 0}, {30, 20, 0});
		return	btn;
	};
	sConfig.settings.background_color	= sConfig.background_color;
	initialize_application_config(&sConfig);
}




void contents_stop()
{
	schedule_downloader::pause();
	sound_manager::clean();
}


#include <amtrs/main/@>
AMTRS_GUI_MAIN(gframe::gframe_application())

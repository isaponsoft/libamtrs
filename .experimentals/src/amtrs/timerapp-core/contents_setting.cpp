/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <timerapp-core/node.hpp>

#include <amtrs/application/application.hpp>
#include <amtrs/node/anchor_node.hpp>
#include <amtrs/node/flexbox_node.hpp>
#include <amtrs/node/fillwrap_node.hpp>
#include <amtrs/node/linerlayout_node.hpp>
#include <amtrs/node/scrollbox_node.hpp>
#include <amtrs/node/slider_bar_node.hpp>
#include <amtrs/node/string_node.hpp>
#include <amtrs/media/image.hpp>
#include <gframe/@>

#include <timerapp-core/application.hpp>
#include <timerapp-core/contents.hpp>
#include <timerapp-core/system.hpp>

#include <timerapp-core/manager/sound_manager.hpp>


using namespace ::amtrs;
using namespace ::gframe;
using namespace ::ui;


class	bg_selector_node
		: public string_node<node>
{
public:
	using	super_type	= string_node<node>;

	bg_selector_node(std::string _txt)
		: super_type(std::move(_txt), ::get_font())
	{
	}
};


class	switch_edit_node
		: public button_node<sprite_node<node>>
{
public:
	using	super_type	= button_node<sprite_node<node>>;

	switch_edit_node(bool _flag)
		: mFlag(_flag)
	{
//		this->set_anchor({anchor_node_base::maximam, anchor_node_base::center, anchor_node_base::none});
		this->set_pivot({1, 0.5f, 0});

		update_texture();
		this->set_trigger_callback([this](auto)
		{
			mFlag	= !mFlag;
			update_texture();
		});
	}


	void update_texture()
	{
		this->set_texture(load_texture(mFlag ? "ui/switch-on.png" : "ui/switch-off.png"));
	}


private:
	bool	mFlag;
};


class	setting_contents
		: public scrollbox_node<fillwrap_node<padding_node<flexbox_node<fill_node<contents::contents_base>>>>>
{
public:
	using	super_type	= scrollbox_node<fillwrap_node<padding_node<flexbox_node<fill_node<contents::contents_base>>>>>;

	virtual std::string title() override
	{
		return	locale::gettext("settings.title", "SETTING");
	}

	setting_contents()
	{
		this->set_name("setting");
		this->set_color(get_application_config()->settings.background_color);
		this->set_padding({0,20,0}, {0,20,0});
		this->set_spacing({0, 30});


		this->async_schedule("setting", [this]()
		{
			// ----------------------------------------------------------
			// デザインの変更
			this->add_item(create_setting_group(locale::gettext("settings.design.title", "DESIGN")), use_scheduler{});


			// ----------------------------------------------------------
			// 背景選択
			if (use_setting(setting_configs::st_bg_images))
			{
				auto	itm = create_setting_sub_contents(locale::gettext("settings.design.backgound", "BG"), locale::gettext("settings.design.backgound.note", "Change bg image"));
				itm->set_trigger_callback([this](auto pos)
				{
					this->set_contents(contents::bg_selector_list::create(
						[](bg::item const& _item) -> unsigned int
						{
							unsigned int	flg	= 0;
							if (save::storage::current()->setting.looks.bgimage == _item.filename)
							{
								flg |= bgitem_mode_selected;
							}
							if (!_item.productId.empty() && !save::storage::current()->purchase.is_purchased(_item.productId))
							{
								flg |= bgitem_mode_locked;
							}
							return	flg;
						},
						[](bg::item const& _item, unsigned int _flg)
						{
							if (!(_flg & bgitem_mode_selected) && !(_flg & bgitem_mode_locked))
							{
								// 背景が選択されたときはカレントストレージの背景を変更する
								save::storage::current()->setting.looks.mode	= save::setting_looks::bgmode_image;
								save::storage::current()->setting.looks.bgimage	= _item.filename;
								wallpaper::update();
								return	true;
							}
							return	(_flg & bgitem_mode_locked) ? false : true;
						})
					);
				});
				this->add_item(itm, use_scheduler{});
			}


			if (!bg::has_bg() && use_setting(setting_configs::st_bg_with_storages))
			{
				auto	itm = create_setting_sub_contents(locale::gettext("settings.design.backgoundfile", "BG"), locale::gettext("settings.design.backgoundfile.note", "Select bg image"));
				itm->set_trigger_callback([this](auto pos)
				{
#if	1
					auto	mi	= media::image::create();
					mi->pickup([=](auto _name)
					{
						if (!_name.empty())
						{
							save::storage::current()->setting.looks.mode	= save::setting_looks::bgmode_image;
							save::storage::current()->setting.looks.bgimage	= _name;
							wallpaper::update();
						}
					});
#else

					this->set_contents(contents::bg_file_selector::create({".png", ".jpg", ".jpeg"},
						[](std::string const& _file) -> bool
						{
							save::storage::current()->setting.looks.mode	= save::setting_looks::bgmode_image;
							save::storage::current()->setting.looks.bgimage	= _file;
							wallpaper::update();
							return	true;
						})
					);
#endif
				});
				this->add_item(itm, use_scheduler{});
			}


	#if	0
			// ----------------------------------------------------------
			// スケジュール表示
			{
				auto	box	= create_setting_switch(u8"スケジュールの表示", u8"ＯＮの時はホーム画面に直近のスケジュールを表示します。");
				this->add_item(box);
			}
	#endif


			// ----------------------------------------------------------
			// 透明度変更
			if (auto itm = create_setting_sub_contents(locale::gettext("settings.design.transpalency", "Transpalency"), locale::gettext("settings.design.transpalency.note", "Change Transpalency")); itm)
			{
				itm->set_trigger_callback([this](auto pos)
				{
					this->set_contents(contents::setting::create_transpalency(save::storage::current()->setting.looks.transpalency, [](int _alpha)
					{
						save::storage::current()->setting.looks.transpalency = _alpha;
					}));
				});
				this->add_item(itm, use_scheduler{});
			}

			// 秒の表示のON/OFF
			if (use_setting(setting_configs::st_schedule_show_seconds))
			{
				auto	itm	= create_setting_switch(locale::gettext("settings.looks.show_seconds")
								, locale::gettext("settings.looks.show_seconds.note")
								, save::storage::current()->setting.looks.schedule_show_seconds()
								, [](auto flg) { save::storage::current()->setting.looks.schedule_show_seconds(flg); }
								);
				this->add_item(itm, use_scheduler{});
			}

			// ----------------------------------------------------------
			// イベントとスケジュール
			this->add_item(create_setting_splitbar(), use_scheduler{});
			this->add_item(create_setting_group(locale::gettext("settings.event.title", "EVENTS & SCHEDULES")), use_scheduler{});

			// アラームの無効化
			{
				auto	itm	= create_setting_switch(locale::gettext("settings.event.alarm-disable")
								, locale::gettext("settings.event.alarm-disable.note")
								, save::storage::current()->setting.alarm_disable()
								, [](auto flg) { save::storage::current()->setting.alarm_disable(flg); }
								);
				this->add_item(itm, use_scheduler{});
			}


			// イベントソース
			{
				auto	itm = create_setting_sub_contents(locale::gettext("settings.event.sources", "Events sources"), locale::gettext("settings.event.sources.note", "Change bg image"));
				itm->set_trigger_callback([this](auto pos)
				{
					this->set_contents(contents::event_sources_selector::create());
				});
				this->add_item(itm, use_scheduler{});
			}


			// ----------------------------------------------------------
			// 音声の設定
			this->add_item(create_setting_splitbar(), use_scheduler{});
			this->add_item(create_setting_group(locale::gettext("settings.sound.title", "SOUND")), use_scheduler{});

			{
				auto	itm	= create_setting_switch(locale::gettext("settings.sound.system", "System sound")
								, locale::gettext("settings.sound.system.note", "Enable system sound")
								, save::storage::current()->setting.sound.systemSound
								, [](auto flg) { save::storage::current()->setting.sound.systemSound = flg; sound_manager::stop_all(); }
								);
				this->add_item(itm, use_scheduler{});
			}


			if (use_setting(setting_configs::st_home_sound))
			{
				auto	itm	= create_setting_switch(locale::gettext("settings.sound.home", "Home sound")
								, locale::gettext("settings.sound.home.note", "Enable home sound")
								, save::storage::current()->setting.sound.homeTouchSound
								, [](auto flg) { save::storage::current()->setting.sound.homeTouchSound = flg; sound_manager::stop_all(); }
								);
				this->add_item(itm, use_scheduler{});
			}

			// ----------------------------------------------------------
			// システム
			this->add_item(create_setting_splitbar(), use_scheduler{});
			this->add_item(create_setting_group(locale::gettext("settings.system.title")), use_scheduler{});
			// スリープ
			{
				auto	itm	= create_setting_switch(locale::gettext("settings.system.sleep")
								, locale::gettext("settings.system.sleep.note")
								, save::storage::current()->setting.energy.none_sleep()
								, [](auto flg) { save::storage::current()->setting.energy.none_sleep(flg); application::get_application()->set_sleep_enable(!flg); }
								);
				this->add_item(itm, use_scheduler{});
			}


			this->add_item(create_setting_splitbar(), use_scheduler{});
			this->add_item(create_setting_group(locale::gettext("settings.about.title", "ABOUT")), use_scheduler{});

			if (auto itm = create_setting_sub_contents(locale::gettext("settings.about.license", "ABOUT"), locale::gettext("settings.about.license.note", "ABOUT")); itm)
			{
				itm->set_trigger_callback([this](auto pos)
				{
					this->set_contents(contents::about::create());
				});
				this->add_item(itm, use_scheduler{});
			}
		});
	}


	virtual int on_containor(contents::contents_containor* _containor, contents::contents_containor::message_id _msg) override
	{
		if (_msg == contents::contents_containor::removing)
		{
			// セーブ
			save::storage::current()->save();
			sound_manager::play_sys(get_application_config()->sound(sound_types::setting_complite));
		}
		return	super_type::on_containor(_containor, _msg);
	}

	bg_selector_node*					mBGSelector	= nullptr;
};



amtrs::ref<contents::contents_base> contents::setting::create()
{
	return	new setting_contents();
}

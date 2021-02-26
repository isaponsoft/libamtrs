/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <timerapp-core/node.hpp>

#include <amtrs/chrono/time.hpp>
#include <amtrs/node/node.hpp>
#include <amtrs/node/fill_node.hpp>
#include <amtrs/node/fillwrap_node.hpp>
#include <amtrs/node/flexbox_node.hpp>
#include <amtrs/node/linerlayout_node.hpp>
#include <amtrs/node/padding_node.hpp>
#include <amtrs/node/scrollbox_node.hpp>
#include <amtrs/node/string_node.hpp>

#include <timerapp-core/application.hpp>
#include <timerapp-core/contents.hpp>
#include <timerapp-core/manager/schedule_downloader.hpp>
#include <timerapp-core/manager/sound_manager.hpp>

using namespace ::amtrs;
using namespace ::gframe;
using namespace ::contents;



// ****************************************************************************
//! bgフォルダ内の背景画像を選択します。
// ----------------------------------------------------------------------------
//! 選択された項目をコールバックで返します。
// ----------------------------------------------------------------------------
class	event_sources_contents
		: public fillwrap_node<padding_node<flexbox_node<scrollbox_node<fill_node<event_sources_selector>>>>>
{
public:
	using	super_type	= fillwrap_node<padding_node<flexbox_node<scrollbox_node<fill_node<event_sources_selector>>>>>;

	event_sources_contents()
	{
		this->set_color(get_application_config()->settings.background_color);
		this->set_padding({10, 10, 0}, {10, 0, 0});
		this->set_spacing({20, 20});
		this->set_name("eventsorces");

		this->set_justify(flexbox_node_base::justify_content_type::space_around);

		refresh();
	}

	virtual std::string title() override
	{
		return	locale::gettext("settings.event.sources", "Event Sources");
	}

	struct	element
	{
		setting_switch_node*	switchnode	= nullptr;
		schedule_source*		source		= nullptr;
	};
	std::list<element>	mElements;

	void refresh()
	{
		mElements	= {};

		schedule_downloader::enumrate_source([this](auto* _source)
		{
			this->add_item(create_setting_splitbar(), use_scheduler{});
			this->add_item(create_setting_group(_source->title), use_scheduler{});


			auto	skipOld	= create_setting_switch(locale::gettext("settings.event.sources.enable", "Enable")
							, locale::gettext("settings.event.sources.enable.note", "Enable source")
							, _source->enable()
							, [_source](auto flg) { _source->enable(flg); }
							);
			this->add_item(skipOld, use_scheduler{});

			auto	enable	= create_setting_switch(locale::gettext("settings.event.sources.old", "Visible old")
							, locale::gettext("settings.event.sources.old.note", "Old event&schedule visiblity")
							, _source->older()
							, [_source](auto flg) { _source->older(flg); }
							);
			this->add_item(enable, use_scheduler{});
/*
			if (_source->calendar && _source->calendar->writable())
			{
				auto	defwrite	=  create_setting_switch(locale::gettext("settings.event.sources.default")
								, locale::gettext("settings.event.sources.default.note")
								, _source->save_default()
								, [this, _source](auto flg)
									{
										_source->save_default(flg);
										if (flg)
										{
											// ほかにデフォルト書き込みが有効になっているものを無効にする
											for (auto& e : mElements)
											{
												if (e.source != _source)
												{
													e.switchnode->set_flag(false);
												}
											}
										}
									}
								);
				this->add_item(defwrite, use_scheduler{});
				mElements.push_back({
					defwrite,
					_source
				});
			}
*/
		});
	}

	virtual int on_containor(contents::contents_containor* _containor, contents::contents_containor::message_id _msg) override
	{
		AMTRS_DEBUG_LOG("Event source on_containor");
		if (_msg == contents::contents_containor::removing)
		{
			// セーブ
			schedule_downloader::save();
			sound_manager::play_sys(get_application_config()->sound(sound_types::setting_complite));
		}
		return	super_type::on_containor(_containor, _msg);
	}

};



amtrs::ref<contents::contents_base> contents::event_sources_selector::create()
{
	return	new event_sources_contents();
}

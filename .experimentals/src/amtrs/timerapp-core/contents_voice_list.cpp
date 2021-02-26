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
#include <amtrs/media/audio.hpp>

#include <timerapp-core/application.hpp>
#include <timerapp-core/contents.hpp>

using namespace ::amtrs;
using namespace ::gframe;
using namespace ::ui;
using namespace ::contents;



// ****************************************************************************
//! bgフォルダ内の背景画像を選択します。
// ----------------------------------------------------------------------------
//! 選択された項目をコールバックで返します。
// ----------------------------------------------------------------------------
class	voice_list_impl
		: public fillwrap_node<padding_node<flexbox_node<scrollbox_node<fill_node<voice_list>>>>>
{
public:
	using	super_type	= fillwrap_node<padding_node<flexbox_node<scrollbox_node<fill_node<voice_list>>>>>;

	virtual std::string title() override
	{
		return	locale::gettext("sound-select.title");
	}

	template<class A, class B>
	voice_list_impl(A&& _status, B&& _confirmcallback)
		: mItemStatus(std::move(_status))
		, mConfirm(std::move(_confirmcallback))
	{
		this->set_color(get_application_config()->settings.background_color);
		this->set_name("voiceselector");

		this->set_justify(flexbox_node_base::justify_content_type::space_around);

		refresh();
	}


	void filepickup()
	{
#if	1
		auto	m	= media::audio::create();
		m->pickup([=](auto _filename)
		{
			if (mConfirm(_filename, 0))
			{
				this->pop();
			}
		});
#else
		this->set_contents(bg_file_selector::create({".ogg"}, [this](std::string const& _filename) -> bool
		{
			if (mConfirm(_filename, 0))
			{
				this->pop();
			}
			return	false;
		}));
#endif
	}

	void refresh()
	{
		{
			auto	c	= new horizontalwrap_node<button_node<children_wrap_node<node>>>();
			auto	n	= create_sounditem_node(locale::gettext("sound-select.none"), 0);
			c->add_child(n);
			c->set_trigger_callback([=](auto)
			{
				if (mConfirm("", 0))
				{
					this->pop();
				}
			});
			this->add_item(c, use_scheduler{});
		}

		// アプリ内にサウンドデータが存在しない場合はファイル選択を開く
		if (!sound::has_sound())
		{
			filepickup();
			return;
		}

#if	0
		// ファイルピックアップメニュー
		auto	c	= new horizontalwrap_node<button_node<children_wrap_node<node>>>();
		auto	n	= create_sounditem_node(locale::gettext("sound-select.file"), bgitem_mode_storage);
		c->add_child(n);
		c->set_trigger_callback([=](auto)
		{
			filepickup();
		});
		this->add_item(c, use_scheduler{});
#endif

		// アプリ内のサウンド一覧を表示
		for (auto itm : sound::enum_items())
		{
			this->async_schedule(itm.second->filename, [this, itm]()
			{
				auto	c	= new horizontalwrap_node<button_node<children_wrap_node<node>>>();
				auto	f	= mItemStatus(*itm.first, *itm.second);
				auto	n	= create_sounditem_node(itm.second->title, f);
				c->add_child(n);
				c->set_trigger_callback([=](auto)
				{
					if (mConfirm(itm.second->filename, f))
					{
						this->pop();
					}
				});

				this->add_item(c, use_scheduler{});
			});
		}
	}


protected:
	std::function<unsigned int(sound::group const&, sound::item const&)>		mItemStatus;
	std::function<bool(std::string const&, unsigned int)>						mConfirm;
};



amtrs::ref<contents::contents_base> contents::voice_list::create(std::function<unsigned int(sound::group const&, sound::item const&)> _itemstatus, std::function<bool(std::string const&, unsigned int)> _confirmcallback)
{
	return	new voice_list_impl(std::move(_itemstatus), std::move(_confirmcallback));
}

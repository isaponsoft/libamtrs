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

using namespace ::amtrs;
using namespace ::gframe;
using namespace ::contents;



// ****************************************************************************
//! bgフォルダ内の背景画像を選択します。
// ----------------------------------------------------------------------------
//! 選択された項目をコールバックで返します。
// ----------------------------------------------------------------------------
class	bg_selector_list_contents
		: public fillwrap_node<padding_node<flexbox_node<scrollbox_node<fill_node<bg_selector_list>>>>>
{
public:
	using	super_type	= fillwrap_node<padding_node<flexbox_node<scrollbox_node<fill_node<bg_selector_list>>>>>;

	template<class A, class B>
	bg_selector_list_contents(A&& _status, B&& _confirmcallback)
		: mItemStatus(std::move(_status))
		, mConfirm(std::move(_confirmcallback))
	{
		this->set_color(get_application_config()->settings.background_color);
		this->set_padding({10, 10, 0}, {10, 0, 0});
		this->set_spacing({20, 20});
		this->set_name("bgselector");

		this->set_justify(flexbox_node_base::justify_content_type::space_around);

		refresh();
	}

	virtual std::string title() override
	{
		return	locale::gettext("settings.design.backgound", "BG");
	}

	void refresh()
	{
		for (auto* itm : bg::enum_items())
		{
			this->async_schedule(itm->filename, [this, itm]()
			{
				auto	c	= new button_node<children_wrap_node<node>>();
				auto	tex	= load_texture(bg::prev_filename(itm->filename));
				auto	f	= mItemStatus(*itm);
				auto	n	= create_bgitem_node(std::move(tex), f);
				c->add_child(n);
				c->set_trigger_callback([=](auto)
				{
					if (mConfirm(*itm, f))
					{
						this->pop();
					}
				});

				this->schedule_once([this, c](float)
				{
					this->add_item(c);
				});
			});
		}
	}//, unsigned int _modes

protected:
	std::function<unsigned int(bg::item const&)>		mItemStatus;
	std::function<bool(bg::item const&, unsigned int)>	mConfirm;
};



amtrs::ref<contents::contents_base> contents::bg_selector_list::create(std::function<unsigned int(bg::item const&)> _itemstatus, std::function<bool(bg::item const&, unsigned int)> _confirmcallback)
{
	return	new bg_selector_list_contents(std::move(_itemstatus), std::move(_confirmcallback));
}

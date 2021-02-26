/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <timerapp-core/node.hpp>
#include <amtrs/chrono/time.hpp>
#include <amtrs/node/node.hpp>
#include <amtrs/node/anchor_node.hpp>
#include <amtrs/node/fill_node.hpp>
#include <amtrs/node/fillwrap_node.hpp>
#include <amtrs/node/flexbox_node.hpp>
#include <amtrs/node/linerlayout_node.hpp>
#include <amtrs/node/scrollbox_node.hpp>
#include <amtrs/node/string_node.hpp>
#include <amtrs/node/text_node.hpp>
#include <amtrs/node/vertical_list_node.hpp>
#include <amtrs/net/web.hpp>

#include <timerapp-core/application.hpp>
#include <timerapp-core/contents.hpp>

using namespace ::amtrs;
using namespace ::gframe;
using namespace ::ui;


struct	aboutdata
{
	int			identify;
	std::string	title;
	std::string	license;
	std::string	license_details;
};


std::vector<aboutdata> aboutdatalist()
{
	std::vector<aboutdata>	retval;
	retval.push_back({
		1,
		std::string(u8"libamtrs"),
		std::string(u8"BSD License"),
		#include "license/license-libamtrs.lxx"
	});

	retval.push_back({
		1,
		std::string(u8"REMIX ICON"),
		std::string(u8"Apache License Version 2.0"),
		#include "license/license-remixicon.lxx"
	});

	retval.push_back({
		1,
		std::string(u8"zlib"),
		std::string(u8"zlib License"),
		#include "license/license-zlib.lxx"
	});

	retval.push_back({
		1,
		std::string(u8"libpng"),
		std::string(u8"PNG Reference Library License version 2"),
		#include "license/license-libpng.lxx"
	});
	retval.push_back({
		1,
		std::string(u8"libressl"),
		std::string(u8"OpenSSL License"),
		#include "license/license-libressl.lxx"
	});
	retval.push_back({
		1,
		std::string(u8"libogg"),
		std::string(u8"BSD License"),
		#include "license/license-libogg.lxx"
	});
	retval.push_back({
		1,
		std::string(u8"libvorbis"),
		std::string(u8"BSD License"),
		#include "license/license-libvorbis.lxx"
	});
	return	retval;
}


class	contents_abount
		: public scrollbox_node<flexbox_node<fill_node<contents::about>>>
{
public:
	using	super_type	= scrollbox_node<flexbox_node<fill_node<contents::about>>>;

	virtual std::string title() override
	{
		return	u8"権利・免責事項など";
	}

	class	license_node
			: public button_node<horizontalwrap_node<children_verticalwrap_node<node>>>
	{
	public:
		using	super_type	= button_node<horizontalwrap_node<children_verticalwrap_node<node>>>;

		license_node(aboutdata _about)
			: mAbout(std::move(_about))
		{
			this->set_padding({40,40,0}, {40,40,0});

			mTitle		= new string_node<node>(get_font(font_type::ft_large));
			mTitle->set_string(mAbout.title);
			mTitle->string_update();
			this->add_child(mTitle);
		
			mLicecse	= new string_node<node>(get_font());
			mLicecse->set_string(mAbout.license);
			mLicecse->string_update();
			this->add_child(mLicecse);

			mOpenClose	= new sprite_node<node>(::gframe::load_texture("ui/arrow-up-s.png"));
			this->add_child(mOpenClose);
			mOpenClose->set_pivot({1, 0, 0});

			auto	col	= get_application_config()->settings.background_color;
			mOpenClose->set_color({1.0f-col.r,1.0f-col.g,1.0f-col.b,1.0f});

			this->set_trigger_callback([this](auto)
			{
				openclose();
			});
		}

		void openclose()
		{
			if (mDetails)
			{
				close();
			}
			else
			{
				contents::loading(true);
				static_cast<contents_abount*>(this->parent())->close_all();
				mOpenClose->set_texture(::gframe::load_texture("ui/arrow-down-s.png"));
				this->async_schedule("", [this]()
				{

					mDetails	= new horizontalwrap_node<text_node<node>>(get_font());
					mDetails->set_string(mAbout.license_details);
					mDetails->force_update();
					this->schedule_once([this]()
					{
						this->add_child(mDetails);
						reposition();
						contents::loading(false);
					});
				});
			}
		}

		void close()
		{
			if (mDetails)
			{
				mDetails->remove_from_parent();
				mDetails	= nullptr;
				mOpenClose->set_texture(::gframe::load_texture("ui/arrow-up-s.png"));
				reposition();
			}
		}


		void on_size_change(typename super_type::size_type& _size) override
		{
			super_type::on_size_change(_size);
			reposition();
		}

		void reposition()
		{
			auto	sz	= this->children_area();

			mOpenClose->set_position({sz.width, 0, 0});

			float	y	= 0;
			mTitle->set_position({0, y, 0});
			y	+= mTitle->size().height + 30;

			mLicecse->set_position({0, y, 0});
			y	+= mLicecse->size().height + 30;

			if (mDetails)
			{
				mDetails->set_position({0, y, 0});
			}
		}
		

		aboutdata				mAbout;
		sprite_node<node>*		mOpenClose	= nullptr;
		string_node<node>*		mTitle		= nullptr;
		string_node<node>*		mLicecse	= nullptr;
		text_node<node>*		mDetails	= nullptr;
	};


	class	staff_node
			: public horizontalwrap_node<children_verticalwrap_node<node>>
	{
	public:
		using	super_type	= horizontalwrap_node<children_verticalwrap_node<node>>;

		staff_node(staff _staff)
		{
			this->set_padding({40,40,0}, {40,40,0});

			auto	c		= new horizontalwrap_node<vertical_list_node<node>>();
			c->set_auto_resize(true);

			auto	roll	= new anchor_node<string_node<node>>(get_font());
			roll->set_string(_staff.roll);
			c->add_item(roll);

			auto	nc		= new button_node<children_verticalwrap_node<horizontalwrap_node<node>>>();
			nc->set_padding({0, 30, 0}, {0, 30, 0});
			c->add_item(nc);
			auto	n		= new string_node<node>(get_font(font_type::ft_large));
			n->set_string(_staff.name);
			nc->add_child(n);

			nc->set_trigger_callback([_staff](auto)
			{
				if (!_staff.url.empty())
				{
					web::jump_to(_staff.url);
				}
			});

			this->add_child(c);
		}
	};


	contents_abount()
	{
		this->set_color(get_application_config()->settings.background_color);

		for (auto& staff : create_stafflist())
		{
			auto	item	= new staff_node(staff);
			this->add_item(item);
		}

		for (auto& ad : aboutdatalist())
		{
			auto	item	= new license_node(ad);
			this->add_item(item);
			mItems.push_back(item);
		}

		this->schedule_once([this]()
		{
			for (auto& itm : mItems)
			{
				itm->reposition();
			}
		});
	}

	void close_all()
	{
		for (auto& itm : mItems)
		{
			itm->close();
		}
	}

protected:
	std::vector<license_node*>			mItems;
};



amtrs::ref<contents::contents_base> contents::about::create()
{
	return	new contents_abount();
}

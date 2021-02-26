/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <timerapp-core/node.hpp>

#include <amtrs/application/@>
#include <amtrs/chrono/time.hpp>
#include <amtrs/filesystem/permission.hpp>
#include <amtrs/node/anchor_node.hpp>
#include <amtrs/node/back_key_node.hpp>
#include <amtrs/node/fill_node.hpp>
#include <amtrs/node/fillwrap_node.hpp>
#include <amtrs/node/flexbox_node.hpp>
#include <amtrs/node/linerlayout_node.hpp>
#include <amtrs/node/padding_node.hpp>
#include <amtrs/node/scrollbox_node.hpp>
#include <amtrs/node/string_node.hpp>

#include <timerapp-core/application.hpp>
#include <timerapp-core/contents.hpp>
#include <timerapp-core/save/storage.hpp>


#include <compati-stl/filesystem/filesystem>

using namespace ::amtrs;
using namespace ::gframe;
using namespace ::contents;

namespace fs	= amtrs::compati::filesystem;


amtrs::ref<amtrs::node> create_basic_selector(std::string _title, amtrs::ref<amtrs::g3d::texture> _icon, std::function<void()> _trigger)
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




class	selector_contents
		: public back_key_node<fillwrap_node<padding_node<flexbox_node<scrollbox_node<fill_node<bg_file_selector>>>>>>
{
public:
	using	super_type	= back_key_node<fillwrap_node<padding_node<flexbox_node<scrollbox_node<fill_node<bg_file_selector>>>>>>;

	selector_contents(std::initializer_list<std::string> _extras, std::function<bool(std::string const&)> _select)
		: mFilter(_extras)
		, mSelectCallback(std::move(_select))
	{
		this->set_color(get_application_config()->settings.background_color);
		this->set_padding({10, 10, 0}, {10, 0, 0});
		this->set_spacing({20, 20});
		this->set_name("bgselector");

		this->set_justify(flexbox_node_base::justify_content_type::space_around);

		mRoute.push_back(std::string(amtrs::application::get_application()->documents_dir()));
		mParentIcon		= load_texture("uiparts/arrow-up-line.png");
		mFoldarIcon		= load_texture("uiparts/folder-line.png");
		mPictureIcon	= load_texture("uiparts/image-line.png");
		amtrs::filesystem::permission([this](auto ok)
		{
			if (ok) { refresh(); }
			else	{ pop(); }
		});

	}


	virtual std::string title() override
	{
		return	locale::gettext("settings.design.backgoundfile");
	}


	virtual bool on_back() override
	{
		auto	r	= super_type::on_back();
		if (r)
		{
			return	r;
		}

		if (mRoute.size() <= 1)
		{
			return	false;
		}

		mRoute.pop_back();
		refresh();
		return	true;
	}

	void route(std::string _path)
	{
		if (!mRoute.empty() && mRoute.back() == _path)
		{
			// 元の場所に戻った
			mRoute.pop_back();
		}
		else
		{
			// 新しい場所に来た
			mRoute.push_back(_path);
		}
	}


	void refresh()
	{
		auto	cdir	= mRoute.back();
		AMTRS_DEBUG_LOG("Refrefsh \"%s\"", cdir.c_str());
		auto	items	= this->items();
		for (auto& item : items)
		{
			item->remove_from_parent();
		}


		this->update_title(cdir);

		if (cdir != "/")
		{
			std::error_code	ec;
			fs::directory_iterator(amtrs::filesystem::parent_path(cdir), ec);
			if (!ec)
			{
				this->add_item(create_basic_selector("..", mParentIcon, [this]()
				{
					route(std::string(amtrs::filesystem::parent_path(mRoute.back())));
					this->schedule_once([this]()
					{
						refresh();
					});
				}));
			}
		}
		std::error_code	ec;
		auto			di	= fs::directory_iterator(cdir, ec);
		if (!ec)
		{
			for (auto file : di)
			{
				auto	path	= file.path().string();
				AMTRS_DEBUG_LOG("... %s", path.c_str());

				if (file.is_directory())
				{
					this->add_item(create_basic_selector(std::string(filesystem::filename(path)) + "/", mFoldarIcon, [=]()
					{
						route(path);
						this->schedule_once([this]()
						{
							refresh();
						});
					}));
				}
				else if (file.is_regular_file())
				{
					auto	extr	= filesystem::extension(path);
					auto	m		= std::find_if(mFilter.begin(), mFilter.end(), [&](auto e) { return e == extr; });
					if (m != mFilter.end())
					{
						this->add_item(create_basic_selector(std::string(filesystem::filename(path)), mPictureIcon, [=]()
						{
							mSelectCallback(path);
						}));
					}
				}
			}
		}
	}

	
	ref<g3d::texture>						mParentIcon;
	ref<g3d::texture>						mFoldarIcon;
	ref<g3d::texture>						mPictureIcon;
	std::list<std::string>					mRoute;
	std::vector<std::string>				mFilter;
	std::function<bool(std::string const&)> mSelectCallback;
};



amtrs::ref<contents_base> bg_file_selector::create(std::initializer_list<std::string> _extras, std::function<bool(std::string const&)> _select)
{
	return	new selector_contents(std::move(_extras), std::move(_select));
}

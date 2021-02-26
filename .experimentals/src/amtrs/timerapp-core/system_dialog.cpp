/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/node/anchor_node.hpp>
#include <amtrs/node/back_key_node.hpp>
#include <amtrs/node/fill_node.hpp>
#include <amtrs/node/fillwrap_node.hpp>
#include <amtrs/node/flexbox_node.hpp>
#include <amtrs/node/input_untransparentable_node.hpp>
#include <amtrs/node/string_node.hpp>
#include <amtrs/node/text_node.hpp>
#include <amtrs/node/vertical_list_node.hpp>

#include <timerapp-core/application.hpp>
#include <timerapp-core/contents.hpp>
#include <timerapp-core/system.hpp>

namespace dialog {

using namespace amtrs;



class	dialogbox
		: public input_untransparentable_node<back_key_node<fillwrap_node<fill_node<node>>>>
{
public:
	static inline dialogbox*	sThiz;

	dialogbox()
	{
		auto	col	= get_application_config()->background_color;

		sThiz	= this;
		this->set_color(col);
		this->schedule_once([this]()
		{
			this->set_enable(false);
		});
	}


	void setup(dialog_params const& _dp, std::function<void(int)> _callback)
	{
		release();
		if (_dp.flags & dialog_params::use_color)
		{
			this->set_color(_dp.color);
		}
		else
		{
			this->set_color(get_application_config()->background_color);
		}

		mCallback	= std::move(_callback);
		mBackId		= _dp.back;

		auto	ps	= this->parent()->children_area();
		auto	wi	= ps.width * 0.8f;

		auto	cnt	= new anchor_node<vertical_list_node<node>>();
		cnt->set_anchor({anchor_node_base::center, anchor_node_base::center, anchor_node_base::none});
		cnt->set_pivot({0.5f, 0.5f, 0});
		cnt->set_auto_resize(true);
		cnt->set_width(wi);
		mContainor	= this->add_child(cnt);

		if (!_dp.title.empty())
		{
			auto	titlebox	= new horizontalwrap_node<children_verticalwrap_node<node>>();
			titlebox->set_padding({0, 40, 0}, {0, 40, 0});
			mTitle	= this->add_child(titlebox);

			auto	title		= new anchor_node<string_node<node>>(get_font());
			title->set_anchor({anchor_node_base::center, anchor_node_base::center, anchor_node_base::none});
			title->set_pivot({0.5f, 0.5f, 0});
			title->set_string(_dp.title);
			titlebox->add_child(title, use_scheduler());
		}

		if (_dp.custom)
		{
			cnt->add_item(_dp.custom);
		}

		if (!_dp.text.empty())
		{
			auto	text		= new anchor_node<text_node<node>>(get_font());
			text->set_width_adjust(ps.width * 0.6f);
			text->set_anchor({anchor_node_base::center, anchor_node_base::none, anchor_node_base::none});
			text->set_pivot({0.5f, 0, 0});
			text->set_string(_dp.text);
			mText	= cnt->add_item(text, use_scheduler());
		}

		mMenu	= this->add_child(create_bottom_menu(_dp.buttons, mBackId, [this](auto sender, int i)
		{
			this->close(i);
		}));

		update_size();
		this->set_enable(true);
	}


	void close(int _buttonId)
	{
		this->schedule_once([=]()
		{
			release();
			this->set_enable(false);
			this->mCallback(_buttonId);
		});
	}

	void release()
	{
		if (mTitle)
		{
			mTitle->remove_from_parent();
			mTitle	= nullptr;
		}
		mText	= nullptr;
		if (mContainor)
		{
			mContainor->remove_from_parent();
			mContainor	= nullptr;
		}
		for (auto& b : mButtons)
		{
			b->remove_from_parent();
		}
		mButtons	= {};
		if (mMenu)
		{
			mMenu->remove_from_parent();
			mMenu	= nullptr;
		}
	}


	virtual bool on_back() override
	{
		if (mBackId >= 0)
		{
			close(mBackId);
		}
		return	true;
	}

	virtual void on_size_change(typename node::size_type& _size) override
	{
		super_type::on_size_change(_size);
		update_size();
	}

	void update_size()
	{
		if (mText)
		{
			mText->set_width(this->children_area().width * 0.8f);
		}

		float	w	= this->children_area().width / (float)(1 + mButtons.size());
		for (auto& btn : mButtons)
		{
			btn->set_width(w);
		}
	}

	std::function<void(int)>	mCallback;
	node*						mContainor	= nullptr;
	node*						mCustom		= nullptr;
	node*						mMenu		= nullptr;
	node*						mTitle		= nullptr;
	text_node<node>*			mText		= nullptr;
	std::vector<node*>			mButtons;
	int							mBackId;
};



void show(dialog_params const& _dp, std::function<void(int)> _callback)
{
	dialogbox::sThiz->setup(_dp, std::move(_callback));
}


void show(
	  std::string							_text
	, std::initializer_list<std::string>	_buttons
	, int									_back
	, std::function<void(int)> 				_callback
)
{
	dialog_params	dp;
	dp.text		= _text;
	dp.buttons	= _buttons;
	dp.back		= _back;
	dialogbox::sThiz->setup(dp, std::move(_callback));
}


ref<node> initialize()
{
	return	new dialogbox();
}



}

/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <timerapp-core/node.hpp>

#include <amtrs/node/node.hpp>
#include <amtrs/node/anchor_node.hpp>
#include <amtrs/node/fillwrap_node.hpp>
#include <amtrs/node/flexbox_node.hpp>
#include <amtrs/node/linerlayout_node.hpp>
#include <amtrs/node/scrollbox_node.hpp>
#include <amtrs/node/slider_bar_node.hpp>
#include <amtrs/node/string_node.hpp>
#include <gframe/@>

#include <timerapp-core/application.hpp>
#include <timerapp-core/contents.hpp>

using namespace ::amtrs;
using namespace ::gframe;



class	setting_transpalency_contents
		: public fillwrap_node<fill_node<contents::contents_base>>
{
public:
	using	flexbox		= flexbox_node<node>;
	using	super_type	= fillwrap_node<fill_node<contents::contents_base>>;


	std::thread	mInitThread;

	virtual std::string title() override
	{
		return	locale::gettext("settings.design.transpalency", "Transpalency");
	}

	~setting_transpalency_contents()
	{
		if (mInitThread.joinable())
		{
			mInitThread.join();
		}
	}

	setting_transpalency_contents(int _value, std::function<void(int _transpalency)> _callback)
		: mCallback(std::move(_callback))
	{
		this->set_color(schedule_contents::background_color((float)save::storage::current()->setting.looks.transpalency / 100.0f));

		mScheduleBox	= new scrollbox_node<flexbox>();
		mScheduleBox->set_justify(flexbox_node_base::justify_content_type::space_around);
		this->add_child(mScheduleBox);
		
		auto	f		= get_font();
		auto	fc		= f->color();

		auto	title	= mScheduleBox->add_item(new horizontalwrap_node<fill_node<node>>());
		title->set_height(80);
		title->set_color({1-((float)fc.r/255.0f),1-((float)fc.g/255.0f),1-((float)fc.b/255.0f),1});
		title->set_alpha((float)_value / 100.0f);

		auto	label	= new anchor_node<string_node<node>>(f);
		label->set_string(locale::gettext("settings.design.transpalency.preview", "Preview"));
		label->set_pivot({0.5f, 0.5f, 0});
		label->set_anchor({anchor_node_base::center, anchor_node_base::center, anchor_node_base::none});
		title->add_child(label);

		// プレビュー用を作成
		this->async_schedule("schedulelist", [this, _value]()
		{
			for (int i = 0; i < 5; ++i)
			{
				ref<save::schedule>	data	= new save::schedule();
				data->noedit			= false;
				data->basic.title		= u8"sample";
				data->time.flags		= save::schedule::timedata::use_begin;
				data->time.begintime	= save::timetype(save::timetype::now().get() + ((i * 2) * 20 * 60 * 60));
				data->time.alarmTime	= data->time.begintime;
				ref<node>	s				= schedule_contents::create(data);
				s->set_alpha((float)_value / 100.0f);
				this->schedule_once([this, s](auto)
				{
					mScheduleBox->add_item(s);
				});
			}
		});


		mFillbox				= new fill_node<node>();
		mFillbox->set_color({1,1,1,1});
		mFillbox->set_height(300);
		this->add_child(mFillbox);

		mSlider					= new slider_bar_node<float, node>((float)_value / 100.0f, 0.0f, 1.0f);
		mSlider->set_track_button(new sprite_node<node>(load_texture("ui/slider_pointer.png")))
		->set_pivot({0.5f, 0.5f, 0});
		mSlider->set_pivot({0.5f, 0.5f, 0});
		mSlider->set_value_change_listener([this](float _value) -> float
		{
			this->set_color(schedule_contents::background_color(_value));
			for (auto& i : mScheduleBox->citems())
			{
				i->set_alpha(_value);
			}
			mCallback((int)(100.0f * _value));
			return	_value;
		});
		mFillbox->add_child(mSlider);
	}

	virtual void on_size_change(typename super_type::size_type& _size) override
	{
		super_type::on_size_change(_size);
		mScheduleBox->set_position({0, 0, 0});
		mScheduleBox->set_size({_size.width, _size.height - mFillbox->size().height, 0});

		mFillbox->set_position({0, _size.height - mFillbox->size().height, 0});
		mFillbox->set_width(_size.width);

		mSlider->set_position({mFillbox->size().width/2, mFillbox->size().height/2, 0});
		mSlider->set_size({_size.width * 0.8f, 100, 0});
	}


	flexbox*								mScheduleBox	= nullptr;
	fill_node<node>*						mFillbox		= nullptr;
	slider_bar_node<float, node>*			mSlider			= nullptr;
	std::function<void(int _transpalency)>	mCallback;
};



amtrs::ref<contents::contents_base> contents::setting::create_transpalency(int _value, std::function<void(int _transpalency)> _callback)
{
	return	new setting_transpalency_contents(_value, std::move(_callback));
}


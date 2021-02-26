/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <iostream>
#include <string>
#include <timerapp-core/node.hpp>

#include <amtrs/chrono/time.hpp>
#include <amtrs/node/node.hpp>
#include <amtrs/node/anchor_node.hpp>
#include <amtrs/node/fill_node.hpp>
#include <amtrs/node/fillwrap_node.hpp>
#include <amtrs/node/flexbox_node.hpp>
#include <amtrs/node/linerlayout_node.hpp>
#include <amtrs/node/padding_node.hpp>
#include <amtrs/node/updatable_node.hpp>

#include <timerapp-core/application.hpp>
#include <timerapp-core/contents.hpp>
#include <timerapp-core/system.hpp>

#include <timerapp-core/manager/schedule.hpp>
#include <timerapp-core/manager/schedule_downloader.hpp>
#include <timerapp-core/manager/sound_manager.hpp>

using namespace ::amtrs;
using namespace ::gframe;
using namespace ::contents;



class	schedule_list_contents
		: public fillwrap_node<flexbox_node<fill_node<schedule_list>>>
{
public:
	static constexpr float	refresh_time	= 60 * 60.0f;

	using	super_type	= fillwrap_node<flexbox_node<fill_node<schedule_list>>>;

	schedule_list_contents(schedule_mode_type _mode, save::storage* _storage)
		: mMode(_mode)
		, mStorage(_storage)
	{
		this->set_color(schedule_contents::background_color((float)save::storage::current()->setting.looks.transpalency / 100.0f));
		this->set_justify(flexbox_node_base::justify_content_type::space_around);
		this->set_width(contents::contesnts_width);
		this->set_name("alarm");
		this->schedule_once([this]()
		{
			refresh();
		});
	}


	virtual std::string title() override
	{
		return	u8"予定";
	}

	bool use_alarm() const noexcept { return mMode == save::schedule_type::none || mMode == save::schedule_type::alarm; }
	bool use_schedule() const noexcept { return mMode == save::schedule_type::none || mMode == save::schedule_type::schedule; }


	virtual void set_schedule_select(bool _enable) override
	{
		mEnable	= _enable;
	}


	virtual void on_update(float _delta) override
	{
		super_type::on_update(_delta);
		mRefreshWait += _delta;
		if (mRefreshWait >= refresh_time)
		{
			mRefreshWait	= 0;
			refresh();
		}
	}

	void refresh() override
	{
		AMTRS_DEBUG_LOG("schedule list refresh");
		this->set_color(schedule_contents::background_color((float)save::storage::current()->setting.looks.transpalency / 100.0f));

		std::vector<save::schedule_ref>	items;
		schedule_manager::sync();


		for (auto& s : mStorage->schedules)
		{
#ifdef	AMTRS_DEBUG_LOG_USE
			std::string	title	= s->basic.title;
			title.erase(std::remove_if(title.begin(), title.end(), [](char c){ return c == '\r' ||  c == '\n'; }), title.end());
			AMTRS_DEBUG_LOG("schedule list check: %s:%s mode=%d, time.type=%d", s->basic.providor.c_str(), title.c_str(), mMode, s->time.type);
#endif
			if (mMode == save::schedule_type::none || s->time.type == mMode)
			{
				items.push_back(s);
			}
		}


		// 存在しないノードを削除
		for (auto it = mItems.begin(); it != mItems.end(); )
		{
			// スケジュールリストに載っているなら何もしない
			if (std::find_if(items.begin(), items.end(), [&](auto& s) { return (*it)->data()->basic.identify == s->basic.identify; }) != items.end())
			{
				++it;
				continue;
			}

			// スケジュールリストに存在しないので削除
			(*it)->remove_from_parent();
			it	= mItems.erase(it);
		}

		std::sort(items.begin(), items.end(), [](auto& a, auto& b)
		{
			return	a->time.alarmTime.get() > b->time.alarmTime.get();
		});

		this->async_schedule("schedulelist", [this, items]()
		{
			// 存在しないスケジュールならリストへ追加
			for (auto& s : items)
			{
				// すでに追加済みなら更新のみ
				auto	n	= std::find_if(mItems.begin(), mItems.end(), [&](auto n) { return n->data()->basic.identify == s->basic.identify; });
				if (n != mItems.end())
				{
					(*n)->update();
					continue;
				}

				// アイテムリストにないので新規追加
				save::schedule	sdef=*s;
				auto			i	= schedule_contents::create(s);
				i->set_alpha((float)save::storage::current()->setting.looks.transpalency / 100.0f);
				i->set_trigger_callback([=](auto pos)
				{
					if (!mEnable)
					{
						return;
					}

					sound_manager::play_sys(get_application_config()->sound(sound_types::editor_open));
					this->set_contents(contents::schedule_edit::create(schedule_edit::mode_types::modified, *s, [=](auto _msg, auto _alm)
					{
						auto	it	= std::find_if(mStorage->schedules.begin(), mStorage->schedules.end(), [&](auto& s)
						{
							return	s->basic.identify == _alm.basic.identify;
						});
						if (mStorage->schedules.end() == it)
						{
							return;
						}

						if (_msg == contents::schedule_edit::confirm)
						{
							if (auto itm = find_item(_alm.basic.identify); itm)
							{
								itm->update(_alm);
							}

							if (sdef.basic.providor != _alm.basic.providor)
							{
								schedule_manager::insert(&_alm);
								schedule_manager::erase(sdef.basic.providor, sdef.basic.identify);
							}
							else
							{
								schedule_manager::save(&_alm);
							}

							sound_manager::play_sys(get_application_config()->sound(sound_types::editor_complite));
							this->schedule_once([this](auto delta)
							{
								this->refresh();
							});
						}
						if (_msg == contents::schedule_edit::remove)
						{
							schedule_manager::erase(&_alm);
						//	sound_manager::play_sys(sound_edit_complite());
							this->schedule_once([this](auto delta)
							{
								this->refresh();
							});
						}
					}));
				});
				this->mItems.push_back(i);

				this->schedule_once([this, i](auto) { this->add_item(i); });
			}

			if (!mAddButton)
			{
				this->async_schedule("schedulelist", [this]()
				{
					auto	btn	= schedule_contents::create_new_schedule([this](save::schedule_type _type)
								{
									open_create_edit(_type);
								});
					if (btn)
					{
						btn->set_priority(100000);
						btn->set_alpha((float)save::storage::current()->setting.looks.transpalency / 100.0f);
						this->schedule_once([this, btn](auto) { this->add_item(btn); mAddButton = btn; });
					}
				});
			}

			this->schedule_once([this,items]()
			{
				int		p		= 0;
				float	alpha	= (float)save::storage::current()->setting.looks.transpalency / 100.0f;
				for (auto& i : items)
				{
					auto	it	= std::find_if(mItems.begin(), mItems.end(), [&](auto& item)
								{
									return	item->data()->basic.identify == i->basic.identify;
								});
					if (it != mItems.end())
					{
						(*it)->set_priority(p++);
						(*it)->set_alpha(alpha);
						(*it)->	refresh();
					}
				}
			});
		});
	}


	//! スクロールオーバー時のコールバックを設定します。
	virtual void set_overfllow_listener(std::function<void(vec2<float>&,bool)> _overfllow) override
	{
		mOverfllowListener	= std::move(_overfllow);
	}

	virtual void set_scroll_position(vec2<float> _pos) override
	{
		super_type::set_scroll(_pos);
	}

	
	virtual void create_editor(save::schedule_type _type) override
	{
		open_create_edit(_type);
	}

protected:
	virtual void on_overfllow(vec2<float>& _overfllow, bool _release) noexcept override
	{
		super_type::on_overfllow(_overfllow, _release);
		if (mOverfllowListener)
		{
			mOverfllowListener(_overfllow, _release);
		}
	}


	schedule_contents* find_item(std::string const& _identify)
	{
		auto	it	= std::find_if(mItems.begin(), mItems.end(), [&](auto& i) { return i->data()->basic.identify == _identify; });
		if (it == mItems.end())
		{
			return	nullptr;
		}
		return	*it;
	}

	void open_create_edit(save::schedule_type _type)
	{
		save::schedule	alm;
		alm.basic.providor	= schedule_manager::default_calendar_id();
		alm.basic.title		= u8"";
		alm.time.type		= _type == save::schedule_type::none ? mMode : _type;
		alm.time.flags		|= save::schedule::timedata::use_begin;
		alm.time.begintime	=  save::timetype::now();
		if (alm.time.type == save::schedule_type::alarm)
		{
			alm.time.flags		|= save::schedule::timedata::use_repeattime | save::schedule::timedata::use_weeks;
			alm.time.repeattime	=  alm.time.begintime;
		}
		sound_manager::play_sys(get_application_config()->sound(sound_types::editor_open));
		this->set_contents(contents::schedule_edit::create(schedule_edit::mode_types::created, alm, [this](auto _msg, auto _alm)
		{
			AMTRS_DEBUG_LOG("schedule edit callback");
			if (_msg == contents::schedule_edit::confirm)
			{
				ref<save::schedule>	s	= new save::schedule(_alm);
//				s->basic.identify	= mStorage->create_schedule_identify();
				schedule_manager::insert(s);
				sound_manager::play_sys(get_application_config()->sound(sound_types::editor_complite));
				this->schedule_once([this](auto delta)
				{
					this->refresh();
				});
			}
		}));
	}

	schedule_mode_type										mMode;
	ref<save::storage>										mStorage;
	std::list<schedule_contents*>							mItems;
	node*													mAddButton	= nullptr;
	float													mRefreshWait	= 0;
	bool													mEnable			= true;
	std::function<void(vec2<float>&,bool)>					mOverfllowListener;
};


amtrs::ref<contents::schedule_list> contents::schedule_list::create(schedule_mode_type _mode, save::storage* _storage)
{
	if (!_storage)
	{
		_storage	= save::storage::current();
	}
	return	new schedule_list_contents(_mode, _storage);
}

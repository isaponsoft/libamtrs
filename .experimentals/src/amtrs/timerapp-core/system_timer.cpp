/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/chrono/time.hpp>
#include <amtrs/node/anchor_node.hpp>
#include <amtrs/node/back_key_node.hpp>
#include <amtrs/node/input_untransparentable_node.hpp>
#include <amtrs/node/slider_bar_node.hpp>
#include <amtrs/node/updatable_node.hpp>
#include <amtrs/node/sliderlock_node.hpp>
#include <amtrs/node/string_node.hpp>

#include <timerapp-core/application.hpp>
#include <timerapp-core/contents.hpp>
#include <timerapp-core/system.hpp>
#include <timerapp-core/manager/sound_manager.hpp>

using namespace amtrs;
using namespace gframe;


class	alarm_view_node
		: public back_key_node<input_untransparentable_node<fillwrap_node<fill_node<updatable_node<amtrs::node>>>>>
{
public:
	static constexpr float	waittimming	= 3.0f;		// サウンドの繰り返し時間

	using	super_type	= back_key_node<input_untransparentable_node<fillwrap_node<fill_node<updatable_node<amtrs::node>>>>>;

	alarm_view_node(save::timetype _triggerTime, ref<save::schedule> _schedule)
		: mTriggerTime(_triggerTime)
		, mSchedule(_schedule)
	{
#ifdef  AMTRS_DEBUG_LOG_USE
		auto	l	= _triggerTime.localtime();
		AMTRS_DEBUG_LOG("Trigger Alarm %04d/%02d/%02d %02d:%02d:%02d %s", l.tm_year+1900, l.tm_mon+1, l.tm_mday, l.tm_hour, l.tm_min, l.tm_sec, mSchedule->basic.title.c_str());
#endif
        
		auto	c	= get_application_config()->settings.background_color;
		c.a	= 0.9f;
		this->set_color(c);

		mTitle		= new string_node<node>(get_font(font_type::ft_big));
		mTitle->set_string(mSchedule->basic.title);
		mTitle->set_pivot({0.5f, 1, 0});
		this->add_child(mTitle);

		auto	tm	= mTriggerTime.times();
		mDate		= new string_node<node>(get_font(font_type::ft_big));
		mDate->set_string(format<std::string>(u8"%02d:%02d", tm.hour, tm.min));
		mDate->set_pivot({0.5f, 0, 0});
		this->add_child(mDate);


		mLockBar	= new sliderlock_node<node>();
		auto	btn		= new sprite_node<node>(::gframe::load_texture("uiparts/key-fill.png"));
		btn->set_color({1-c.r,1-c.g,1-c.b,1});
		btn->set_pivot({0.5f, 0.5f, 1});
		mLockBar->set_size({500, 300, 0});
		mLockBar->set_track_button(btn);
		mLockBar->set_pivot({0.5f, 0, 1});
		this->add_child(mLockBar);

		mLock		= new anchor_node<sprite_node<node>>(::gframe::load_texture("uiparts/lock-line.png"));
		mLock->set_anchor({anchor_node_base::maximam, anchor_node_base::center, anchor_node_base::none});
		mLock->set_pivot({0.5f, 0.5f, 1});
		mLock->set_color({1-c.r,1-c.g,1-c.b,1});
		mLockBar->add_child(mLock);

		mPlayingQueue	= std::list<std::string>(mSchedule->sounds.begin(), mSchedule->sounds.end());
	}

	void set_unlock_listener(std::function<void()> _listener)
	{
		mLockBar->set_unlock_listener([this, _listener](auto status)
		{
			if (status == sliderlock_node_base::status_type::locking)
			{
				mLock->set_texture(::gframe::load_texture("uiparts/lock-line.png"));
				mLockBar->get_track_button()->set_visibility(true);
			}
			else
			{
				mLock->set_texture(::gframe::load_texture("uiparts/lock-unlock-line.png"));
				mLockBar->get_track_button()->set_visibility(false);
			}
			if (status == sliderlock_node_base::status_type::unlocked)
			{
				_listener();
			}
		});
	}

	


	void on_update(float _delta) override
	{
		super_type::on_update(_delta);

		// 再生中なら何もしない
		if (!mPlaying.empty() && !mPlaying->is_stopped())
		{
			return;
		}
		mPlaying.clear();

		// サウンドキューがないならサウンドキューを復元
		if (mPlayingQueue.empty())
		{
			if (mSchedule->status.loop_sound())
			{
				mPlayingQueue	= std::list<std::string>(mSchedule->sounds.begin(), mSchedule->sounds.end());
				mPlayingWait	= waittimming;	// 5分待つ
			}
			else
			{
				// ループ再生しないので一時的にスリープを許可する
				application::get_application()->set_sleep_enable(true);
			}
		}

		if (mPlayingQueue.empty())
		{
			return;
		}

		// 待ち時間があれば待ち時間の間は何もしない
		if (mPlayingWait > 0)
		{
			mPlayingWait -= _delta;
			if (mPlayingWait > 0)
			{
				return;
			}
			mPlayingWait	= 0;
		}

		mPlaying	= sound_manager::play_alarm(mPlayingQueue.front());
		mPlayingQueue.pop_front();
	}


	void on_size_change(typename super_type::size_type& _size) override
	{
		super_type::on_size_change(_size);

		auto	base	= vec3<float>(_size.width/2, _size.height/2, 0);

		mTitle	->set_position({base.x, base.y - 30, 0});
		mDate	->set_position({base.x, base.y + 30, 0});
		mLockBar->set_position({base.x, _size.height * 0.73f, 0});
	}

private:
	string_node<node>*					mTitle			= nullptr;
	string_node<node>*					mDate			= nullptr;
	sliderlock_node<node>*				mLockBar		= nullptr;
	anchor_node<sprite_node<node>>*		mLock			= nullptr;

	float								mPlayingWait	= 0;
	ref<sound_manager::playing_status>	mPlaying;
	std::list<std::string>				mPlayingQueue;

	save::timetype						mTriggerTime;
	ref<save::schedule>					mSchedule;
};



class	timer_node
		: public updatable_node<fillwrap_node<amtrs::node>>
{
public:
		// 毎フレーム処理をしても電力がもったいないので３分の１秒単位で処理する
//	static constexpr float	timming		= 4.3f;
	static constexpr float	timming		= 0.3f;


	using	super_type	= updatable_node<fillwrap_node<amtrs::node>>;

	//! 1秒ごとにアラームの更新処理を行う
	void on_update(float _delta) override
	{
		super_type::on_update(_delta);


		mTimespan += _delta;
		if (mTimespan < timming)
		{
			return;
		}
		mTimespan	-= timming;

		// キューが無いかアラーム表示中なら何もしない
		if (mAlarmView)
		{
			return;
		}

		auto	now		= save::timetype::now();
		for (auto& s : save::storage::current()->schedules)
		{
			if ((now < s->time.alarmTime) || (s->status.finish()))
			{
				continue;
			}

			AMTRS_DEBUG_LOG("Wakeup alarm %s", s->basic.title.c_str());

			// アラームの時間を経過した。
			if (((now.get() - s->time.alarmTime.get()) < (5*60)))
			{
				if (!save::storage::current()->setting.alarm_disable())
				{
					// アラームを表示
					mAlarmView	= new alarm_view_node(s->time.alarmTime, s);
					this->add_child(mAlarmView);
					mAlarmView->set_position({0, this->size().height, 0});
					mAlarmView->run_action(new move_by_action<>({0,0,0}, 0.4f));
					mAlarmView->set_unlock_listener([this]
					{
						mAlarmView->run_action(new move_by_action<>({0, this->size().height, 0}, 0.2f))
						->set_finish_callback([this]()
						{
							this->schedule_once([this]()
							{
								sound_manager::stop_all();
								mAlarmView->remove_from_parent();
								contents::palen_visible(true);
								application::get_application()->set_sleep_enable(!save::storage::current()->setting.energy.none_sleep());
								mAlarmView	= nullptr;
							});
						});
					});

					contents::palen_visible(false);
				}
			}


			bool	saving	= false;
			if (s->time.type == save::schedule_type::schedule)
			{
				// 終了する
				saving				= true;
				s->status.finish(true);
			}
			if (s->time.type == save::schedule_type::alarm)
			{
				AMTRS_DEBUG_LOG("alarm next time");

				// 次の時間をセットする(リピートタイプ)
				auto	next	= s->time.compute_next_alarmtime(now);
				if (s->time.alarmTime != next)
				{
					saving	= true;
					s->time.alarmTime	= next;
				}
			}
			if (saving)
			{
				save::storage::current()->save();
			}

			break;
		}
	}


	float					mTimespan	= 0;
	alarm_view_node*		mAlarmView	= nullptr;
};


amtrs::ref<amtrs::node> timer::initialize()
{
	return	new timer_node();
}

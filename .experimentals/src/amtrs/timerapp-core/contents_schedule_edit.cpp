/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <timerapp-core/node.hpp>

#include <amtrs/chrono/time.hpp>
#include <amtrs/node/anchor_node.hpp>
#include <amtrs/node/clipping_node.hpp>
#include <amtrs/node/fill_node.hpp>
#include <amtrs/node/fillwrap_node.hpp>
#include <amtrs/node/flexbox_node.hpp>
#include <amtrs/node/linerlayout_node.hpp>
#include <amtrs/node/moveslideshow_node.hpp>
#include <amtrs/node/scrollbox_node.hpp>
#include <amtrs/node/string_node.hpp>
#include <amtrs/node/text_node.hpp>
#include <amtrs/media/audio.hpp>

#include <timerapp-core/application.hpp>
#include <timerapp-core/contents.hpp>
#include <timerapp-core/system.hpp>
#include <timerapp-core/manager/schedule.hpp>

using namespace ::amtrs;
using namespace ::gframe;


rgba<float> get_text_color(bool _active)
{
	return	_active
			? rgba<float>{1,1,1,1}
			: rgba<float>{1,1,1,0.5f};
}

class	stringedit_node
		: public horizontalwrap_node<children_verticalwrap_node<horizonatlmoveslideshow_node<clipping_node<node>>>>
{
public:
	using	super_type	= horizontalwrap_node<children_verticalwrap_node<horizonatlmoveslideshow_node<clipping_node<node>>>>;

	stringedit_node()
	{
		this->set_color({1,1,1,1});
		auto	s	= new string_node<node>(::get_font());
		mString	= add_child(s);
	}

	void set_string(std::string _str)
	{
		mString->set_string(std::move(_str));
		this->reset_moveslideshow_scroll();
	}

	void set_date(save::timetype _time)
	{
		auto	lt	= _time.localtime();
		auto	str	= format<std::string>("%04d/%02d/%02d", lt.tm_year + 1900, lt.tm_mon + 1, lt.tm_mday);
		mString->set_string(str);
		this->reset_moveslideshow_scroll();
	}

	void set_time(save::timetype _time)
	{
		auto	lt	= _time.localtime();
		auto	str	= format<std::string>("%02d:%02d", lt.tm_hour, lt.tm_min);
		mString->set_string(str);
		this->reset_moveslideshow_scroll();
	}

	void set_active_color(bool _active)
	{
		mString->set_color(get_text_color(_active));
	}

private:
	string_node<node>*	mString	= nullptr;
};

class	stringlabel_node
		: public horizontalwrap_node<children_verticalwrap_node<horizonatlmoveslideshow_node<clipping_node<node>>>>
{
public:
	using	super_type	= horizontalwrap_node<children_verticalwrap_node<horizonatlmoveslideshow_node<clipping_node<node>>>>;

	stringlabel_node()
	{
		auto	s	= new string_node<node>(::get_font());
		mString	= add_child(s);
		this->set_color({1,1,1,1});
	}

	void set_string(std::string _str)
	{
		mString->set_string(std::move(_str));
		this->reset_moveslideshow_scroll();
	}

	void set_active_color(bool _active)
	{
		mString->set_color(get_text_color(_active));
	}

private:
	string_node<node>*	mString	= nullptr;
};

class	textedit_node
		: public horizontalwrap_node<text_node<node>>
{
public:
	using	super_type	= horizontalwrap_node<text_node<node>>;

	textedit_node()
		: super_type(::get_font())
	{}
};


class	weekedit_node
		: public horizontalwrap_node<children_verticalwrap_node<flexbox_node<node>>>
{
public:
	using	super_type	= horizontalwrap_node<children_verticalwrap_node<flexbox_node<node>>>;

	weekedit_node()
	{
		set_justify(flexbox_node_base::justify_content_type::space_around);
		static const char*	weeks[7] = { "weeks.short.sun", "weeks.short.mon", "weeks.short.tue", "weeks.short.wed", "weeks.short.thu", "weeks.short.fri", "weeks.short.sat" };
		auto				f	= get_font();
		for (int windex = 0; windex < 7; ++windex)
		{
			auto	w	= new button_node<string_node<node>>(f);
			w->set_trigger_size_expand({(float)f->size(),(float)f->size()*2});
			w->set_string(locale::gettext(weeks[windex]));
			w->set_trigger_callback([this, windex](auto)
			{
				mCallback(windex);
			});
			mWeeks[windex]	= this->add_item(w);
		}
	}

	void set_select(std::function<void(int)> _callback)
	{
		mCallback	= std::move(_callback);
	}

	void set_weeks(unsigned int _weeks)
	{
		for (int windex = 0, wmask = 1; windex < 7; ++windex, wmask <<= 1)
		{
			mWeeks[windex]->set_color(
				_weeks & wmask
				? rgba<float>(1,1,1,1)
				: rgba<float>(1,1,1,0.5f)
			);
		}
	}
	string_node<node>*			mWeeks[7]	= {nullptr};
	std::function<void(int)>	mCallback;
};


template<class T, class Setter, class Edit>
amtrs::ref<T> create_editor(flexbox_node<node>* _containor, editor_info::types _type, const char* _tag, bool _noedit, Setter&& _setter, Edit&& _edit)
{
	editor_info::editor	e;
	auto	node	= new T();
	_setter(node);
	e.type		= _type;
	e.label		= locale::gettext(_tag);
	e.node		= node;
	if (_noedit)
	{
		e.callback	= [](auto listener){};
	}
	else
	{
		e.callback	= [node, setter = std::move(_setter), callback = std::move(_edit)](auto listener)
		{
			callback([=]()
			{
				setter(node);
				listener();
			});
		};
	}

	_containor->add_item(create_editor(std::move(e)), node::use_scheduler{});
	return	node;
}




class	schedule_edit_contents
		: public fillwrap_node<fill_node<contents::schedule_edit>>
{
public:
	using	super_type	= fillwrap_node<fill_node<contents::schedule_edit>>;

	virtual std::string title() override
	{
		return	locale::gettext("editor.title", "EDIT");
	}

	schedule_edit_contents(mode_types _mode, value_type const& _alarm, std::function<callback_type> _callback)
		: mMode(_mode)
		, mAlarm(_alarm)
		, mCallback(std::move(_callback))
	{
		this->set_name("alarm");
		this->set_color(get_application_config()->settings.background_color);

		auto	containor	= new fillwrap_node<scrollbox_node<flexbox_node<node>>>();


		containor->add_item(create_setting_group(locale::gettext("editor.group.title")), use_scheduler{});

		// ==========================================================
		// タイトルの編集
		// ----------------------------------------------------------
		create_editor<stringedit_node>(containor, editor_info::title_edit, "editor.attr.title", mAlarm.noedit,
			// set
			[this](auto node)
			{
				if (mAlarm.basic.title.empty())
				{
					node->set_string(locale::gettext("editor.warning.require-title"));
					node->set_active_color(false);
				}
				else
				{
					node->set_string(mAlarm.basic.title);
					node->set_active_color(true);
				}
			},
			// edit
			[this](auto update)
			{
				textinput(mAlarm.basic.title, [=](auto txt)
				{
					mAlarm.basic.title = txt;
					update();
				});
			}
		);


		containor->add_item(create_setting_splitbar(), use_scheduler{});
		containor->add_item(create_setting_group(locale::gettext("editor.group.datetime")), use_scheduler{});

		// ==========================================================
		// 日付の編集
		// ----------------------------------------------------------
		if (mAlarm.time.type == contents::schedule_mode_type::schedule)
		{
			create_editor<stringedit_node>(containor, editor_info::date_edit, "editor.attr.date", mAlarm.noedit,
				// set
				[this](auto node)
				{
					node->set_date(mAlarm.time.begintime);
					node->set_active_color(true);
				},
				// edit
				[this](auto update)
				{
					mDateInput	= dateinput_dialog::create();
					mDateInput->show();
					mDateInput->set_date(mAlarm.time.begintime);
					mDateInput->set_event_callback([=](auto _event, auto* _sender) -> bool
					{
						auto	range	= mAlarm.time.endtime.get() - mAlarm.time.begintime.get();
						if (_event == dateinput_dialog::event_type::ok)
						{
							mAlarm.time.flags	|= save::schedule::timedata::use_begin;
							mAlarm.time.begintime.set(mDateInput->days());
							mAlarm.time.endtime		= chrono::timev(mAlarm.time.begintime.get() + range);
							update();
						}
						this->schedule_once([this]() { mDateInput->close(); mDateInput.clear(); rerender_request(); });
						return	true;
					});
				}
			);
		}


		// ==========================================================
		// 時間の編集
		// ----------------------------------------------------------
		create_editor<stringedit_node>(containor, editor_info::time_edit, "editor.attr.time", mAlarm.noedit,
			// set
			[this](auto node)
			{
				node->set_time(mAlarm.time.begintime);
				node->set_active_color(true);
			},
			// edit
			[this](auto update)
			{
				mTimeInput	= timeinput_dialog::create();
				mTimeInput->show();
				mTimeInput->set_time(mAlarm.time.begintime);
				mTimeInput->set_event_callback([this, update](auto _event, auto* _sender) -> bool
				{
					auto	range	= mAlarm.time.endtime.get() - mAlarm.time.begintime.get();
					if (mAlarm.time.type == contents::schedule_mode_type::alarm)
					{
						mAlarm.time.flags	|= save::schedule::timedata::use_repeattime;
						mAlarm.time.repeattime.set(mTimeInput->times());
						update();
					}
					else
					{
						mAlarm.time.begintime.set(mTimeInput->times());
						mAlarm.time.endtime		= chrono::timev(mAlarm.time.begintime.get() + range);
						update();
					}
					this->schedule_once([this]() { mTimeInput->close(); mTimeInput.clear(); rerender_request(); });
					return	true;
				});
			}
		);



		// ==========================================================
		// 曜日の編集
		// ----------------------------------------------------------
		if (mAlarm.time.type == contents::schedule_mode_type::alarm)
		{
			auto	weekn	= create_editor<weekedit_node>(containor, editor_info::weeks_edit, "editor.attr.weeks", mAlarm.noedit,
				[](auto node){},
				[](auto update){}
			);
			weekn->set_weeks(mAlarm.time.week);
			weekn->set_select([this, weekn](int w)
			{
				mAlarm.time.flags	|= save::schedule::timedata::use_weeks;
				mAlarm.time.week	^= 1<<w;
				weekn->set_weeks(mAlarm.time.week);
			});
		}

		containor->add_item(create_setting_splitbar(), use_scheduler{});
		containor->add_item(create_setting_group(locale::gettext("editor.group.sound")), use_scheduler{});

		// ==========================================================
		// サウンドの編集
		// ----------------------------------------------------------
		create_editor<stringlabel_node>(containor, editor_info::sound_edit, "editor.attr.sound", false,
			// set
			[this](auto node)
			{
				if (mAlarm.sounds.empty())
				{
					node->set_string(locale::gettext("editor.warning.caninput-sound"));
					node->set_active_color(false);
				}
				else
				{
					auto*	itm	= sound::find_item(mAlarm.sounds.front());
					if (itm)
					{
						node->set_string(itm->title);
					}
					else
					{
						node->set_string((std::string)filesystem::filename(mAlarm.sounds.front()));
					}
					node->set_active_color(true);
				}
			},
			// edit
			[this](auto update)
			{
				this->set_contents(contents::voice_list::create(
					[=](sound::group const& _group, sound::item const& _item) -> unsigned int
					{
						unsigned int	flg	= 0;
						if ((!mAlarm.sounds.empty()) && mAlarm.sounds.front() == _item.filename)
						{
							flg |= sounditem_mode_selected;
						}
						if (!_group.product.empty() && !save::storage::current()->purchase.is_purchased(_group.product))
						{
							flg |= sounditem_mode_locked;
						}
						return	flg;
					},
					[=](std::string const& _filename, unsigned int _flg)
					{
						if (!(_flg & sounditem_mode_selected) && !(_flg & sounditem_mode_locked))
						{
							mAlarm.sounds	= {};
							if (!_filename.empty())
							{
								AMTRS_DEBUG_LOG("Alarm sound %s", _filename.c_str());
								mAlarm.sounds.push_back(_filename);
							}
							update();
							return	true;
						}
						return	(_flg & sounditem_mode_locked) ? false : true;
					})
				);
			}
		);


		// ==========================================================
		// サウンドループの編集
		// ----------------------------------------------------------
		create_editor<stringedit_node>(containor, editor_info::notify_edit, "editor.attr.loop_sound", false,
			// set
			[this](auto node)
			{
				node->set_string(mAlarm.status.loop_sound()
					? locale::gettext("editor.attr.loop_sound.on")
					: locale::gettext("editor.attr.loop_sound.off")
				);
			},
			// edit
			[this](auto update)
			{
				mAlarm.status.loop_sound(!mAlarm.status.loop_sound());
				update();
			}
		);


		containor->add_item(create_setting_splitbar(), use_scheduler{});
		containor->add_item(create_setting_group(locale::gettext("editor.group.note")), use_scheduler{});

		// ==========================================================
		// メモの編集
		// ----------------------------------------------------------
		if (!mAlarm.noedit || !mAlarm.extra.note.empty())
		{
			create_editor<textedit_node>(containor, editor_info::note_edit, "editor.attr.note", mAlarm.noedit,
				// set
				[this](auto node)
				{
					if (mAlarm.extra.note.empty())
					{
						node->set_string(locale::gettext("editor.warning.caninput-note"));
						node->set_color(get_text_color(false));
					}
					else
					{
						node->set_string(mAlarm.extra.note);
						node->set_color(get_text_color(true));
					}
				},
				// edit
				[this](auto update)
				{
					textinput(mAlarm.extra.note, [=](auto txt)
					{
						mAlarm.extra.note = txt;
						update();
					});
				}
			);
		}

		containor->add_item(create_setting_splitbar(), use_scheduler{});
		containor->add_item(create_setting_group(locale::gettext("editor.group.saveto")), use_scheduler{});

		// ==========================================================
		// 保存先
		// ----------------------------------------------------------
		create_editor<stringedit_node>(containor, editor_info::saveto_edit, "editor.attr.saveto", false,
			// set
			[this](auto node)
			{
				if (mAlarm.time.type == save::schedule_type::alarm)
				{
					node->set_string(locale::gettext("editor.warning.alarm-is-localonly"));
					node->set_active_color(false);
				}
				else
				{
					node->set_string(schedule_manager::calendar_display_name(mAlarm.basic.providor));
					node->set_active_color(true);
				}
			},
			// edit
			[this](auto update)
			{
				if (mAlarm.time.type != save::schedule_type::alarm)
				{
					this->set_contents(contents::create_calendar_selector(mAlarm.basic.providor,
						[=](std::string const& _calendarId)
						{
							mAlarm.basic.providor	= _calendarId;
							update();
						})
					);
				}
			}
		);

		containor->add_item(create_setting_splitbar(), use_scheduler{});
		containor->add_item(create_setting_group(locale::gettext("editor.group.notification")), use_scheduler{});

		// ==========================================================
		// 通知の編集
		// ----------------------------------------------------------
		create_editor<stringedit_node>(containor, editor_info::notify_edit, "editor.attr.notify", false,
			// set
			[this](auto node)
			{
				node->set_string(mAlarm.status.notify()
					? locale::gettext("editor.attr.notify.on")
					: locale::gettext("editor.attr.notify.off")
				);
			},
			// edit
			[this](auto update)
			{
				mAlarm.status.notify(!mAlarm.status.notify());
				update();
			}
		);


		// ==========================================================
		// 自動起動の編集(Androidのみ)
		// ----------------------------------------------------------
#if	AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_ANDROID
		create_editor<stringedit_node>(containor, editor_info::wakeup_edit, "editor.attr.wakeup", false,
			// set
			[this](auto node)
			{
				node->set_string(mAlarm.status.wakeup()
					? locale::gettext("editor.attr.wakeup.on")
					: locale::gettext("editor.attr.wakeup.off")
				);
			},
			// edit
			[this](auto update)
			{
				mAlarm.status.wakeup(!mAlarm.status.wakeup());
				update();
			}
		);
#endif


		mItem	= this->add_child(new horizontalwrap_node<node>());
//		mItem->add_child(create_editor(e));
		mItem->add_child(containor);


		std::vector<std::string>	menus{locale::gettext("editor.confirms.commit"), locale::gettext("editor.confirms.cancel")};
		if (mMode == mode_types::modified && !mAlarm.noedit)
		{
			menus.push_back(locale::gettext("editor.confirms.delete"));
		}
		mMenu	= this->add_child(create_bottom_menu(menus, -1, [this](auto sender, int id)
		{
			if (id == 0)
			{
				if (mAlarm.basic.title.empty())
				{
					dialog::show(
						format<std::string>(locale::gettext("editor.warning.require-title"), mAlarm.basic.title.c_str()),
						{ locale::gettext("editor.warning.require.back") } , 1,
						[](int _id)
						{}
					);
					return;
				}

				if (mAlarm.time.week == 0 && mAlarm.time.type == save::schedule_type::alarm)
				{
					dialog::show(
						format<std::string>(locale::gettext("editor.warning.require-weeks"), mAlarm.basic.title.c_str()),
						{ locale::gettext("editor.warning.require.back") } , 1,
						[](int _id)
						{}
					);
					return;
				}

				mAlarm.time.alarmTime	= mAlarm.time.compute_next_alarmtime(save::timetype::now());
				mAlarm.status.finish(	mAlarm.time.alarmTime <= save::timetype::now()
										? true			// 過去の時間をさしているので終了済み扱い
										: false);		// 未来の時間なのでまだ終了していない
				mCallback(message_id::confirm, mAlarm);
				this->pop();
			}
			if (id == 1)
			{
				this->pop();
			}
			if (id == 2)
			{
				dialog::show(
					format<std::string>(locale::gettext("editor.warning.delete"), mAlarm.basic.title.c_str()),
					{ locale::gettext("editor.warning.delete.commit"), locale::gettext("editor.warning.delete.cancel") } , 1,
					[this](int _id)
					{
						if (_id == 0)
						{
							mCallback(message_id::remove, mAlarm);
							this->pop();
						}
					}
				);
			}
		}));

		this->schedule_once([this]()
		{
			update_position();
		});
	}


	void on_size_change(typename super_type::size_type& _size) override
	{
		super_type::on_size_change(_size);
		update_position();
	}


	void on_children_transform_change(typename super_type::transform_component* _child) override
	{
		super_type::on_children_transform_change(_child);
		update_position();
	}

	void on_children_size_change(typename super_type::transform_component* _child) override
	{
		super_type::on_children_size_change(_child);
		update_position();
	}


	void textinput(std::string _text, std::function<void(std::string)> _listener)
	{
		mTitleInput	= textinput_dialog::create();
		mTitleInput->show();
		mTitleInput->set_string(std::move(_text));
		mTitleInput->set_return_callback([this, _listener]()
		{
			std::string	t	= mTitleInput->text();
			this->schedule_once([=]()
			{
				_listener(t);
				mTitleInput->close();
				mTitleInput	= nullptr;
				rerender_request();
			});
		});
	}

	void update_position()
	{
		if (mItem && !mUpdateDirty)
		{
			mUpdateDirty	= true;
			this->schedule_once([this]()
			{
				auto	cs	= this->children_area();
				mItem->set_height(cs.height - mMenu->size().height);
				mUpdateDirty	= false;
			});
		}
	}


	mode_types						mMode;
	node*							mItem		= nullptr;
	node*							mMenu		= nullptr;

	bool							mUpdateDirty	= false;

	value_type						mAlarm;
	std::function<callback_type>	mCallback;

	ref<textinput_dialog>			mTitleInput;
	ref<dateinput_dialog>			mDateInput;
	ref<timeinput_dialog>			mTimeInput;
};




amtrs::ref<contents::schedule_edit> contents::schedule_edit::create(mode_types _mode, value_type const& _alarm, std::function<callback_type> _callback)
{
	return	new schedule_edit_contents(_mode, _alarm, _callback);
}

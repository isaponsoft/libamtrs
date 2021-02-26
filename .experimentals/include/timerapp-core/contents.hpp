/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libscheduler__contents__hpp
#define	__libscheduler__contents__hpp
#include <amtrs/node/fillwrap_node.hpp>
#include <amtrs/node/intersects_notification_node.hpp>
#include <amtrs/node/scrollbox_node.hpp>
#include <gframe/@>
#include "save/storage.hpp"
#include "dataset/bg.hpp"
#include "dataset/sound.hpp"
#include "manager/purchase.hpp"


namespace contents {

using namespace amtrs;

class	contents_base;
class	contents_containor;

static constexpr float	contesnts_width	= 986.0f;


void palen_visible(bool _vis);
void set_main_contents(amtrs::ref<contents_base> _node);
void loading(bool _enable);




ref<node> create_home();
ref<node> create_loading();
ref<contents_base> create_calendar_selector(std::string const& _current, std::function<void(std::string const&)> _callback);

class	contents_containor
		: public amtrs::intersects_notification_node<amtrs::node>
{
public:
	enum class	message_id
	{
		setup			= 0,		//!< コンテンツをセットアップした
		trigger_icon	= 1,		//!< アイコンを押した
		removing		= 2,		//!< コンテンツの削除を開始した
	};
	static constexpr message_id		setup			= message_id::setup;
	static constexpr message_id		trigger_icon	= message_id::trigger_icon;
	static constexpr message_id		removing		= message_id::removing;

	using	super_type	= amtrs::intersects_notification_node<amtrs::node>;

	virtual ~contents_containor() {}

	static amtrs::ref<contents_containor> create();

	virtual void set_contents(amtrs::ref<contents_base> _contents) = 0;

	virtual void pop(size_t _poppage = 1) = 0;

	virtual void update_title(std::string _titlte) {}
};



class	contents_base
		: public amtrs::node
{
public:
	virtual std::string title() = 0;

	void update_title(std::string _titlte)
	{
		mContainor->update_title(_titlte);
	}

	void pop(size_t _poppage = 1)
	{
		mContainor->pop(_poppage);
	}

	virtual int on_containor(contents::contents_containor* _containor, contents::contents_containor::message_id _msg)
	{
		mContainor	= _containor;
		return	0;
	}

	void set_contents(amtrs::ref<contents_base> _contents)
	{
		mContainor->set_contents(_contents);
	}

private:
	contents_containor*		mContainor	= nullptr;
};




using	schedule_mode_type	= save::schedule_type;



// アバウト/著作情報など
class	about
		: public contents::contents_base
{
public:
	static amtrs::ref<contents_base> create();
};


// 背景選択
class	bg_selector_list
		: public contents_base
{
public:
	static amtrs::ref<contents_base> create(std::function<unsigned int(bg::item const&)> _itemstatus, std::function<bool(bg::item const&, unsigned int)> _confirmcallback);
};


// 背景選択
class	bg_file_selector
		: public contents_base
{
public:
	static amtrs::ref<contents_base> create(std::initializer_list<std::string> _extras, std::function<bool(std::string const&)> _select);
};
// イベントソース選択
class	event_sources_selector
		: public contents_base
{
public:
	static amtrs::ref<contents_base> create();
};




// ボイス選択（アラーム音声）
class	voice_list
		: public contents_base
{
public:
	static amtrs::ref<contents_base> create(std::function<unsigned int(sound::group const&, sound::item const&)> _itemstatus, std::function<bool(std::string const&, unsigned int)> _confirmcallback);
};




// スケジュール一覧
class	schedule_list
		: public scrollbox_node<contents::contents_base>
{
public:
	//! リストをリフレッシュします。
	virtual void refresh() = 0;

	//! スクロールオーバー時のコールバックを設定します。
	virtual void set_overfllow_listener(std::function<void(vec2<float>&,bool)> _overfllow) = 0;

	virtual void set_scroll_position(vec2<float> _pos) = 0;

	//! 新規作成画面を開く
	virtual void create_editor(save::schedule_type _type) = 0;

	//! スケジュール選択のON/OFFを切り替えます。
	virtual void set_schedule_select(bool _enable) = 0;

	static amtrs::ref<schedule_list> create(schedule_mode_type _mode, save::storage* _storage = nullptr);
};




// 設定
class	setting
{
public:
	static amtrs::ref<contents::contents_base> create();
	static amtrs::ref<contents::contents_base> create_transpalency(int _value, std::function<void(int _transpalency)> _callback);
};


// ショップ
class	shop_list
		: public contents::contents_base
{
public:
	static amtrs::ref<contents_base> create(save::storage* _storage = nullptr);
};






class	schedule_edit
		: public contents::contents_base
{
public:
	enum class message_id
	{
		confirm	= 1,
		cancel	= 2,
		remove	= 3,
	};
	static constexpr message_id	confirm	= message_id::confirm;
	static constexpr message_id	cancel	= message_id::cancel;
	static constexpr message_id	remove	= message_id::remove;

	enum class mode_types
	{
		created		= 0,
		modified	= 1,
	};


	using	value_type		= save::schedule;
	using	callback_type	= void(message_id _msgid, value_type const& _alarm);

	static amtrs::ref<schedule_edit> create(mode_types _mode, value_type const& _alarm, std::function<callback_type> _callback);
};

class	shop_detail
		: public contents::contents_base
{
public:
	static amtrs::ref<contents_base> create(purchase::purchase_product const& _product, save::storage* _storage = nullptr);
};


}
#endif

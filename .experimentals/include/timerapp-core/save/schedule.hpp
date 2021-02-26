/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libschedule__save__schedule__hpp
#define	__libschedule__save__schedule__hpp
#include <ctime>
#include <vector>
#include <amtrs/amtrs.hpp>
#include <amtrs/string/@>
#include "time.hpp"
#include "setting_looks.hpp"
#include "setting_sound.hpp"
namespace save {

// ******************************************************************
//! スケジュールタイプ
// ------------------------------------------------------------------
enum class	schedule_type : uint16_t
{
	none			= 0,		//!< 複合モード/未指定
	schedule		= 1,		//!< スケジュール：　日時指定
	alarm			= 2,		//!< アラーム：　　　曜日と時間指定
};


// ******************************************************************
//! スケジュールデータ
// ------------------------------------------------------------------
//!
// ==================================================================
struct	schedule : amtrs::ref_object
{
	static constexpr uint32_t		finished_bits			= 1<<0;
	static constexpr uint32_t		notify_enable_bits		= 1<<1;
	static constexpr uint32_t		app_wakeup_bits			= 1<<2;
	static constexpr uint32_t		loop_sound_bits			= 1<<3;
	static constexpr uint32_t		deleted_bits			= 1<<4;


	// 基本情報
	struct	basicdata
	{
		std::string		providor;				//!< 配信元のURL
		std::string		identify;				//!< 識別子
		std::string		title;					//!< 表題
	};

	// 時間情報
	struct	timedata
	{
		static constexpr uint16_t	use_begin		= 1<<0;
		static constexpr uint16_t	use_end			= 1<<1;
		static constexpr uint16_t	use_repeattime	= 1<<2;
		static constexpr uint16_t	use_weeks		= 1<<3;

		schedule_type	type		= schedule_type::none;
		uint16_t		flags		= 0;				//!< 設定時刻のフラグ use_begin は必須

		// ユーザーが設定したデータ
		timetype		begintime	= timetype(0);		//!< 設定日時(必須)
		timetype		endtime		= timetype(0);		//!< 終了日時
		timetype		repeattime	= timetype(0);		//!< アラームにおけるリピート時間
		uint16_t		week		= 0;				//!< 曜日指定(０以外の場合は指定曜日で自動的に次のアラームを設定する)

		// 次回、アラームを実行する時間
		// type == schedule_type::schedule の場合は begintime と等しくなる
		timetype		alarmTime	= timetype(0);		//!< 次にアラームを鳴らすタイミング

		//! _baseから計算して次のアラーム時間を計算します。
		timetype compute_next_alarmtime(timetype _base) const noexcept;

		void refresh_alart(timetype _base) noexcept { alarmTime = compute_next_alarmtime(_base); }
	};

	struct	extradata
	{
		std::string		url;					//!< URL
		std::string		note;					//!< 注釈・メモ
	};

	struct	statusdata
	{
		uint32_t	flags	= notify_enable_bits
							| app_wakeup_bits
							| loop_sound_bits
					;

		bool finish() const noexcept		{ return flags & finished_bits; }
		void finish(bool _on) noexcept		{ if (_on) { flags |= finished_bits; } else { flags &= ~finished_bits; } }
		bool notify() const noexcept		{ return flags & notify_enable_bits; }
		void notify(bool _on) noexcept		{ if (_on) { flags |= notify_enable_bits; } else { flags &= ~notify_enable_bits; } }
		bool wakeup() const noexcept		{ return flags & app_wakeup_bits; }
		void wakeup(bool _on) noexcept		{ if (_on) { flags |= app_wakeup_bits; } else { flags &= ~app_wakeup_bits; } }
		bool loop_sound() const noexcept	{ return flags & loop_sound_bits; }
		void loop_sound(bool _on) noexcept	{ if (_on) { flags |= loop_sound_bits; } else { flags &= ~loop_sound_bits; } }
		bool deleted() const noexcept		{ return flags & deleted_bits; }
		void deleted(bool _on) noexcept		{ if (_on) { flags |= deleted_bits; } else { flags &= ~deleted_bits; } }
	};

	struct	soundlist
			: std::vector<std::string>
	{
		using	super_type	= std::vector<std::string>;
		using	super_type::super_type;
		using	super_type::operator =;
	};


	basicdata		basic;
	timedata		time;
	extradata		extra;

	// ユーザー設定領域
	soundlist		sounds;					//!< 設定するサウンド
	statusdata		status;

	bool			noedit	= false;		//!< ダウンロードしたデータなどの編集不可能データの時は true

	virtual ~schedule();
};




struct	schedulelist
		: std::vector<amtrs::ref<schedule>>
{
	using	super_type	= std::vector<amtrs::ref<schedule>>;
	using	super_type::super_type;
	using	super_type::operator =;
	using	super_type::operator [];
};


using	schedule_ref	= amtrs::ref<schedule>;

bool deserialize(schedule::basicdata& _data, std::istream& _in);
bool deserialize(schedule::timedata& _data, std::istream& _in);
bool deserialize(schedule::extradata& _data, std::istream& _in);
bool deserialize(schedule::soundlist& _data, std::istream& _in);
bool deserialize(schedule::statusdata& _data, std::istream& _in);
bool deserialize(schedule& _source, std::istream& _in);
bool deserialize(schedulelist& _source, std::istream& _in);


bool serialize(std::ostream& _out, schedule::basicdata const& _data);
bool serialize(std::ostream& _out, schedule::timedata const& _data);
bool serialize(std::ostream& _out, schedule::extradata const& _data);
bool serialize(std::ostream& _out, schedule::soundlist const& _data);
bool serialize(std::ostream& _out, schedule::statusdata const& _data);
bool serialize(std::ostream& _out, schedule const& _source);
bool serialize(std::ostream& _out, schedulelist const&);


}
#endif

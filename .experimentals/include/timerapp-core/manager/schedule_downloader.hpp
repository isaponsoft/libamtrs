/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__module__schedule_downloader__hpp
#define	__module__schedule_downloader__hpp
#include <string>
#include <vector>
#include <functional>
#include <amtrs/chrono/calendar.hpp>
#include "../save/schedule.hpp"


// ==================================================================
//! イベントのソース
// ------------------------------------------------------------------
struct	schedule_source
{
	static constexpr uint32_t	enable_bits			= 1<<0;
	static constexpr uint32_t	older_bits			= 1<<1;

	std::string							identify;
	std::string							title;
	std::string							url;
	uint32_t							flags			= enable_bits
														;

	amtrs::ref<amtrs::chrono::calendar>	calendar;

	bool enable() const noexcept { return flags & enable_bits; }
	void enable(bool _on) noexcept { if (_on) { flags |= enable_bits; } else { flags &= ~enable_bits; } }

	bool older() const noexcept { return flags & older_bits; }
	void older(bool _on) noexcept { if (_on) { flags |= older_bits; } else { flags &= ~older_bits; } }
};


struct	schedule_downloader
{
	enum class	status_t
	{
		busy			= 0,		//!< 何もしていない
		connecting		= 1,		//!< 接続開始
		success			= 2,		//!< 通信成功
		failure			= 3,		//!< 通信失敗
	};
	static constexpr status_t	connecting	= status_t::connecting;
	static constexpr status_t	success		= status_t::success;
	static constexpr status_t	failure		= status_t::failure;

	// ========================================================================
	//! 初期化を行う。
	// ------------------------------------------------------------------------
	static void initialize();

	// ========================================================================
	//! 通信を開始する。
	// ------------------------------------------------------------------------
	static void start();

	// ========================================================================
	//! システムカレンダーの同期を行う
	// ------------------------------------------------------------------------
	static void sync(std::string const& _calendarId);

	// ========================================================================
	//! 同期を停止する。
	// ------------------------------------------------------------------------
	static void pause();

	// ========================================================================
	//! 現在のステータスを取得する。
	// ------------------------------------------------------------------------
	static status_t state();

	// ========================================================================
	//! 現在のステータスを保存する。
	// ------------------------------------------------------------------------
	//! enumrate_source() で取得したデータを変更した際にも呼び出してください。
	// ------------------------------------------------------------------------
	static void save();

	// ========================================================================
	//! イベントソースを追加します。
	// ------------------------------------------------------------------------
	static void add_eventsource(schedule_source const& _es);

	// ========================================================================
	//! イベントソースを削除します。
	// ------------------------------------------------------------------------
	static void erase_eventsource(std::string _identify);

	// ========================================================================
	//! スケジュールを指定するバッファに反映させます。
	// ------------------------------------------------------------------------
	//! return
	//!		通信終了後、初めてupdate_toを呼び出したときは true を返す
	// ------------------------------------------------------------------------
	static bool update_to(save::schedulelist& _buffer);

	// ========================================================================
	//! イベントソースを列挙します。
	// ------------------------------------------------------------------------
	//! 取得したデータを変更した際は save() を呼び出してください。
	// ------------------------------------------------------------------------
	static void enumrate_source(std::function<void(schedule_source*)> _callback);


	static void update(std::string _calendar_id, amtrs::chrono::calendar::event _event);
	static void remove(std::string _calendar_id, std::string _event_id);
};



#endif

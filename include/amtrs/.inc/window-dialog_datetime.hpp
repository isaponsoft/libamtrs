/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__window__dateinput_dialog__hpp
#define	__libamtrs__window__dateinput_dialog__hpp
AMTRS_NAMESPACE_BEGIN


class	dateinput_dialog
		: public ref_object
{
public:
	enum class event_type
	{
		ok,
		cancel,
	};

	using	event_callback	= bool(event_type _event, dateinput_dialog* _sender);
	using	value_type		= int;


	static ref<dateinput_dialog> create();


	auto year() const noexcept -> value_type { return mYear; }
	auto month()  const noexcept -> value_type { return mMon;  }
	auto day()  const noexcept -> value_type { return mDay;  }

	chrono::days days() const noexcept { return { mYear, mMon, mDay }; }


	void show() { on_show(); }

	// ========================================================================
	//! 時間を設定します。
	// ------------------------------------------------------------------------
	void set_date(value_type _year, value_type _mon, value_type _day);


	// ========================================================================
	//! 時間を設定します。
	// ------------------------------------------------------------------------
	void set_date(chrono::timev _t);


	// ========================================================================
	//! ダイアログを閉じます。
	// ------------------------------------------------------------------------
	void close();


	// ========================================================================
	//! イベント発生時に呼ばれるコールバック
	// ------------------------------------------------------------------------
	//! OKもしくはCancelが押されたときに呼ばれます。
	//! コールバックが true を返した場合、ダイアログを閉じます。
	// ------------------------------------------------------------------------
	void set_event_callback(std::function<event_callback>&& _callback);

protected:
	virtual void on_show() = 0;

	virtual void on_close() = 0;

	virtual void on_set_date(value_type _year, value_type _mon, value_type _day) = 0;


	// ドライバからのイベント通知を行う
	bool notify_event(event_type _event);

	// ドライバからの時間変更通知を行う
	void notify_set_date(value_type _year, value_type _mon, value_type _day);

private:
	std::function<event_callback>		mEventCallback;
	value_type							mYear	= 0;
	value_type							mMon	= 0;
	value_type							mDay	= 0;
};


inline void dateinput_dialog::set_date(value_type _year, value_type _mon, value_type _day)
{
	if (mYear != _year && mMon != _mon && mDay != _day)
	{
		mYear	= _year;
		mMon	= _mon;
		mDay	= _day;
		on_set_date(_year, _mon, _day);
	}
}


inline void dateinput_dialog::set_date(chrono::timev _t)
{
	set_date(_t.year(), _t.month(), _t.mday());
}


inline void dateinput_dialog::close()
{
	on_close();
}


inline void dateinput_dialog::set_event_callback(std::function<event_callback>&& _callback)
{
	mEventCallback	= _callback;
}



inline bool dateinput_dialog::notify_event(event_type _event)
{
	bool	res	= true;
	if (mEventCallback)
	{
		res	= mEventCallback(_event, this);
	}
	if (res)
	{
		close();
	}
	return	res;
}


inline void dateinput_dialog::notify_set_date(value_type _year, value_type _mon, value_type _day)
{
	mYear	= _year;
	mMon	= _mon;
	mDay	= _day;
}



AMTRS_NAMESPACE_END
#endif

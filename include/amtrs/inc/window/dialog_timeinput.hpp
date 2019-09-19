/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__window__timeinput_dialog__hpp
#define	__libamtrs__window__timeinput_dialog__hpp
#include <functional>
#include "def.hpp"
#include "window.hpp"
AMTRS_NAMESPACE_BEGIN


class	timeinput_dialog
		: public ref_object
{
public:
	enum class event_type
	{
		ok,
		cancel,
	};

	using	event_callback	= bool(event_type _event, timeinput_dialog* _sender);
	using	value_type		= int;


	static ref<timeinput_dialog> create();


	auto hour()   const noexcept -> value_type { return mHour; }
	auto minute() const noexcept -> value_type { return mMin;  }


	// ========================================================================
	//! 時間を設定します。
	// ------------------------------------------------------------------------
	void set_time(value_type _hour, value_type _min);


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

	virtual void on_close() = 0;

	virtual void on_set_time(value_type _hour, value_type _min) = 0;


	// ドライバからのイベント通知を行う
	bool notify_event(event_type _event);

	// ドライバからの時間変更通知を行う
	void notify_set_time(value_type _hour, value_type _min);

private:
	std::function<event_callback>		mEventCallback;
	value_type							mHour	= 0;
	value_type							mMin	= 0;
};


inline void timeinput_dialog::set_time(value_type _hour, value_type _min)
{
	if (mHour != _hour && mMin != _min)
	{
		mHour	= _hour;
		mMin	= _min;
		on_set_time(_hour, _min);
	}
}


inline void timeinput_dialog::close()
{
	on_close();
}


inline void timeinput_dialog::set_event_callback(std::function<event_callback>&& _callback)
{
	mEventCallback	= _callback;
}



inline bool timeinput_dialog::notify_event(event_type _event)
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


inline void timeinput_dialog::notify_set_time(value_type _hour, value_type _min)
{
	mHour		= _hour;
	mMin		= _min;
}



AMTRS_NAMESPACE_END
#include AMTRS_MAKE_INCLUDE_PLATFORM(dialog_timeinput)
#endif

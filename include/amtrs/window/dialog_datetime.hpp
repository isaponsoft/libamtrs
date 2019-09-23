/******************************************************************************
Copyright (c) 2019, isaponsoft (Isao Shibuya)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the libamtrs project.
******************************************************************************/
#ifndef	__libamtrs__window__dateinput_dialog__hpp
#define	__libamtrs__window__dateinput_dialog__hpp
#include <functional>
#include "def.hpp"
#include "window.hpp"
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


	// ========================================================================
	//! 時間を設定します。
	// ------------------------------------------------------------------------
	void set_date(value_type _year, value_type _mon, value_type _day);


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
#include AMTRS_MAKE_INCLUDE_PLATFORM(dialog_datetime)
#endif

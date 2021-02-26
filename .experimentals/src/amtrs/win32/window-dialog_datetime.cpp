/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#if	0
#include <amtrs/.driver/win32/window-window.hpp>
#include <amtrs/.driver/win32/.api-xaml.hpp>

//#include <winrt\Windows.UI.Xaml.Controls.h>

#include <amtrs/window/dialog_datetime.hpp>

using namespace winrt::Windows::UI::Xaml::Controls;

AMTRS_NAMESPACE_BEGIN




class	dateinput_dialog_impl
		: public dateinput_dialog
{
public:

	using	super_type	= dateinput_dialog;
	using	super_type::notify_event;
	using	super_type::notify_set_date;

	using	TimeSpan	= std::chrono::time_point<winrt::clock,winrt::Windows::Foundation::TimeSpan>;

	~dateinput_dialog_impl()
	{
		close();
	}


	void close()
	{
		mCalendar	= nullptr;
		mDialog.close();
	}

	virtual void on_close() override
	{
		close();
	}


	virtual void on_set_date(value_type _year, value_type _mon, value_type _day) override
	{
		std::time_t		t	= amtrs::chrono::timev::mktime(_year, _mon, _day).get();
		auto			dt	= winrt::clock::from_time_t(t);
		mCalendar.SelectedDates().Append(dt);
	}

	void initialize()
	{
	}


	virtual void on_show() override
	{
		mDialog.initialize({L"OK", L"Cancel"},1, [this](auto id)
		{
			if (id == 0)
			{
				auto	sels	= mCalendar.SelectedDates();
				if (sels.Size() > 0)
				{
					auto	dt		= sels.GetAt(0);
					auto	days	= chrono::timev(winrt::clock::to_time_t(dt)).days();
					this->notify_set_date(days.year, days.mon, days.day);
					this->notify_event(event_type::ok);
					return;
				}
			}
			this->notify_event(event_type::cancel);
			close();
		});
		mDialog.panel().Children().Append(mCalendar = CalendarView());
		mDialog.show();
	}



private:
	os::win32::xampldialog	mDialog;
	CalendarView			mCalendar{nullptr};

	friend	dateinput_dialog;
};

ref<dateinput_dialog> dateinput_dialog::create()
{
	ref<dateinput_dialog_impl>	thiz	= new dateinput_dialog_impl();
	thiz->initialize();
	return	thiz;
};


AMTRS_NAMESPACE_END
#endif

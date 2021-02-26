/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#if	0
#include <amtrs/.driver/win32/.api-windows.hpp>
#include <amtrs/.driver/win32/.api-xaml.hpp>
#include <winrt\Windows.UI.Xaml.Controls.h>

#include <amtrs/window/dialog_timeinput.hpp>

using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::Foundation;

AMTRS_NAMESPACE_BEGIN




class	timeinput_dialog_impl
		: public timeinput_dialog
{
public:

	using	super_type	= timeinput_dialog;
	using	super_type::notify_set_time;

	~timeinput_dialog_impl()
	{
		close();
	}

	void close()
	{
		mPicker	= nullptr;
		mDialog.close();
	}

	void initialize()
	{
	}

	virtual void on_close() override
	{
		close();
	}


	virtual void on_set_time(value_type _hour, value_type _min) override
	{
		TimeSpan	ts(0);
		ts += std::chrono::minutes(_min);
		ts += std::chrono::hours(_hour);
		mPicker.SelectedTime(ts);
	}


	virtual void on_show() override
	{
		mDialog.initialize({L"OK", L"Cancel"},1, [this](auto id)
		{
			if (id == 0)
			{
				TimeSpan	ts	= mPicker.SelectedTime().Value();
				auto		min	= std::chrono::duration_cast<std::chrono::minutes>(ts);
				this->notify_set_time(min.count()/60, min.count()%60);
				this->notify_event(event_type::ok);
				return;
			}
			this->notify_event(event_type::cancel);
			close();
		});
		mDialog.panel().Children().Append(mPicker = TimePicker());
		mDialog.show();
	}


private:
	os::win32::xampldialog	mDialog;
	TimePicker				mPicker{nullptr};

	friend	timeinput_dialog;
};

ref<timeinput_dialog> timeinput_dialog::create()
{
	ref<timeinput_dialog_impl>	thiz	= new timeinput_dialog_impl();
	thiz->initialize();
	return	thiz;
};


AMTRS_NAMESPACE_END
#endif

/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#if	0
#include <amtrs/.driver/win32/.api-windows.hpp>
#include <amtrs/.driver/win32/.api-xaml.hpp>
#include <amtrs/.driver/win32/.api-winrt-std.hpp>
#include <winrt\Windows.UI.Xaml.Controls.h>

#include <amtrs/window/dialog_textinput.hpp>

using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::Foundation;

AMTRS_NAMESPACE_BEGIN




class	textinput_dialog_impl
		: public textinput_dialog
{
public:

	using	super_type	= textinput_dialog;

	~textinput_dialog_impl()
	{
		close();
	}

	void close()
	{
		mEdit	= nullptr;
		mDialog.close();
	}

	void initialize()
	{
	}


	virtual void on_close() override
	{
		close();
	}


	virtual void on_text_change(std::string_view _text) override
	{
		mEdit.Text(winrt::to_hstring(_text));
	}


	virtual void on_show() override
	{
		mDialog.initialize({L"OK", L"Cancel"},1, [this](auto id)
		{
			if (id == 0)
			{
				mText	= amtrs::to_string(winrt::hstring(mEdit.Text()));
				notify_text_change();
			}
			this->notify_return();
			close();
		});
		mEdit = TextBox();
		mEdit.AcceptsReturn(false);
		mDialog.panel().Children().Append(mEdit);
		mDialog.show();
	}



private:
	os::win32::xampldialog	mDialog;
	TextBox 				mEdit{nullptr};

	friend	textinput_dialog;
};

ref<textinput_dialog> textinput_dialog::create()
{
	ref<textinput_dialog_impl>	thiz	= new textinput_dialog_impl();
	thiz->initialize();
	return	thiz;
};


AMTRS_NAMESPACE_END
#endif

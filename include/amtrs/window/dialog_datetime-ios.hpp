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
#ifndef	__libamtrs__window__dialog_datetime_ios__hpp
#define	__libamtrs__window__dialog_datetime_ios__hpp
#import <UIKit/UIKit.h>
#include <functional>
#include "../../chrono.hpp"
#include "def.hpp"
#include "window.hpp"

AMTRS_NAMESPACE_BEGIN
class	dateinput_dialog_ios;
AMTRS_NAMESPACE_END

@interface DateInputDialogController : UIViewController<UITextFieldDelegate>
{
	@public
	amtrs::dateinput_dialog_ios*	_dialog;
}
-(void)cancelCallback: (UIBarButtonItem*)sender;
-(void)okCallback: (UIBarButtonItem*)sender;
-(void)changeCallback: (UIDatePicker*)sender;
@end


AMTRS_NAMESPACE_BEGIN


class	dateinput_dialog_ios
		: public dateinput_dialog
{
	using	_base_type	= dateinput_dialog;

public:
	using	_base_type::notify_event;
	using	_base_type::notify_set_date;

	void initialize();


	UIViewController* get_main_controller()
	{
		return	os::ios::app_instance::get_instance()->get_controller();
	}

	virtual void on_close() override;

	virtual void on_set_date(value_type _year, value_type _month, value_type _day) override
	{
		if (mDatePicker)
		{
			[mDatePicker setDate:to_NSDate({_year, _month,_day, 0, 0, 0})];
		}
	}

private:
	DateInputDialogController*	mController	= nullptr;

	UIView*						mPopup		= nullptr;
	UIToolbar*					mToolbar	= nullptr;
	UIDatePicker*				mDatePicker	= nullptr;

	friend	dateinput_dialog;
	friend	DateInputDialogController;
};



inline ref<dateinput_dialog> dateinput_dialog::create()
{
	ref<dateinput_dialog_ios>	thiz	= new dateinput_dialog_ios();
	thiz->initialize();
	return	thiz;
};



inline void dateinput_dialog_ios::initialize()
{
	AMTRS_DEBUG_LOG("dateinput_dialog_ios::initialize()");
	mController				= [DateInputDialogController alloc];
	mController->_dialog	= this;
	dispatch_async(dispatch_get_main_queue(), ^{

		// ---------------------------------
		CGRect			ss			= [[UIScreen mainScreen] bounds];
		float			width		= ss.size.width;
		float			height		= (float)((int)ss.size.height/3);
		float			top			= ss.size.height - height;
		// ---------------------------------


		mPopup						= [[UIView alloc] initWithFrame:ss];
		mPopup.backgroundColor		= [[UIColor blackColor] colorWithAlphaComponent:0.5];

		// ヘッダ部
		mToolbar					= [[UIToolbar alloc] initWithFrame:CGRectMake(0, top-44, width, 44)];
		[mToolbar setBarStyle:UIBarStyleDefault];

		auto*		btnCancel		= [[UIBarButtonItem alloc] initWithTitle:@"Cancel"
										style:UIBarButtonItemStylePlain
										target:mController
										action:@selector(cancelCallback:)];
		auto*		btnOk			= [[UIBarButtonItem alloc] initWithTitle:@"Done"
										style:UIBarButtonItemStylePlain
										target:mController
										action:@selector(okCallback:)];
		auto*		btnSpace		= [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemFlexibleSpace target:nil action:nil];


		mToolbar.items 				= @[btnCancel, btnSpace, btnOk];
		[mPopup addSubview:mToolbar];

		// ピッカー
		mDatePicker					= [[UIDatePicker alloc] initWithFrame:CGRectMake(0, top, width, height)];
		mDatePicker.backgroundColor	= [UIColor whiteColor];
		[mDatePicker setDatePickerMode:UIDatePickerModeDate];
		[mDatePicker addTarget:mController action:@selector(changeCallback:) forControlEvents:UIControlEventValueChanged];
		[mPopup addSubview:mDatePicker];

		on_set_date(_base_type::year(), _base_type::month(), _base_type::day());

		[get_main_controller().view addSubview:mPopup];

	});
}


inline void dateinput_dialog_ios::on_close()
{
	dispatch_async(dispatch_get_main_queue(), ^{
		[mPopup removeFromSuperview];
		mPopup		= nullptr;
		mToolbar	= nullptr;
		mDatePicker	= nullptr;
	});
}

AMTRS_NAMESPACE_END
#endif

/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <UIKit/UIKit.h>
#include <functional>
#include <amtrs/chrono/datetime.hpp>
#include <amtrs/window/dialog_timeinput.hpp>
#include <amtrs/.driver/ios/applekit/app_delegate.hpp>

AMTRS_NAMESPACE_BEGIN
class	timeinput_dialog_ios;
AMTRS_NAMESPACE_END

@interface TimeInputDialogController : UIViewController<UITextFieldDelegate>
{
	@public
	amtrs::timeinput_dialog_ios*	_dialog;
}
-(void)cancelCallback: (UIBarButtonItem*)sender;
-(void)okCallback: (UIBarButtonItem*)sender;
-(void)changeCallback: (UIDatePicker*)sender;
@end


AMTRS_NAMESPACE_BEGIN





class	timeinput_dialog_ios
		: public timeinput_dialog
{
	using	_base_type	= timeinput_dialog;

public:
	using	_base_type::notify_event;
	using	_base_type::notify_set_time;

	void initialize();


	UIViewController* get_main_controller()
	{
		return	os::ios::app_instance::get_instance()->get_controller();
	}

	virtual void on_close() override;

	virtual void on_set_time(value_type _hour, value_type _min) override
	{
		AMTRS_DEBUG_LOG("Hour=%02d, Minute=%02d", (int)_base_type::hour(), (int)_base_type::minute());
		if (mDatePicker)
		{
			[mDatePicker setDate:to_NSDate({1977, 1, 1, _base_type::hour(), _base_type::minute(), 0})];
		}
	}

	virtual void on_show() override
	{
	}

private:
	TimeInputDialogController*	mController	= nullptr;

	UIView*						mPopup		= nullptr;
	UIToolbar*					mToolbar	= nullptr;
	UIDatePicker*				mDatePicker	= nullptr;

	friend	timeinput_dialog;
	friend	TimeInputDialogController;
};



ref<timeinput_dialog> timeinput_dialog::create()
{
	ref<timeinput_dialog_ios>	thiz	= new timeinput_dialog_ios();
	thiz->initialize();
	return	thiz;
};



void timeinput_dialog_ios::initialize()
{
	AMTRS_DEBUG_LOG("timeinput_dialog_ios::initialize()");
	mController				= [TimeInputDialogController alloc];
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
		[mDatePicker setDatePickerMode:UIDatePickerModeTime];
		[mDatePicker addTarget:mController action:@selector(changeCallback:) forControlEvents:UIControlEventValueChanged];
		[mPopup addSubview:mDatePicker];

		on_set_time(_base_type::hour(), _base_type::minute());

		[get_main_controller().view addSubview:mPopup];

	});
}


void timeinput_dialog_ios::on_close()
{
	if (mPopup)
	{
		UIView*	view	= mPopup;
		mPopup		= nullptr;
		mToolbar	= nullptr;
		mDatePicker	= nullptr;
		dispatch_async(dispatch_get_main_queue(), ^{
			[view removeFromSuperview];
		});
	}
}

AMTRS_NAMESPACE_END




@implementation TimeInputDialogController

-(void)cancelCallback: (UIBarButtonItem*)sender
{
	_dialog->notify_event(amtrs::timeinput_dialog_ios::event_type::cancel);
}


-(void)okCallback: (UIBarButtonItem*)sender
{
	_dialog->notify_event(amtrs::timeinput_dialog_ios::event_type::ok);
}

-(void)changeCallback: (UIDatePicker*)sender
{
	auto	tm	= amtrs::localtime([sender date]);
	_dialog->notify_set_time(tm.tm_hour, tm.tm_min);
}

@end


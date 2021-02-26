/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#import <UIKit/UIKit.h>
#include <functional>
#include <amtrs/chrono/datetime.hpp>
#include <amtrs/window/dialog_datetime.hpp>
#include <amtrs/.driver/ios/applekit/app_delegate.hpp>


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
public:
	using	super_type	= dateinput_dialog;

	using	super_type::notify_event;
	using	super_type::notify_set_date;

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
			[mDatePicker setDate:to_NSDate({_year, _month-1,_day+1, 0, 0, 0})];
		}
	}

	virtual void on_show() override
	{
	}

private:
	DateInputDialogController*	mController	= nullptr;

	UIView*						mPopup		= nullptr;
	UIToolbar*					mToolbar	= nullptr;
	UIDatePicker*				mDatePicker	= nullptr;

	friend	dateinput_dialog;
	friend	DateInputDialogController;
};



ref<dateinput_dialog> dateinput_dialog::create()
{
	ref<dateinput_dialog_ios>	thiz	= new dateinput_dialog_ios();
	thiz->initialize();
	return	thiz;
};



void dateinput_dialog_ios::initialize()
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

		on_set_date(super_type::year(), super_type::month(), super_type::day());

		[get_main_controller().view addSubview:mPopup];

	});
}


void dateinput_dialog_ios::on_close()
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



@implementation DateInputDialogController

-(void)cancelCallback: (UIBarButtonItem*)sender
{
	_dialog->notify_event(amtrs::dateinput_dialog_ios::event_type::cancel);
}


-(void)okCallback: (UIBarButtonItem*)sender
{
	_dialog->notify_event(amtrs::dateinput_dialog_ios::event_type::ok);
}

-(void)changeCallback: (UIDatePicker*)sender
{
	auto	tm	= amtrs::localtime([sender date]);
	_dialog->notify_set_date(tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
}

@end

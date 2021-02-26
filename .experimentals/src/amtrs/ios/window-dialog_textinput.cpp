/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <functional>
#include <amtrs/application/@>
#include <amtrs/window/dialog_textinput.hpp>

AMTRS_NAMESPACE_BEGIN
class	textinput_dialog_ios;
AMTRS_NAMESPACE_END

@interface TextInputDialogController : UIViewController<UITextFieldDelegate>
{
	@public
	amtrs::textinput_dialog_ios*	_dialog;
}
- (void)textFieldDidEndEditing:(UITextField *)textField;
-(BOOL)textFieldShouldReturn:(UITextField*)textField;
-(void)textFieldDidChange :(UITextField *) textField;
@end


AMTRS_NAMESPACE_BEGIN


class	textinput_dialog_ios
		: public textinput_dialog
{
	using	_base_type	= textinput_dialog;

public:
	using	_base_type::notify_text_change;
	using	_base_type::notify_return;


	//! set_string() によってテキストが変更されたことを派生クラスへ通達します。
	virtual void on_text_change(std::string_view _text) override
	{
		auto&				tx	= _base_type::text();
		mTextField.text		= tx.empty() ? nil : [NSString stringWithCString: tx.data() encoding:NSUTF8StringEncoding];
	}

	virtual void on_close() override
	{
		mController->_dialog	= nullptr;

		UITextField*	tf	= mTextField;
		dispatch_async(dispatch_get_main_queue(), ^{
			AMTRS_DEBUG_LOG("removeFromSuperview");
			[tf removeFromSuperview];
		});
	}

	virtual void on_show() override
	{
	}

	UIViewController* get_main_controller()
	{
		return	os::ios::app_instance::get_instance()->get_controller();
	}

	void initialize();


	void _set_string(std::string&& _text)
	{
		mText	= _text;
	}

private:
	UITextField*				mTextField	= nullptr;
	TextInputDialogController*	mController	= nullptr;

	friend	textinput_dialog;
	friend	TextInputDialogController;
};



ref<textinput_dialog> textinput_dialog::create()
{
	ref<textinput_dialog_ios>	thiz	= new textinput_dialog_ios();
	
	thiz->initialize();

	return	thiz;
};


void textinput_dialog_ios::initialize()
{
	mController				= [TextInputDialogController alloc];
	mController->_dialog	= this;
	dispatch_async(dispatch_get_main_queue(), ^{
		CGRect			ss		= [[UIScreen mainScreen] bounds];
		float			width	= ss.size.width;
		float			height	= ss.size.height;

		CGRect 			rect	= CGRectMake(0, 0, width, height);
		//CGRect 			rect	= CGRectMake(0, height - height/2, width, height/2);


		auto&				tx	= _base_type::text();

		mTextField					= [[UITextField alloc] initWithFrame:rect];
		mTextField.text				= tx.empty() ? nil : [NSString stringWithCString: tx.data() encoding:NSUTF8StringEncoding];
		//mTextField.placeholder		= mHelp.empty() ? nil : [NSString stringWithCString: mHelp.data() encoding:NSUTF8StringEncoding];
		mTextField.keyboardType		= UIKeyboardTypeDefault;
		mTextField.returnKeyType	= UIReturnKeyNext;
		mTextField.delegate			= mController;
		mTextField.textColor 		= [UIColor colorWithRed:0 green:0 blue:0 alpha:1];
		mTextField.backgroundColor	= [UIColor colorWithRed:1 green:1 blue:1 alpha:1];
		[mTextField addTarget:mController action:@selector(textFieldDidChange:) forControlEvents:UIControlEventEditingChanged];

		[mTextField setKeyboardAppearance:UIKeyboardAppearanceDefault];  
		[mTextField setKeyboardType:UIKeyboardTypeDefault];
		[mTextField setReturnKeyType:UIReturnKeyDefault];
		[get_main_controller().view addSubview:mTextField];
		[mTextField becomeFirstResponder];
	});
}

AMTRS_NAMESPACE_END





@implementation TextInputDialogController

- (void)textFieldDidEndEditing:(UITextField *)textField
{ @autoreleasepool {
	AMTRS_DEBUG_LOG("textFieldDidEndEditing");
} }


-(BOOL)textFieldShouldReturn:(UITextField*)textField
{ @autoreleasepool {
	AMTRS_DEBUG_LOG("textFieldShouldReturn");
	_dialog->notify_return();
    return	YES;
} }


-(void)textFieldDidChange :(UITextField *) textField
{
	const char*	text	= [textField.text UTF8String];
	AMTRS_DEBUG_LOG("textFieldDidChange %s", text);
	_dialog->_set_string(text);
	_dialog->notify_text_change();
}

@end


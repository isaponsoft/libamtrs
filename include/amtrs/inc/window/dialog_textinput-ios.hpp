/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__window__textinput_dialog_ios__hpp
#define	__libamtrs__window__textinput_dialog_ios__hpp
#include <functional>
#include "def.hpp"
#include "window.hpp"

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


	//! set_text() によってテキストが変更されたことを派生クラスへ通達します。
	virtual void on_text_change(std::string_view _text) override
	{
		auto&				tx	= _base_type::text();
		mTextField.text		= tx.empty() ? nil : [NSString stringWithCString: tx.data() encoding:NSUTF8StringEncoding];
	}

	virtual void on_close() override
	{
		dispatch_async(dispatch_get_main_queue(), ^{
			[mTextField removeFromSuperview];
		});
	}

	UIViewController* get_main_controller()
	{
		return	os::ios::app_instance::get_instance()->get_controller();
	}

	void initialize();


	void _set_text(std::string&& _text)
	{
		mText	= _text;
	}

private:
	UITextField*				mTextField	= nullptr;
	TextInputDialogController*	mController	= nullptr;

	friend	textinput_dialog;
	friend	TextInputDialogController;
};



inline ref<textinput_dialog> textinput_dialog::create()
{
	ref<textinput_dialog_ios>	thiz	= new textinput_dialog_ios();
	
	thiz->initialize();

	return	thiz;
};


inline void textinput_dialog_ios::initialize()
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
#endif

/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#import <UIKit/UIKit.h>
#include <map>
#include <amtrs/window/window.hpp>
#include <amtrs/window/dialog_textinput.hpp>
#include <amtrs/window/dialog_timeinput.hpp>
#include <amtrs/window/dialog_datetime.hpp>



@implementation	iosWindowView

+ (Class)layerClass
{
	return [CAEAGLLayer class];
}

-(id) initWithFrame:(CGRect)frame
{
	self						= [super initWithFrame:frame];
	self.multipleTouchEnabled	= YES;
	[self setBackgroundColor:[UIColor blackColor]];
	if ([self respondsToSelector:@selector(setContentScaleFactor:)])
	{
		//AMTRS_DEBUG_LOG("Set contentScaleFactor");
		self.contentScaleFactor = [[UIScreen mainScreen] scale];
	}
	return	self;
}

-(void)touchesBegan:(NSSet*)touches withEvent:(UIEvent *)event
{
	window->touches_began(touches, event);
}

-(void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
	window->touches_moved(touches, event);
}

-(void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	window->touches_ended(touches, event);
}

-(void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
	window->touches_cancelled(touches, event);
}



-(void) Update 
{
}

-(void) Render
{
}

@end




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


-(void)textFieldDidChange :(UITextField *) textField{
	const char*	text	= [textField.text UTF8String];
	AMTRS_DEBUG_LOG("textFieldDidChange %s", text);
	_dialog->_set_text(text);
	_dialog->notify_text_change();
}

@end




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
	_dialog->notify_set_date(tm.tm_year + 1, tm.tm_mon, tm.tm_mday);
}

@end

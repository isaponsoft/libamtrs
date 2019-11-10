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
#ifndef	__libamtrs__window__window_ios__hpp
#define	__libamtrs__window__window_ios__hpp
// Framework
// 	UIKit.framework 
// 	QuartzCore.framework 

#import <UIKit/UIKit.h>
#include <vector>
#include <map>
#include "../os/ios/app_delegate.hpp"
#include "../input/@"

AMTRS_NAMESPACE_BEGIN
class	window_ios_interface;
AMTRS_NAMESPACE_END


@interface iosWindowView : UIView
{
	@public	amtrs::window_ios_interface*	window;
}
+ (Class)layerClass;
-(id) initWithFrame:(CGRect)frame;
-(void)touchesBegan:(NSSet*)touches withEvent:(UIEvent *)event;
-(void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event;
-(void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event;
-(void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event;
-(void) Update;
-(void) Render;
@end


AMTRS_NAMESPACE_BEGIN

class	window_ios_interface
{
public:
	virtual void touches_began(NSSet* _touches, UIEvent* _event) = 0;
	virtual void touches_moved(NSSet* _touches, UIEvent* _event) = 0;
	virtual void touches_ended(NSSet* _touches, UIEvent* _event) = 0;
	virtual void touches_cancelled(NSSet* _touches, UIEvent* _event) = 0;
};


template<class BaseT>
class	window_ios
		: public BaseT
		, public window_ios_interface
{
public:
	using	_base_type		= BaseT;
	using	size_type		= typename _base_type::size_type;
	using	context_type	= typename _base_type::context_type;

	virtual size_type size() const noexcept override
	{
		return	mWindowSize;
	}

	virtual context_type get_context() const noexcept override
	{
		return	mView;
	}


	void initialize(size_type _size)
	{
		CGRect		ss	= [[UIScreen mainScreen] bounds];

		// ビューとコントローラを生成
		mView				= [[iosWindowView alloc] initWithFrame:ss];
		mView->window		= this;

        auto*	ctl	= os::ios::app_instance::get_instance()->get_controller();
		[ctl.view addSubview:mView];

		// ウィンドウサイズを保存
		float	scale	= mView.contentScaleFactor;
		mWindowSize	= size_type(ss.size.width * scale, ss.size.height * scale);
	}

	iosWindowView* view() const noexcept { return mView; }





	void touches_began(NSSet* _touches, UIEvent* _event) override
	{
		float	scale	= mView.contentScaleFactor;
		for (UITouch* touch in _touches)
		{
			CGPoint	location = [touch locationInView:mView];
			mouse_event	e;
			e.buttons		= mouse_event::button_id(1<<get_touches_id(touch));
			e.state			= mouse_event::status::press;
			e.point			= mouse_event::point_type(location.x * scale, location.y * scale);
			if (_base_type::onInputEvent)
			{
				_base_type::onInputEvent(&e);
			}
		}
	}

	void touches_moved(NSSet* _touches, UIEvent* _event) override
	{
		float	scale	= mView.contentScaleFactor;
		for (UITouch* touch in _touches)
		{
			CGPoint	location = [touch locationInView:mView];
			mouse_event	e;
			e.buttons		= mouse_event::button_id(1<<get_touches_id(touch));
			e.state			= mouse_event::status::drag;
			e.point			= mouse_event::point_type(location.x * scale, location.y * scale);
			if (_base_type::onInputEvent)
			{
				_base_type::onInputEvent(&e);
			}
		}
	}

	void touches_ended(NSSet* _touches, UIEvent* _event) override
	{
		float	scale	= mView.contentScaleFactor;
		for (UITouch* touch in _touches)
		{
			CGPoint	location = [touch locationInView:mView];
			mouse_event	e;
			e.buttons		= mouse_event::button_id(1<<get_touches_id(touch));
			e.state			= mouse_event::status::release;
			e.point			= mouse_event::point_type(location.x * scale, location.y * scale);
			if (_base_type::onInputEvent)
			{
				_base_type::onInputEvent(&e);
			}
			for (auto& t : mTouchIds)
			{
				if (t == touch)
				{
					t	= nullptr;
				}
			}
		}
	}

	void touches_cancelled(NSSet* _touches, UIEvent* _event) override
	{
		for (UITouch* touch in _touches)
		{
			for (auto& t : mTouchIds)
			{
				if (t == touch)
				{
					t	= nullptr;
				}
			}
		}
	}


protected:

	int get_touches_id(UITouch* _touch)
	{
		int	id	= 0;

		// まずは登録済みの中から探す
		for (auto* t : mTouchIds)
		{
			if (t == _touch)
			{
				return	id;
			}
			++id;
		}

		// 空いているところから探す
		id	= 0;
		for (auto* t : mTouchIds)
		{
			if (t == nullptr)
			{
				mTouchIds[id]	= _touch;
				return	id;
			}
			++id;
		}

		// 追加する
		mTouchIds.push_back(_touch);
		return	id;
	}

	std::vector<UITouch*>	mTouchIds;
	size_type				mWindowSize;
	iosWindowView*			mView;
};



AMTRS_NAMESPACE_END

#endif

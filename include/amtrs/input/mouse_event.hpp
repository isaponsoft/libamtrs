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
#ifndef	__libamtrs__input__mouse_event__hpp
#define	__libamtrs__input__mouse_event__hpp
#include "input_event.hpp"
AMTRS_NAMESPACE_BEGIN



struct	mouse_event
		: input_event
{
	using	point_type		= vec2<int>;

	//! マウスの時の拡張フラグ
	enum button_id : uint32_t
	{
		left			= 1<<0,			//!< 左ボタン
		right			= 1<<1,			//!< 右ボタン
		center			= 1<<2,			//!< 中ボタン
		button4			= 1<<3,			//!< 拡張ボタン
		button5			= 1<<4,			//!< 拡張ボタン
		button6			= 1<<5,			//!< 拡張ボタン
		button7			= 1<<6,			//!< 拡張ボタン
		button8			= 1<<7,			//!< 拡張ボタン
		button9			= 1<<8,			//!< 拡張ボタン
		button10		= 1<<9,			//!< 拡張ボタン
		button11		= 1<<10,		//!< 拡張ボタン
		button12		= 1<<11,		//!< 拡張ボタン
		button13		= 1<<12,		//!< 拡張ボタン
		button14		= 1<<13,		//!< 拡張ボタン
		button15		= 1<<14,		//!< 拡張ボタン
		button16		= 1<<15,		//!< 拡張ボタン
		controll		= 1<<24,		//!< コントロールキーを押しながらマウスを操作した
		shift			= 1<<25,		//!< シフトキーを押しながらマウスを操作した
	};

	mouse_event()
	{
		type	= mouse;
	}

	uint32_t		buttons	= 0;		//!< イベントを起こしたボタン
	point_type		point;				//!< 反応した時の座標
};


struct	mouse_wheel_event
		: input_event
{
	using	button_id	= mouse_event::button_id;
	using	point_type	= vec2<int>;

	enum wheel_axis
	{
		none,
		axis_x,
		axis_y,
		axis_z,
	};


	mouse_wheel_event()
	{
		type	= mouse_wheel;
	}

	point_type		point;				//!< 反応した時の座標

	// ホイールの情報
	wheel_axis	axis;
	float		delta;
};


template<class T>
constexpr vec2<T> input_event::get_point() const noexcept
{
	switch (type)
	{
		case input_event::mouse :		return	vec2<T>(static_cast<const mouse_event*>(this)->point);
		case input_event::mouse_wheel :	return	vec2<T>(static_cast<const mouse_wheel_event*>(this)->point);
		default:	return	{0, 0};
	}
}


template<class T>
inline constexpr void input_event::scale_point(vec2<T> _scale) noexcept
{
	switch (type)
	{
		case input_event::mouse :
			static_cast<mouse_event*>(this)->point *= _scale;
			break;

		case input_event::mouse_wheel :
			static_cast<mouse_wheel_event*>(this)->point *= _scale;
			break;

		default:
			break;
	}
}

inline constexpr mouse_wheel_event* input_event::get_mouse_wheel_event()       noexcept
{
	return	type == mouse_wheel ? static_cast<      mouse_wheel_event*>(this) : nullptr;
}


inline constexpr const mouse_wheel_event* input_event::get_mouse_wheel_event() const noexcept
{
	return	type == mouse_wheel ? static_cast<const mouse_wheel_event*>(this) : nullptr;
}


inline constexpr mouse_event* input_event::get_mouse_event()       noexcept
{
	return	type == mouse ? static_cast<      mouse_event*>(this) : nullptr;
}


inline constexpr const mouse_event* input_event::get_mouse_event() const noexcept
{
	return	type == mouse ? static_cast<const mouse_event*>(this) : nullptr;
}



AMTRS_NAMESPACE_END
#include "gamepad.hpp"
#endif

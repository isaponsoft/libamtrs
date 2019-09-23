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
#ifndef	__libamtrs__input__input_event__hpp
#define	__libamtrs__input__input_event__hpp
#include "../graphics/@"
#include "def.hpp"
#include "key_code.hpp"
AMTRS_NAMESPACE_BEGIN


struct	mouse_event;
struct	mouse_wheel_event;
struct	keyboard_event;



struct	input_event
{
	struct	optional;

	using	index_type		= unsigned int;

	enum status
	{
		press,				//!< 押下した
		release,			//!< 離した
		drag,				//!< ドラッグした(押したまま動かした)
		move,				//!< 移動した（押した状態ではない）
		cancel,				//!< イベントのキャンセルが発生した（非アクティブ化したなど）
		long_pressing,		//!< 長押しした
	};

	enum device_type
	{
		mouse,
		mouse_wheel,
		touch,
		keyboard,
	};


	device_type		type;
	status			state;

	optional*		opt		= nullptr;	//!< プラットフォーム固有コードを取得するためのポインタ。特にパラメータが存在しない場合は nullptr が格納される。


	// ========================================================================
	//! イベントがポインティングデバイス（マウスまたはタッチ）かどうか調べます。
	// ------------------------------------------------------------------------
	inline constexpr bool is_pointer() const noexcept;


	// ========================================================================
	//! イベントがキーボードイベントかどうか調べます。
	// ------------------------------------------------------------------------
	inline constexpr bool is_keyboard() const noexcept;


	// ========================================================================
	//! イベントが押下イベントか調べます。
	// ------------------------------------------------------------------------
	inline constexpr bool is_press() const noexcept;


	// ========================================================================
	//! イベントがポインティングデバイス（マウスまたはタッチ）なら座標を返します。
	// ------------------------------------------------------------------------
	//! 座標を持たない場合は (0, 0) を返します。
	// ------------------------------------------------------------------------
	template<class T>
	inline constexpr vec2<T> get_point() const noexcept;


	// ========================================================================
	//! ポインタ座標にスケールをかけます。
	// ------------------------------------------------------------------------
	//! 座標を持たない場合は何もしません。
	// ------------------------------------------------------------------------
	template<class T>
	inline constexpr void scale_point(vec2<T> _scale) noexcept;


	// ========================================================================
	//! マウスイベントにキャストします。異なる場合は nullptr を返します。
	// ------------------------------------------------------------------------
	inline constexpr       mouse_event* get_mouse_event()       noexcept;
	inline constexpr const mouse_event* get_mouse_event() const noexcept;

	// ========================================================================
	//! マウスイベントにキャストします。異なる場合は nullptr を返します。
	// ------------------------------------------------------------------------
	inline constexpr       mouse_wheel_event* get_mouse_wheel_event()       noexcept;
	inline constexpr const mouse_wheel_event* get_mouse_wheel_event() const noexcept;

	// ========================================================================
	//! キーボードイベントにキャストします。異なる場合は nullptr を返します。
	// ------------------------------------------------------------------------
	inline constexpr       keyboard_event* get_keyboard_event()       noexcept;
	inline constexpr const keyboard_event* get_keyboard_event() const noexcept;

};




constexpr bool input_event::is_pointer() const noexcept
{
	return	type == input_event::mouse
		||  type == input_event::mouse_wheel
		||  type == input_event::touch;
}


constexpr bool input_event::is_press() const noexcept
{
	return	state == press;
}





constexpr bool input_event::is_keyboard() const noexcept
{
	return	type == input_event::keyboard;
}


AMTRS_NAMESPACE_END
#endif

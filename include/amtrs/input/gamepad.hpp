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
#ifndef	__libamtrs__input__gamepad__hpp
#define	__libamtrs__input__gamepad__hpp
#include "../graphics/@"
AMTRS_NAMESPACE_BEGIN

// ============================================================================
//! ゲームパッドへのインターフェースを取得します。
// ----------------------------------------------------------------------------
//!
// ----------------------------------------------------------------------------
class	gamepad
		: public ref_object
{
public:
	static constexpr uint32_t		dpad_up			= 1<<0;		//!< デジタル方向ボタン
	static constexpr uint32_t		dpad_down		= 1<<1;		//!< デジタル方向ボタン
	static constexpr uint32_t		dpad_left		= 1<<2;		//!< デジタル方向ボタン
	static constexpr uint32_t		dpad_right		= 1<<3;		//!< デジタル方向ボタン

	static constexpr uint32_t		dpad_y			= 1<<7;		//!< デジタルYボタン
	static constexpr uint32_t		dpad_a			= 1<<4;		//!< デジタルAボタン
	static constexpr uint32_t		dpad_x			= 1<<6;		//!< デジタルXボタン
	static constexpr uint32_t		dpad_b			= 1<<5;		//!< デジタルBボタン

	static constexpr uint32_t		left_z			= 1<<8;
	static constexpr uint32_t		right_z			= 1<<9;

	static constexpr uint32_t		left_shoulder	= 1<<10;
	static constexpr uint32_t		left_trigger	= 1<<11;

	static constexpr uint32_t		right_shoulder	= 1<<12;
	static constexpr uint32_t		right_trigger	= 1<<13;

	static constexpr uint32_t		right_start		= 1<<14;
	static constexpr uint32_t		right_select	= 1<<15;

	static ref<gamepad> get_device(int _gamepad_id = 0);

	//! ゲームパッドの状態を更新します。
	virtual void update() = 0;

	//!< 現在、押下状態にあるボタンのマスクを取得します。
	virtual uint32_t buttons() const noexcept = 0;

	//!< 左スティックのXY値を取得します。
	virtual vec2<float> left_stick() const noexcept = 0;

	//!< 右スティックのXY値を取得します。
	virtual vec2<float> right_stick() const noexcept = 0;

	//!< left_trigger のアナログ値を取得します。
	virtual float left_trigger_level() const noexcept = 0;

	//!< right_trigger のアナログ値を取得します。
	virtual float right_trigger_level() const noexcept = 0;
};


AMTRS_NAMESPACE_END

#if		AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_WIN32
#include "gamepad-win32.hpp"
#elif	AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_ANDROID
#include "gamepad-android.hpp"
#else
AMTRS_NAMESPACE_BEGIN
// ゲームパッド未対応プラットフォームでは空のゲームパッドを返す
inline ref<gamepad> gamepad::get_device(int _gamepad_id)
{
	return	nullptr;
}
AMTRS_NAMESPACE_END
#endif


#endif

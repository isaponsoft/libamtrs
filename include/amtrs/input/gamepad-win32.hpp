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
#ifndef	__libamtrs__input__gamepad_win32__hpp
#define	__libamtrs__input__gamepad_win32__hpp
#include <map>
#include "../../os/win32.hpp"
#include <XInput.h>
#pragma comment(lib, "XInput.lib")
AMTRS_NAMESPACE_BEGIN

// ============================================================================
//! ゲームパッドへのインターフェースを取得します。
// ----------------------------------------------------------------------------
//!
// ----------------------------------------------------------------------------
class	gamepad_win32
		: public gamepad
{
public:
	gamepad_win32(int _gamepad_id, const XINPUT_STATE& _status)
		: mGamePadId(_gamepad_id)
		, mStatus(_status)
	{
		update_status();
	}

	~gamepad_win32()
	{
		get_instances().erase(get_instances().find(mGamePadId));
	}

	
	//! ゲームパッドの状態を更新します。
	virtual void update() override
	{
		XInputGetState((DWORD)mGamePadId, &mStatus);
		update_status();
	}

	//!< 現在、押下状態にあるボタンのマスクを取得します。
	virtual uint32_t buttons() const noexcept override
	{
		return	mButtons;
	}

	//!< 左スティックのXY値を取得します。
	virtual vec2<float> left_stick() const noexcept override { return mLeftStick; }

	//!< 右スティックのXY値を取得します。
	virtual vec2<float> right_stick() const noexcept override { return mRightStick; }

	//!< left_trigger のアナログ値を取得します。
	virtual float left_trigger_level() const noexcept override { return mLeftTrigger; }

	//!< right_trigger のアナログ値を取得します。
	virtual float right_trigger_level() const noexcept override { return mRightTrigger; }


protected:
	void update_status()
	{
		mButtons	= 0;
		if (mStatus.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP   ) { mButtons |= dpad_up;    }
		if (mStatus.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN ) { mButtons |= dpad_down;  }
		if (mStatus.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT ) { mButtons |= dpad_left;  }
		if (mStatus.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) { mButtons |= dpad_right; }

		if (mStatus.Gamepad.wButtons & XINPUT_GAMEPAD_START) { mButtons |= right_start;  }
		if (mStatus.Gamepad.wButtons & XINPUT_GAMEPAD_BACK ) { mButtons |= right_select; }

		if (mStatus.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB ) { mButtons |= left_z;  }
		if (mStatus.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) { mButtons |= right_z; }

		if (mStatus.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER ) { mButtons |= left_shoulder;  }
		if (mStatus.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) { mButtons |= right_shoulder; }

		if (mStatus.Gamepad.wButtons & XINPUT_GAMEPAD_A) { mButtons |= dpad_a; }
		if (mStatus.Gamepad.wButtons & XINPUT_GAMEPAD_B) { mButtons |= dpad_b; }
		if (mStatus.Gamepad.wButtons & XINPUT_GAMEPAD_X) { mButtons |= dpad_x; }
		if (mStatus.Gamepad.wButtons & XINPUT_GAMEPAD_Y) { mButtons |= dpad_y; }

		mLeftStick	= vec2<float>(
						(float)mStatus.Gamepad.sThumbLX / (float)32767.0f,
						(float)mStatus.Gamepad.sThumbLY / (float)32767.0f
					);

		mRightStick	= vec2<float>(
						(float)mStatus.Gamepad.sThumbRX / (float)32767.0f,
						(float)mStatus.Gamepad.sThumbRY / (float)32767.0f
					);

		mLeftTrigger	= (float)mStatus.Gamepad.bLeftTrigger  / (float)255.0f;
		mRightTrigger	= (float)mStatus.Gamepad.bRightTrigger / (float)255.0f;
	}

	int				mGamePadId	= -1;
	uint32_t		mButtons;
	vec2<float>		mLeftStick;
	vec2<float>		mRightStick;
	float			mLeftTrigger;
	float			mRightTrigger;
	XINPUT_STATE	mStatus;

	static std::map<int, gamepad*>& get_instances()
	{
		static std::map<int, gamepad*>	s_list;
		return	s_list;
	}

	friend	gamepad;
};


inline ref<gamepad> gamepad::get_device(int _gamepad_id)
{
	auto&	m	= gamepad_win32::get_instances();
	if (auto it = m.find(_gamepad_id); it != m.end())
	{
		return	it->second;
	}

	XINPUT_STATE	xs;
	if (ERROR_DEVICE_NOT_CONNECTED == XInputGetState((DWORD)_gamepad_id, &xs))
	{
		return	nullptr;
	}

	ref<gamepad_win32>	retval	= new gamepad_win32(_gamepad_id, xs);
	m.insert({_gamepad_id, retval.get()});
	return	retval;
}





AMTRS_NAMESPACE_END
#endif

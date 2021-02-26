/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__input__input_event__hpp
#define	__libamtrs__input__input_event__hpp
AMTRS_NAMESPACE_BEGIN

enum class key_code
{
	unkown			= 0x00,
	backspace_key	= 0x08,
	tab_key			= 0x09,
	clear_key		= 0x0c,
	return_key		= 0x0d,
	shift_key		= 0x10,
	control_key		= 0x11,
	menu_key		= 0x12,
	pause_key		= 0x13,
	capital_key		= 0x14,
	// IME
	kana_key		= 0x15,
	hangul_key		= 0x15,
	junja_key		= 0x17,
	final_key		= 0x18,		// IME "確定"
	hanja_key		= 0x19,
	kanji_key		= 0x19,
	escape_key		= 0x1b,
	convert_key		= 0x1c,
	nonconvert_key	= 0x1d,
	accept_key		= 0x1e,
	modechange_key	= 0x1f,

	space_key		= 0x20,
	prior_key		= 0x21,
	next_key		= 0x22,
	end_key			= 0x23,
	home_key		= 0x24,
	left_key		= 0x25,
	up_key			= 0x26,
	right_key		= 0x27,
	down_key		= 0x28,


	back_key		= 0x010001,	// Android Back key

	command_key		= 0x020001,	// OSX command key
};


struct	mouse_event;
struct	mouse_wheel_event;
struct	keyboard_event;



struct	input_event
{
	struct	optional;

	using	index_type		= unsigned int;

	enum status
	{
		press			= 1<<0,		//!< 押下した
		release			= 1<<1,		//!< 離した
		drag			= 1<<2,		//!< ドラッグした(押したまま動かした)
		move			= 1<<3,		//!< 移動した（押した状態ではない）
		cancel			= 1<<4,		//!< イベントのキャンセルが発生した（非アクティブ化したなど）
		long_pressing	= 1<<5,		//!< 長押しした
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



struct	keyboard_event
		: input_event
{
	keyboard_event()
	{
		type	= keyboard;
	}


	key_code	key;					//!< 押された/離されたボタン
};


inline constexpr       keyboard_event* input_event::get_keyboard_event()       noexcept { return type == keyboard ? static_cast<      keyboard_event*>(this) : nullptr; }
inline constexpr const keyboard_event* input_event::get_keyboard_event() const noexcept { return type == keyboard ? static_cast<const keyboard_event*>(this) : nullptr; }




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

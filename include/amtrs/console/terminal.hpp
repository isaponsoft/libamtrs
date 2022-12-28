/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__console__terminal__hpp
#define	__libamtrs__console__terminal__hpp
#include "../graphics/geometory/vec.hpp"
#include "../graphics/geometory/size.hpp"
#include "../io/fdesc.hpp"
#include "../memory/simplearray.hpp"
AMTRS_CONSOLE_NAMESPACE_BEGIN


/*
 * カーソルを移動します。
 */
AMTRS_API bool cursor_move(io::fdesc _target, graph::vec2<int> _move);
AMTRS_API bool cursor_move_up(io::fdesc _target, int _count = 1);
AMTRS_API bool cursor_move_down(io::fdesc _target, int _count = 1);
AMTRS_API bool cursor_move_left(io::fdesc _target, int _count = 1);
AMTRS_API bool cursor_move_right(io::fdesc _target, int _count = 1);


/*
 * カーソル位置を設定します。
 */
AMTRS_API bool cursor_position(io::fdesc _target, graph::vec2<int> _position);

/*
 * カーソル位置を読み取ります。
 */
AMTRS_API graph::vec2<int> cursor_position(io::fdesc _target);

/*
 * ターミナルの大きさを取得します。
 */
AMTRS_API graph::size2<int> terminal_size(io::fdesc _target);

/*
 * "ESC[" + _txt を書き込みます
 */
AMTRS_API bool write_escape_command(io::fdesc _target, text _txt);

/*
 * "ESC[" + _num + _txt を書き込みます
 */
AMTRS_API bool write_escape_command(io::fdesc _target, int _num, text _txt);



/*
 * ターミナルの操作をします
 */
class	terminal
{
public:
	using	keycode_type	= int16_t;

	#define	__MAKEKEY(n)	(1<<8) + n
	enum ctrlkey
	{
		ck_int			= __MAKEKEY(0),		// "Ctrl + C" or SIGINT
		ck_home			= __MAKEKEY(1),
		ck_end			= __MAKEKEY(2),
		ck_delete		= __MAKEKEY(3),
		ck_left			= __MAKEKEY(4),
		ck_right		= __MAKEKEY(5),
		ck_up			= __MAKEKEY(6),
		ck_down			= __MAKEKEY(7),
		ck_backspace	= __MAKEKEY(8),
		ck_pageup		= __MAKEKEY(9),
		ck_pagedown		= __MAKEKEY(10),
		ck_nextline		= __MAKEKEY(11),
		ck_backline		= __MAKEKEY(12),
		ck_setx			= __MAKEKEY(13),
		ck_sety			= __MAKEKEY(14),
		ck_insert		= __MAKEKEY(15),
		ck_sigint		= __MAKEKEY(16),
	};
	#undef	__MAKEKEY


	#define	__MAKEKEY(n)	(2<<8) + n
	enum funckey
	{
		fk_f1	= __MAKEKEY(1),		// "Ctrl + C" or SIGINT
		fk_f2	= __MAKEKEY(2),
		fk_f3	= __MAKEKEY(3),
		fk_f4	= __MAKEKEY(4),
		fk_f5	= __MAKEKEY(5),
		fk_f6	= __MAKEKEY(6),
		fk_f7	= __MAKEKEY(7),
		fk_f8	= __MAKEKEY(8),
		fk_f9	= __MAKEKEY(9),
		fk_f10	= __MAKEKEY(10),
		fk_f11	= __MAKEKEY(11),
		fk_f12	= __MAKEKEY(12),
	};
	#undef	__MAKEKEY


	struct	keyresult
	{
		keycode_type	code;
		int				repeat	= 0;
	};

	terminal() = default;
	terminal(terminal const&) = default;
	AMTRS_API terminal(io::fdesc _output, io::fdesc _input);
	AMTRS_API ~terminal();
	AMTRS_API keyresult getkey();	// キーを直接読み取る
	AMTRS_API graph::vec2<int> position() { return cursor_position(mOut); }
	AMTRS_API void position(graph::vec2<int> pos) { cursor_position(mOut, pos); }
	AMTRS_API graph::size2<int> size() { return terminal_size(mOut); }
	bool is_done() const noexcept { return mDone; }
	void done(bool _done) noexcept { mDone = _done; }
	static bool is_ctrlkey(keycode_type _k) noexcept { return (_k & 0xff00) ? true : false; }
	AMTRS_API void reset();

	io::fdesc& in() noexcept { return mIn; } 
	io::fdesc& out() noexcept { return mOut; } 

private:
	io::fdesc					mIn;
	io::fdesc					mOut;
	bool						mDone		= false;
	simplearray<unsigned char>	mPending;
	int							mPendingIn	= 0;
	void*						mData		= nullptr;
};
AMTRS_CONSOLE_NAMESPACE_END
#endif

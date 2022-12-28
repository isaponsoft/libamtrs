/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <io.h>
#include <amtrs/console/terminal.hpp>
AMTRS_CONSOLE_NAMESPACE_BEGIN
static DWORD	gDefaultConsoleMode	= 0;
static HANDLE GoHandle(io::fdesc _desc) noexcept { return reinterpret_cast<HANDLE>(_get_osfhandle(_desc.get())); }

bool cursor_position(io::fdesc _target, graph::vec2<int> _position)
{
	return	SetConsoleCursorPosition(GoHandle(_target), { (SHORT)_position.x, (SHORT)_position.y }) ? true : false;
}


graph::vec2<int> cursor_position(io::fdesc _target)
{
	CONSOLE_SCREEN_BUFFER_INFO	info;
	GetConsoleScreenBufferInfo(GoHandle(_target), &info);
	return	 { (int)info.dwCursorPosition.X, (int)info.dwCursorPosition.Y };
}


graph::size2<int> terminal_size(io::fdesc _target)
{
	CONSOLE_SCREEN_BUFFER_INFO	info;
	GetConsoleScreenBufferInfo(GoHandle(_target), &info);
	return	 { (int)info.dwSize.X, (int)info.dwSize.Y };
}



terminal::terminal(io::fdesc _output, io::fdesc _input)
	: mIn(_input)
	, mOut(_output)
{
	DWORD	dwMode;
	GetConsoleMode(GoHandle(mOut), &dwMode);
	gDefaultConsoleMode	= dwMode;

	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(GoHandle(mOut), dwMode);
}

terminal::~terminal()
{
}

auto terminal::getkey() -> keyresult
{
	HANDLE	hStdin	= GoHandle(mIn);
	while (!is_done())
	{
		if (WaitForSingleObject(hStdin, 100) != WAIT_OBJECT_0)
		{
			continue;
        }
		DWORD			dummy;
		INPUT_RECORD	irec;
		ReadConsoleInput(hStdin, &irec, 1, &dummy);
		switch (irec.EventType)
		{
			case KEY_EVENT :
			{
				auto&	ke	= irec.Event.KeyEvent;
				if (!ke.bKeyDown)
                {
					break;
				}
				int	rep	= ke.wRepeatCount ? (int)ke.wRepeatCount : 1;
				if (irec.Event.KeyEvent.dwControlKeyState & ENHANCED_KEY)
				{
					switch (ke.wVirtualKeyCode)
					{
						case VK_HOME:	return	{ ck_home, rep };
						case VK_END:	return	{ ck_end, rep };
						case VK_DELETE:	return	{ ck_delete, rep };
						case VK_LEFT:	return	{ ck_left, rep };
						case VK_RIGHT:	return	{ ck_right, rep };
						case VK_UP:		return	{ ck_up, rep };
						case VK_DOWN:	return	{ ck_down, rep };
						default:		return	{ 0, rep };
					}
				}
				if (!ke.uChar.AsciiChar)
				{
					return	{-1, 0};
				}
				switch (ke.uChar.AsciiChar)
				{
					case VK_BACK:	return	{ ck_backspace, rep };
					default:		return	{ ke.uChar.AsciiChar & 0xff, rep };
				}
			}
			case MOUSE_EVENT :
			{
				break;
			}
		}
	}
	return	{ -1, 0 };
}

void terminal::reset()
{
	DWORD	written	= 0;
	auto*	cmd		= L"\x1b[2J";
	WriteConsoleW(GoHandle(mOut), cmd, (DWORD)wcslen(cmd), &written, NULL);
}


AMTRS_CONSOLE_NAMESPACE_END

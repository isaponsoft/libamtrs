/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/console/escape.hpp>
#include <amtrs/console/terminal.hpp>
#include <amtrs/string/numeric.hpp>
AMTRS_CONSOLE_NAMESPACE_BEGIN


bool cursor_move(io::fdesc _target, graph::vec2<int> _move)
{
	if ((_move.x < 0) && !cursor_move_left(_target, -_move.x))
	{
		return	false;
	}
	if ((_move.x > 0) && !cursor_move_right(_target, _move.x))
	{
		return	false;
	}
	if ((_move.y < 0) && !cursor_move_up(_target, -_move.y))
	{
		return	false;
	}
	if ((_move.y > 0) && !cursor_move_down(_target, _move.y))
	{
		return	false;
	}
	return	true;
}


bool cursor_move_up(io::fdesc _target, int _count)
{
	return	write_escape_command(_target, _count, "A");
}


bool cursor_move_down(io::fdesc _target, int _count)
{
	return	write_escape_command(_target, _count, "B");
}


bool cursor_move_left(io::fdesc _target, int _count)
{
	return	write_escape_command(_target, _count, "D");
}


bool cursor_move_right(io::fdesc _target, int _count)
{
	return	write_escape_command(_target, _count, "C");
}


bool write_escape_command(io::fdesc _target, text _txt)
{
	char	buff[2] = { '\033', '[' };
	if (_target.write(&buff[0], 2) != 2)
	{
		return	false;
	}
	return	_target.write(_txt.data(), (int)_txt.size()) == _txt.size();
}

bool write_escape_command(io::fdesc _target, int _num, text _txt)
{
	char	buff[30] = { '\033', '[' };
	if (_target.write(&buff[0], 2) != 2)
	{
		return	false;
	}
	size_t	len		= strutil::write_int(buff, sizeof(buff), _num);
	if (_target.write(&buff[0], (int)len) != len)
	{
		return	false;
	}
	return	_target.write(_txt.data(), (int)_txt.size()) == _txt.size();
}


AMTRS_CONSOLE_NAMESPACE_END

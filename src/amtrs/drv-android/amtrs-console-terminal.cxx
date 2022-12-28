/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <unistd.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <termios.h>
#include <amtrs/console/terminal.hpp>
AMTRS_CONSOLE_NAMESPACE_BEGIN

struct termios gDefaultConf;

static size_t write_int(char* buff, size_t size, int n);

bool cursor_position(io::fdesc _target, graph::vec2<int> _position)
{
	char	buff[20];
	int		i = 0;
	buff[i++]	= '\033';
	buff[i++]	= '[';
	i += write_int(buff+i, sizeof(buff)-i, _position.y);
	buff[i++]	= ';';
	i += write_int(buff+i, sizeof(buff)-i, _position.x);
	buff[i++]	= 'H';
	auto	r	= write(_target.get(), buff, i);
	return	r == i;
}


graph::vec2<int> cursor_position(io::fdesc _target)
{
	struct termios		term;
	struct termios		restore;
	graph::vec2<int>	ret	= { 0, 0 };

	tcgetattr(0, &term);
	tcgetattr(0, &restore);
	term.c_lflag &= ~(ICANON|ECHO);
	tcsetattr(0, TCSANOW, &term);

	auto		r	= write(_target.get(), "\033[6n", 4);

	char		buf[20]	= {0};
	int			i		= 0;
	while (i < sizeof(buf))
	{
		char	c;
		if (!read(0, &c, 1))
		{
		 	break;
		}
		buf[i++] = c;
		if (c == 'R')
		{
			break;
		}
	}

	int	x	= 0;
	int	y	= 0;

	i -= 2;
	for (int dig = 1; i > 0; --i, dig *= 10)
	{
		if (buf[i] == ';')
		{
			ret.x	= x;
			break;
		}
		x	= x + (buf[i] - '0') * dig;
	}

	i -= 1;
	for (int dig = 1; i > 0; --i, dig *= 10)
	{
		if (buf[i] == '[')
		{
			ret.y	= y;
			break;
		}
		y	= y + (buf[i] - '0') * dig;
	}

	tcsetattr(0, TCSANOW, &restore);
	return	ret;
}


graph::size2<int> terminal_size(io::fdesc _target)
{
	struct winsize w;
	ioctl(_target.get(), TIOCGWINSZ, &w);
	return	{ w.ws_col, w.ws_row };
}

terminal::terminal(io::fdesc _output, io::fdesc _input)
	: mIn(_input)
	, mOut(_output)
{
 	tcgetattr(mIn.get(), &gDefaultConf);
}

terminal::~terminal()
{
	tcsetattr(mIn.get(), 0, &gDefaultConf);
}

void terminal::reset()
{
	tcsetattr(mIn.get(), 0, &gDefaultConf);
}


// このあたりがくわしい
//	https://docs.microsoft.com/ja-jp/windows/console/console-virtual-terminal-sequences
auto terminal::getkey() -> keyresult
{
	struct termios term;
 	tcgetattr(mIn.get(), &term);
	term.c_iflag |= IGNBRK;
	term.c_iflag &= ~(INLCR | ICRNL | IXON | IXOFF);
	term.c_lflag &= ~(ICANON | ECHO | ECHOK | ECHOE | ECHONL | ISIG | IEXTEN);
//	term.c_lflag &= ~(BRKINT|ISTRIP|IXON);
//	term.c_lflag &= ~(ECHO|ICANON|IEXTEN);
	tcsetattr(mIn.get(), 0, &term);
	keyresult	res	= [this]() -> keyresult
	{
		auto	_self_getchar	= [&]() -> unsigned char
		{
			while (mPendingIn >= mPending.size() && !is_done())
			{
				struct timeval	tv;
				tv.tv_sec	= 0;
				tv.tv_usec	= 10*1000;
				fd_set			fds;
				FD_ZERO(&fds);
				FD_SET(mIn.get(), &fds);
				int	ret	= select(mIn.get() + 1, &fds, NULL, NULL, &tv);
				if (!FD_ISSET(mIn.get(), &fds))
				{
					continue;
				}

				mPendingIn	= BUFSIZ;
				mPending.resize(BUFSIZ);
				int				r	= ::read(mIn.get(), mPending.data(), (int)mPending.size());
				if (r <= 0)
				{
					continue;
				}
				mPending.resize(r);
				mPendingIn	= 0;
			}
			if (is_done())
			{
				return	0;
			}
			return	mPending[mPendingIn++];
		};


		while (!is_done())
		{
			unsigned char	c	= _self_getchar();
			if (c == 0x03)		// Ctrl+C
			{
				return	{ ck_sigint, 1 };
			}
			if (c == 0x1b)		// ESC
			{
				unsigned char	group	= _self_getchar();
				// cursor
				if (group == '[')
				{
					int	num	= 0;
					for (;;)
					{
						c	= _self_getchar();
						if (c >= '0' && c <= '9')
						{
							num	= num*10 + (c - '0');
							continue;
						}
						if (c == 'A') { return { ck_up,    num }; }
						if (c == 'B') { return { ck_down,  num }; }
						if (c == 'C') { return { ck_right, num }; }
						if (c == 'D') { return { ck_left,  num }; }
						if (c == 'E') { return { ck_nextline, num }; }
						if (c == 'F') { return { ck_backline, num }; }
						if (c == 'G') { return { ck_setx,     num }; }
						if (c == 'd') { return { ck_sety,     num }; }
						if (c == '~')
						{
							if (num == 2) { return { ck_insert,   1 }; }
							if (num == 3) { return { ck_delete,   1 }; }
							if (num == 5) { return { ck_pageup,   1 }; }
							if (num == 6) { return { ck_pagedown, 1 }; }

							if (num == 15) { return { fk_f5,  1 }; }
							if (num == 17) { return { fk_f6,  1 }; }
							if (num == 18) { return { fk_f7,  1 }; }
							if (num == 19) { return { fk_f8,  1 }; }
							if (num == 20) { return { fk_f9,  1 }; }
							if (num == 21) { return { fk_f10, 1 }; }
							if (num == 23) { return { fk_f11, 1 }; }
							if (num == 24) { return { fk_f12, 1 }; }
						}
					}

				}
				// F1 - F4
				if (group == 'O')
				{
				}

				// ctrl code.
				unsigned char	buf[20] = {0};
				int				len		= 0;
				for (;;)
				{
					if (is_done()) { return { -1, 0 }; }
					buf[len++] = _self_getchar();
					if (len >= 1 && buf[0] != '[')
					{
						return	{buf[0], 1};
					}

					// check cntrl code.
					if (len == 2 && buf[0] == '[' && buf[1] == 'A') { return { ck_up,    1 }; }
					if (len == 2 && buf[0] == '[' && buf[1] == 'B') { return { ck_down,  1 }; }
					if (len == 2 && buf[0] == '[' && buf[1] == 'C') { return { ck_right, 1 }; }
					if (len == 2 && buf[0] == '[' && buf[1] == 'D') { return { ck_left,  1 }; }
					if (len == 3 && buf[0] == '[' && buf[1] == '3' && buf[2] == '~') { return { ck_delete,    1 }; }

					if (len == sizeof(buf))
					{
						return	{-1, 0};
					}
				}
			}
			else if (c == 0x7f)
			{
				return	{ ck_backspace, 1 };
			}
			return	{(unsigned char)c, 1};
		}
		return	{-1, 0};
	}();
	tcsetattr(mIn.get(), 0, &gDefaultConf);
	return	res;
}

static size_t write_int(char* buff, size_t size, int n)
{
	size_t	len	= 0;
	int		t	= n;
	do
	{
		len += 1;
		t   /= 10;
	} while (t > 0);
	if (size >= len)
	{
		buff	= buff + len;
		t		= n;
		do
		{
			--buff;
			*buff	= '0' + (t % 10);
			t   /= 10;
		} while (t > 0);
	}
	return	len;
}

AMTRS_CONSOLE_NAMESPACE_END

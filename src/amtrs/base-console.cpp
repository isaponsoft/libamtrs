/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/console.hpp>
#include <amtrs/chrono.hpp>
#include <iomanip>
#include <sstream>
AMTRS_CONSOLE_NAMESPACE_BEGIN

// 0 : none
// 1 : progress bar mode
static	int				sMode	= 0;

static	struct
		{
			progresstype	type;
			std::string		reson;
			size_t			start;
			size_t			end;
			size_t			current;
			size_t			band;
			std::string		details;
			std::string		cache;
			std::time_t		startTime;
			std::time_t		lastTime;
		}				sProgress;


static void finish_current_mode();
static void progress_draw(bool _force);


void progress_start(progresstype _type, size_t _start,  size_t _end, char const* _reson)
{
	finish_current_mode();
	sMode				= 1;
	sProgress.type		= _type;
	sProgress.reson		= _reson;
	sProgress.start		= _start;
	sProgress.current	= _start;
	sProgress.end		= _end;
	sProgress.details.clear();
	sProgress.band		= 0;
	sProgress.startTime	= time(nullptr);
	sProgress.lastTime	= 0;
	sProgress.cache.clear();
	if (sProgress.reson.size() > 30)
	{
		sProgress.reson	= sProgress.reson.substr(0, 30);
	}
	progress_draw(true);
}

void progress_maxsize(size_t _end)
{
	if (sMode != 1)
	{
		return;
	}
	sProgress.end		= _end;
}

void progress_inc(size_t _inc)
{
	if (sMode != 1)
	{
		return;
	}
	sProgress.current += _inc;
	sProgress.band    += _inc;
	progress_draw(false);
}

void progress_inc_string(std::string_view _line, size_t _inc)
{
	if (sMode != 1)
	{
		return;
	}
	if (!_line.empty())
	{
		_line	= trim(_line, "\r\n\t ");
		if (auto pos = _line.find_first_of("\r\n"); pos != std::string_view::npos)
		{
			_line	= trim(_line.substr(0, pos), "\r\n\t ");
		}
		if (!_line.empty())
		{
			std::cout << _line.substr(0, 30) << std::flush;
		}
	}
	sProgress.details	= std::string(_line);
	sProgress.current += _inc;
	progress_draw(false);
}

void progress_end()
{
	if (sMode != 1)
	{
		return;
	}
	progress_draw(true);
	std::cout << std::flush << std::endl;
	sMode	= 0;
}


static std::string make_number(double _num)
{
	static constexpr char	units[]	= { ' ', 'K', 'M', 'G', 'T', 'P' };
	char					unit	= ' ';
	for (int i = 0; i < 6; ++i)
	{
		unit	= units[i];
		if (_num < 1000)
		{
			break;
		}
		_num	/= 1024;
	}
	std::stringstream	ss;
	ss <<  std::fixed << std::setprecision(2) << _num;

	std::stringstream	ss2;
	ss2 << std::right << std::setw(6) << ss.str() << unit; 
	return	ss2.str();
}

void progress_draw(bool _force)
{
	if (sMode != 1)
	{
		return;
	}

	int		barwith		= 30;
	auto	now			= time(nullptr);
	if (now - sProgress.lastTime == 0 && !_force)
	{
		return;
	}
	sProgress.lastTime	= now;

	auto	difftime	= now - sProgress.startTime;
	std::string	r;
	r.append(format<std::string>("%-30s ", sProgress.reson.c_str()));
	switch (sProgress.type)
	{
		case progresstype::extract :
			r.append("Extract  ");
			break;
		case progresstype::download :
			r.append("Download ");
			break;
		default :
			r.append("         ");
			break;
	}
	if (sProgress.start < sProgress.end)
	{
		if (sProgress.current > sProgress.end)
		{
			sProgress.current = sProgress.end;
		}
		int	persent	= static_cast<int>((sProgress.current - sProgress.start) * 100 / (sProgress.end - sProgress.start));
		int	bar		= persent * barwith / 100;
		r.append(format<std::string>("%3d%% ", persent));
		r.append("[");
		for (int i = 0; i < barwith; ++i)
		{
			r.append(((i < bar) ? "=" : " "));
		}
		r.append("] ");
	}
	else
	{
		r.append("     [");
		if (!_force)
		{
			for (int i = 0; i < barwith; ++i)
			{
				r.append((i == (difftime%30)) ? "?" : " ");
			}
		}
		else
		{
			for (int i = 0; i < barwith; ++i)
			{
				r.append("*");
			}
		}
		r.append("] ");
	}

	r.append(format<std::string>("%02d:%02d ", difftime/60, difftime%60));

	if (sProgress.type == progresstype::download)
	{
		r.append(make_number((double)sProgress.band));
		r.append("/s ");
		sProgress.band	= 0;

		r.append(make_number((double)sProgress.current));
		if (sProgress.start < sProgress.end)
		{
			r.append("/");
			r.append(make_number((double)sProgress.end));
			r.append(" ");
		}
		else
		{
			r.append("         ");
		}
	}

	if (!sProgress.details.empty())
	{
		r.append(sProgress.details);
	}

	if (sProgress.cache != r)
	{
		sProgress.cache = std::move(r);
		std::cout << "\r" << sProgress.cache << clear_line << std::flush;
	}

}


static void finish_current_mode()
{
	if (sMode == 1)
	{
		progress_end();
	}
}

AMTRS_CONSOLE_NAMESPACE_END

/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/filesystem.hpp>
#include <amtrs/system.hpp>
#include <amtrs/scriptutil.hpp>
AMTRS_SCRIPTUTIL_NAMESPACE_BEGIN



std::string array_shift(std::deque<std::string>& _array)
{
	std::string	c;
	if (!_array.empty())
	{
		c	= _array.front();
		_array.pop_front();
	}
	return	c;
}


update_result update(std::string_view _targetfile, std::string_view _text, bool _noupdate)
{
	if (filesystem::exists(_targetfile))
	{
		auto	now	= amtrs::filesystem::file_get_contents<std::string>(_targetfile);
		if (now == _text)
		{
			return	update_skip;
		}
	}
	if (_noupdate)
	{
		return	update_modify;
	}

	if (auto dir = amtrs::filesystem::parent_path(_targetfile); !dir.empty())
	{
		std::filesystem::create_directories(dir);
	}
	return	amtrs::filesystem::file_put_contents(_targetfile, _text)
			? update_modify
			: update_err;
}


std::string add_exe(std::string_view _filename)
{
#if	AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_WIN32
	return	std::string(_filename) + ".exe";
#else
	return	std::string(_filename);
#endif
}


std::string slashdir(std::string_view _filename)
{
	return	regex_replace_callback<std::string>(std::string(_filename), R"(\\)", [&](auto&)
	{
		return	"/";
	});
}


std::string backslashdir(std::string_view _filename, bool _escape)
{
	return	regex_replace_callback<std::string>(std::string(_filename), "/", [&](auto&)
	{
		return	_escape ? "\\\\" : "\\";
	});
}


int exec(std::string& _out, std::string& _err, std::string const& _command)
{
	try
	{
		return	system::exec(_command.c_str(), [&](auto type, auto* data, auto* size) -> int
		{
			if (!*data)
			{
				std::string&	str(type ==  system::streamtype::cout ? _out : _err);
				auto	s	= str.size();
				str.resize(s + *size);
				*data	= str.data() + s;
			}
			return	0;
		});
	}
	catch (...)
	{
		return	-1;
	}
}


int exec(std::string& _out, std::string const& _command)
{
	try
	{
		return	system::exec(_command.c_str(), [&](auto type, auto* data, auto* size) -> int
		{
			if (!*data)
			{
				if (type == system::streamtype::cout)
				{
					auto	s	= _out.size();
					_out.resize(s + *size);
					*data	= _out.data() + s;
				}
				if (type ==  system::streamtype::cerr)
				{
					std::cerr << std::string_view((char*)*data, *size);
				}
			}
			return	0;
		});
	}
	catch (...)
	{
		return	-1;
	}
}


int exec(std::string const& _command)
{
	try
	{
		return	system::exec(_command.c_str());
	}
	catch (...)
	{
		return	-1;
	}
}


AMTRS_SCRIPTUTIL_NAMESPACE_END

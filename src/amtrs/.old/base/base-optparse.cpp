/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/filesystem.hpp>
#include <amtrs/optparse.hpp>
AMTRS_NAMESPACE_BEGIN

bool readopt(std::string_view& _ret, int& _argc, char**& _argv)
{
	if (_argc == 0)
	{
		return	false;
	}
	_ret	= std::string_view(_argv[0]);
	--_argc;
	++_argv;
	return	true;
}


bool readopt(std::string_view& _ret, int& _argc, optstr_type*& _argv)
{
	if (_argc == 0)
	{
		return	false;
	}
	_ret	= std::string_view(_argv[0]);
	--_argc;
	++_argv;
	return	true;
}



bool optparse(optstate& out, optstr_type*& argv, size_t& argc, optinfo const* opts, size_t optsn)
{
	if (argc == 0)
	{
		return	false;
	}
	auto	cmd	= std::string_view(*argv);
	if (starts_with(cmd, "-"))
	{
		for (auto i = 0; i < optsn; ++i)
		{
			bool	match	= false;
			if (!match && opts[i].longname && (starts_with(cmd, "--")))
			{
				match	= (opts[i].flags & (unsigned int)opttype::starts_with)
						? starts_with(cmd.substr(2), opts[i].longname)
						: cmd.substr(2) == opts[i].longname;
			}
			if (!match && starts_with(cmd, "-" ) && (cmd.size() >= 2 && cmd[1] == opts[i].shortval))
			{
				match	= (opts[i].flags & (unsigned int)opttype::starts_with)
						? true
						: cmd.size() == 2;
			}
			if (match)
			{
				out.shortval	= opts[i].shortval;
				out.longmode	= starts_with(cmd, "--") ? true : false;
				out.opt			= cmd.data();
				out.arg.clear();
				if (opts[i].flags & (unsigned int)opttype::starts_with)
				{
					out.arg		= starts_with(cmd, "--")
								? cmd.substr(2 + strlen(opts[i].longname))
								: cmd.substr(2);
				}
				else if (opts[i].flags & (unsigned int)opttype::required)
				{
					if (argc < 2)
					{
						out.shortval	= (char)opterr::none_argumrnt;
						return	true;
					}
					out.arg		= argv[1];
					++argv;
					--argc;
				}
				++argv;
				--argc;
				return	true;
			}
		}
	}
	return	false;
}


bool optparse(optstate& _out, char**& _argv, int& _argc, optinfo const* _opts, size_t _optsn)
{
	optstr_type*	v	= reinterpret_cast<optstr_type*>(reinterpret_cast<void*>(_argv));
	size_t			s	= static_cast<size_t>(_argc);
	bool			r	= optparse(_out, v, s, _opts, _optsn);
	if (r)
	{
		_argv	= reinterpret_cast<char**>(reinterpret_cast<void*>(v));
		_argc	= static_cast<int>(s);
	}
	return	r;
}



std::string usage_short(optinfo const* _opts, size_t _optsn, std::string_view _command, std::string_view _arguments)
{
	std::string	t;
	t += "usage: ";
	t += filesystem::filename(_command);
	t += " ";
	size_t	linelen	= t.size();
	for (auto i = 0; i < _optsn; ++i)
	{
		auto&	opt(_opts[i]);
		std::string	line;
		line += "[-";
		line += opt.shortval;
		if (opt.longname)
		{
			line += ",--";
			line += opt.longname;
		}
		line += "] ";
		if (linelen + line.size() >= 79)
		{
			t 		+= "\n        ";
			linelen	= 8;
		}
		t.append(line);
		linelen	+= line.size();
	}
	if (linelen + _arguments.size() >= 79)
	{
		t 	+= "\n       ";
	}
	t += _arguments;
	return	t;
}

std::string usage_long(optinfo const* _opts, size_t _optsn, std::string_view _command, std::string_view _arguments, std::string_view _note)
{
	std::string	t;
	if (!_command.empty())
	{
		t += "Usage: ";
		t += filesystem::filename(_command);
		t += " [OPTIONS] ";
		t += _arguments;
		t += '\n';
	}
	if (!_note.empty())
	{
		t += _note;
		t += '\n';
	}

	for (auto i = 0; i < _optsn; ++i)
	{
		auto&	opt(_opts[i]);
		std::string	line;
		line += "  -";
		line += opt.shortval;
		if (opt.longname)
		{
			line += ", --";
			line += opt.longname;
		}
		if (line.size() > 25)
		{
			line += "\n                          ";
		}
		for (size_t i = line.size(); i < 26; ++i)
		{
			line += ' ';
		}
		line += opt.help;
		line += '\n';
		t.append(line);
	}
	return	t;
}
AMTRS_NAMESPACE_END

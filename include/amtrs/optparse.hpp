/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__optparse__hpp
#define	__libamtrs__optparse__hpp
#include "string.hpp"
AMTRS_NAMESPACE_BEGIN

enum class	opttype
{
	none		= 0,		//!< Have not arg.
	required	= 1,		//!< Requirement arg.
};

enum class	opterr
{
	eoa				= 0,
	unknown_err		= 1,
	none_argumrnt	= 2,
};

struct	optinfo
{
	char 		shortval;
	char const*	longname;
	opttype		has_args;
	char const*	help;
};

struct	optstate
{
	char 		shortval;
	bool		longmode;
	std::string	opt;
	std::string	arg;
};

using	optstr_type	= char const*;

std::string usage_short(optinfo const* _opts, size_t _optsn, std::string_view _command, std::string_view _arguments);

template<size_t N>
std::string usage_short(optinfo const (&_opts)[N], std::string_view _command, std::string_view _arguments)
{
	return	usage_short(&_opts[0], N, _command, _arguments);
}


std::string usage_long(optinfo const* _opts, size_t _optsn, std::string_view _command, std::string_view _arguments, std::string_view _note);


template<size_t N>
std::string usage_long(optinfo const (&_opts)[N], std::string_view _command, std::string_view _arguments, std::string_view _note)
{
	return	usage_long(&_opts[0], N, _command, _arguments, _note);
}


bool optparse(optstate& out, optstr_type*& argv, size_t& argc, optinfo const* opts, size_t optsn);

bool optparse(optstate& _out, char**& _argv, int& _argc, optinfo const* _opts, size_t _optsn);


template<class Argv, class Argc, size_t N>
bool optparse(optstate& _out, Argv& _argv, Argc& _argc, optinfo const (&_opts)[N])
{
	return	optparse(_out, _argv, _argc, &_opts[0], N);
}


template<class Arg>
bool optparse(optstate& _out, Arg& _argv, optinfo const* _opts, size_t _optsn)
{
	size_t						argc(_argv.size());
	std::vector<optstr_type>	args;
	for (auto& a : _argv)
	{
		args.push_back(a.data());
	}
	optstr_type*				argv	= args.data();
	auto	r	= optparse(_out, argv, argc, _opts, _optsn);
	if (r)
	{
		size_t	remove	= _argv.size() - argc;
		while (remove > 0)
		{
			_argv.pop_front();
			--remove;
		}
	}
	return	r;
}



template<class Arg, size_t N>
bool optparse(optstate& _out, Arg& _argv, optinfo const (&_opts)[N])
{
	return	optparse(_out, _argv, &_opts[0], N);
}



template<class Arg, size_t N>
optstate optparse(Arg& _argv, optinfo const (&_opts)[N])
{
	optstate	r;
	if (!optparse(r, _argv, &_opts[0], N))
	{
		r.shortval	= (char)opterr::eoa;
	}
	return	r;
}



AMTRS_NAMESPACE_END
#endif

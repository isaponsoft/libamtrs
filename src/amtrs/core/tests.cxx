/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/tests.hpp>
#include <amtrs/string/wildcard.hpp>
#include <chrono>
#include <iomanip>
#include <locale>
#include <sstream>
#include <string.h>
#include <vector>

AMTRS_TEST_NAMESPACE_BEGIN
bool		failed;
bool		oklog;
char const*	selfpath;

static	std::string		filter;
static	bool			stop	= false;
static	std::ostream*	msg;

std::ostream& teststream()
{
	if (oklog)
	{
		return	std::cout;
	}
	return	*msg;
}



int tests(int _argc, char** _args)
{
	std::locale loc(std::locale(), new std::ctype<char>);
	selfpath	= _args[0];

	for (int i = 1; i < _argc;)
	{
		std::string	cmd(_args[i++]);
		if (cmd == "--help" || cmd == "-h")
		{
			std::cerr << "usage)" << _args[0] << std::endl;
			std::cerr << " --filter wildcard patturn." << std::endl;
			std::cerr << " --stop   if NG stop." << std::endl;
			return	0;
		}
		else if (cmd == "--filter")
		{
			if (i == _argc)
			{
				std::cerr << "--filter has not wildcard patturn" << std::endl;
				return	1;
			}
			filter	= _args[i++];
		}
		else if (cmd == "--oklog")
		{
			oklog	= true;
		}
		else if (cmd == "--stop")
		{
			stop	= true;
		}
	}

	std::stringstream	msgs;
	msg	= &msgs;

	size_t						testcount	= 0;
	size_t						maxlen		= 0;
	std::vector<test_entry*>	entries;
	for (auto* test = test_entry::first; test; test = test->next)
	{
		if (!filter.empty() && !wcmatch(text(filter), test->name))
		{
			continue;
		}
		entries.push_back(test);
		maxlen	= std::max<size_t>(::strlen(test->name), maxlen);
		++testcount;
	}

	std::sort(entries.begin(), entries.end(), [](auto* a, auto* b)
	{
		return	std::string_view(a->name) < std::string_view(b->name);
	});
	
	size_t		passcount	= 0;
	size_t		faiilecount	= 0;
	for (auto* test : entries)
	{
		AMTRS_TEST_NAMESPACE::failed	= false;
		msgs.str("");
		msgs.clear(std::stringstream::goodbit);

		bool	pass	= false;
		double	time	= 0;

		std::cout << "[TEST] " << test->name;
		for (auto space = maxlen - ::strlen(test->name); space > 0; --space)
		{
			std::cout << ' ';
		}
		std::cout << " ... " << std::flush;

		try
		{
			auto	start	= std::chrono::system_clock::now();
			test->func();
			auto	end		= std::chrono::system_clock::now();
			if (!failed)
			{
				pass	= true;
				time	= static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count());
			}
		}
		catch (std::exception& e)
		{
			msgs << e.what();
		}
		catch (...)
		{
			msgs << "exception!!";
		}

		if (!pass)
		{
			std::cout << "[TEST] " << test->name;
			for (auto space = maxlen - ::strlen(test->name); space > 0; --space)
			{
				std::cout << ' ';
			}
			std::cout << " ... ";
		}
		if (pass)
		{
			std::cout << "OK " << std::fixed << std::setprecision(2) << (time/1000.0) << " sec" << std::endl;
			++passcount;
		}
		else
		{
			std::cout << "NG " << msgs.str() << std::endl;
			++faiilecount;
			if (stop)
			{
				return	1;
			}
		}
	}
	if (faiilecount == 0)
	{
		std::cout << "all pass." << std::endl;
		return	0;
	}
	else
	{
		std::cout << faiilecount << " miss." << std::endl;
		return	1;
	}
}
AMTRS_TEST_NAMESPACE_END

/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <stdio.h>
#include <amtrs/console/arguments.hpp>
PSNVM_SLSH_NAMESPACE_BEGIN
using namespace AMTRS_STRING_NAMESPACE;
using namespace AMTRS_SOURCECODE_NAMESPACE;


int f_history(sstring& e)
{
	simplearray<sstring>	args	= amtrs::console::parse_command_args(e);
	if (args.size() >= 1 && args[0] == "-d")
	{
		if (args.size() < 2)
		{
			io::fdesc::stdo() << "history -d has not line number\n";
			return	1;
		}
		auto	n	= read_numeric_literal(text(args[1]));
		if (n.empty())
		{
			io::fdesc::stdo() << args[1] << " is not number\n";
			return	1;
		}
		bool	s;
		auto	ln	= to_intmax(s, n.code);
		curent_editor()->remove_history((int)ln);
		return	0;
	}


	int	index	= 0;
	for (auto& h : curent_editor()->history())
	{
		char	buff[100];
		snprintf(buff, sizeof(buff), "%5d  ", index);
		io::fdesc::stdo() << buff << h << "\n";
		++index;
	}
	return	0;
}

PSNVM_SLSH_NAMESPACE_END

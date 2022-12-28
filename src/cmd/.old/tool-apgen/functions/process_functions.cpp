/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <iostream>
#include "../apgen.hpp"
#include "functions.hpp"

namespace apgen {

bool process_function(status& _status, std::string& line)
{
	bool	proc	= false;
	amtrs::regex_match_callback(line, R"(@([a-zA-Z_]+)\((.*)\))", [&](auto m)
	{
		std::string	func	= m[1];
		std::string	args	= m[2];
		auto*		fp		= function_generator::find(func);
		if (fp)
		{
			auto		pos		= m.position();
			auto		len		= m.length();
			std::string	fline	= line.substr(pos, len);

			function_params	params;
			params.arguments	= args;
			(*fp)(_status, fline, params);


			std::string	retval;

			retval	+= line.substr(0, pos);
			retval	+= fline;
			retval	+= line.substr(pos + len);

			line	= retval;
			proc	= true;
		}
	});

	return	proc;
}


}

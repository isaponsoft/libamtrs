/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <regex>
#include "../apgen.hpp"
#include "functions.hpp"

namespace apgen {


function_generator	_message("message", [](status& _status, std::string& line, function_params const& params) -> bool
{
	report(params.arguments);
	line	= "";
	return	true;
});



}

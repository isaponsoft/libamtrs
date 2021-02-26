/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/filesystem/@>
#include <passion/sourcecode.hpp>
PASSION_NAMESPACE_BEGIN


vmsourcecode vmsourcecode::load(std::string const& _filename)
{
	vmsourcecode	retval;
	retval.filename	= _filename;
	retval.source	= vmstring(amtrs::filesystem::file_get_contents<std::string>(_filename));
	return	retval;
}


PASSION_NAMESPACE_END

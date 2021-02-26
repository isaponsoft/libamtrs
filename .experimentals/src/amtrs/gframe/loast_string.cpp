/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/filesystem/@>
#include <amtrs/utility/@>
#include <gframe/load_string.hpp>
GFRAME_NAMESPACE_BEGIN
using namespace amtrs;



std::string load_string(std::string const& _filename)
{
	std::string	retval;

	auto		fin			= filesystem::fileloader::get_instance()->open(_filename);
	auto		in			= ios::make_rstream(fin);
	auto		inSize		= in.seekg(0, std::ios_base::end).tellg();
	in.seekg(0, std::ios_base::beg);

	retval.resize(std::size_t(inSize));
	in.read(retval.data(), retval.size());

	return	retval;
}


std::vector<char> load_byte(std::string const& _filename)
{
	std::vector<char>	retval;

	auto		fin			= filesystem::fileloader::get_instance()->open(_filename);
	auto		in			= ios::make_rstream(fin);
	auto		inSize		= in.seekg(0, std::ios_base::end).tellg();
	in.seekg(0, std::ios_base::beg);

	retval.resize(std::size_t(inSize));
	in.read(retval.data(), retval.size());

	return	retval;
}


GFRAME_NAMESPACE_END

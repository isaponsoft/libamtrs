/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <iostream>
#include <string>
#include <string_view>
#include <amtrs/filesystem.hpp>
#include <amtrs/net.hpp>
#include <amtrs/string.hpp>
#include "apgen.hpp"



std::string apgen::unzip(void const* _buff, size_t _size, std::string const& _dir)
{
	using namespace amtrs;
	using namespace amtrs::net;

	auto	in	= make_view((char*)_buff, _size);
	auto	z	= filesystem::zip::open(io::make_stream_in(in));

	std::string	retval;

	auto	fst	= z.begin();
	if (fst != z.end())
	{
		if (ends_with(std::string_view(fst->filename().data()), std::string_view("/")))
		{
			retval	= fst->filename().data();
		}
	}

	for (auto e : z)
	{
		auto	n	= std::string(e.filename().data());
		auto	i	= z.open(e);

		if (ends_with(std::string_view(n), std::string_view("/")))
		{
			filesystem::create_directory(filesystem::parent_path((std::string_view)n));
		}

		std::cout << "Extract: " << n << std::endl;

		std::ofstream	o(n, std::ios::trunc|std::ios::binary);
		auto			zin	= amtrs::io::inflate_stream_in<decltype(i), decltype(i)&>(i);
		amtrs::io::copy_to_eof(o, zin);
	}


	return	retval;
}

std::string apgen::unzip(amtrs::io::vstreamif& _in)
{
	using namespace amtrs;
	using namespace amtrs::net;

	auto	z	= filesystem::zip::open(_in);

	std::string	retval;

	auto	fst	= z.begin();
	if (fst != z.end())
	{
		if (ends_with(std::string_view(fst->filename().data()), std::string_view("/")))
		{
			retval	= fst->filename().data();
		}
	}

	for (auto e : z)
	{
		auto	n	= std::string(e.filename().data());
		auto	i	= z.open(e);

		if (ends_with(std::string_view(n), std::string_view("/")))
		{
			filesystem::create_directory(filesystem::parent_path((std::string_view)n));
		}

		std::cout << "Extract: " << n << std::endl;

		std::ofstream	o(n, std::ios::trunc|std::ios::binary);
		auto			zin	= amtrs::io::inflate_stream_in<decltype(i), decltype(i)&>(i);
		amtrs::io::copy_to_eof(o, zin);
	}


	return	retval;
}

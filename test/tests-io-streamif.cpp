/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <cstring>
#include <string.h>
#include <amtrs/io.hpp>
#include <amtrs/tests.hpp>
#include <fstream>
using namespace amtrs;

#define	_PRFX(_name)	io_streamif_##_name


// setup io_test!!
AMTRS_TEST_F(_PRFX(0_setup))
{
	std::ofstream	o("test.txt");
	if (!o.good())
	{
		std::cerr << "Can't create \"test.txt\". This is need a tests." << std::endl;
		return;
	}
	o << "Hello world.";
}


AMTRS_TEST_F(_PRFX(fisteam_2_move))
{
	std::ifstream	f("test.txt");
	auto			i	= io::make_streamif(std::move(f));
	char			buf[100];
	auto			r	= io::read(buf, i);
	AMTRS_TEST_EQ("Hello world.", std::string_view(buf, r));
}

AMTRS_TEST_F(_PRFX(fisteam_2_ptr))
{
	std::ifstream	f("test.txt");
	auto			i	= io::make_streamif(&f);
	char			buf[100];
	auto			r	= io::read(buf, i);
	AMTRS_TEST_EQ("Hello world.", std::string_view(buf, r));
}

AMTRS_TEST_F(_PRFX(stdlib_1))
{
	FILE*			f	= fopen("test.txt", "rb");
	auto			i	= io::make_streamif(f);
	char			buf[100];
	auto			r	= io::read(buf, i);
	AMTRS_TEST_EQ("Hello world.", std::string_view(buf, r));
}

AMTRS_TEST_F(_PRFX(stringview_1))
{
	auto			i	= io::make_streamif(std::string_view("Test"));
	char			buf[100];
	auto			r	= io::read(buf, i);
	AMTRS_TEST_EQ("Test", std::string_view(buf, r));
}

AMTRS_TEST_F(_PRFX(stringview_2))
{
	auto			i	= io::make_streamif(std::string_view("Test"));
	auto			i2	= io::make_streamif(std::move(i));
	static_assert(std::is_same<decltype(i), decltype(i2)>::value);
	char			buf[100];
	auto			r	= io::read(buf, i2);
	AMTRS_TEST_EQ("Test", std::string_view(buf, r));
}

AMTRS_TEST_F(_PRFX(vstreamif_1))
{
	auto			i	= io::make_streamif(std::string_view("Test"));
	auto			i2	= io::make_vstreamif(std::move(i));
	char			buf[100];
	auto			r	= io::read(buf, i2);
	AMTRS_TEST_EQ("Test", std::string_view(buf, r));
}

AMTRS_TEST_F(_PRFX(vstreamif_2))
{
	auto			i	= io::make_vstreamif(io::make_streamif(std::string_view("Test")));
	char			buf[100];
	auto			r	= io::read(buf, i);
	AMTRS_TEST_EQ("Test", std::string_view(buf, r));
}

AMTRS_TEST_F(_PRFX(vstreamif_3))
{
	auto			i	= io::make_vstreamif(io::make_streamif(std::string_view("Test")));
	auto			i2	= io::make_vstreamif(std::move(i));
	char			buf[100];
	auto			r	= io::read(buf, i2);
	AMTRS_TEST_EQ("Test", std::string_view(buf, r));
}

AMTRS_TEST_F(_PRFX(vstreamif_4))
{
	auto			i	= io::make_vstreamif(io::make_streamif(std::string_view("Test")));
	auto			i2	= io::make_vstreamif(std::move(i));
	char			buf[100];
	auto			r	= io::read(buf, i2);
	AMTRS_TEST_EQ("Test", std::string_view(buf, r));
}


AMTRS_TEST_F(_PRFX(stringview))
{
	char			buf[1];
	auto			i	= io::make_streamif(std::string_view("abcdefghijklmn"));

	AMTRS_TEST_EQ(14, io::size(i))

	io::seek(i, -1, std::ios::end);
	AMTRS_TEST_EQ(1, io::read(buf, i, 1));
	AMTRS_TEST_EQ(std::string_view("n"), std::string_view(buf, sizeof(buf)));
	AMTRS_TEST_TRUE(i.eof());

	i.clear();
	AMTRS_TEST_TRUE(i.good());
	AMTRS_TEST_FALSE(i.eof());

	io::seek(i,  1, std::ios::beg);
	AMTRS_TEST_EQ(1, io::read(buf, i, 1));
	AMTRS_TEST_EQ(std::string_view("b"), std::string_view(buf, sizeof(buf)));

	io::seek(i, 7, std::ios::beg);
	io::read(buf, i, 1);
	AMTRS_TEST_EQ(std::string_view("h"), std::string_view(buf, sizeof(buf)));

	io::seek(i, -3, std::ios::cur);
	io::read(buf, i, 1);
	AMTRS_TEST_EQ(std::string_view("f"), std::string_view(buf, sizeof(buf)));

	io::seek(i,  7, std::ios::cur);
	io::read(buf, i, 1);
	AMTRS_TEST_EQ(std::string_view("n"), std::string_view(buf, sizeof(buf)));
	AMTRS_TEST_TRUE(i.eof());
	i.clear();

	io::seek(i,  -100, std::ios::cur);
	io::read(buf, i, 1);
	AMTRS_TEST_EQ(std::string_view("a"), std::string_view(buf, sizeof(buf)));

	io::seek(i,   100, std::ios::cur);
	AMTRS_TEST_EQ(0, io::read(buf, i, 1));
	AMTRS_TEST_TRUE(i.eof());
	i.clear();


	char			buf4[4];
	io::seek(i, 0, std::ios::beg);
	AMTRS_TEST_EQ(4, io::read(buf4, i));
	AMTRS_TEST_EQ(std::string_view("abcd"), std::string_view(buf4, sizeof(buf4)));
}


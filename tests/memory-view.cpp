/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/tests.hpp>
#include <amtrs/memory/view.hpp>
#include <amtrs/string/text.hpp>
#include <amtrs/io/reader.hpp>
#include <amtrs/io/writer.hpp>
#include <amtrs/system/log.hpp>
using namespace amtrs;

#define	_PRFX(_name)	memory_view_##_name


AMTRS_TEST_F(_PRFX(1_iterator))
{
	static constexpr char	src[] = {0,1,2,3,4,5,6,7,8,9,10};
	view<char const>	v(src);
	AMTRS_TEST_EQ(*v++, 0)
	AMTRS_TEST_EQ(*v++, 1)
	AMTRS_TEST_EQ(*v++, 2)
	AMTRS_TEST_EQ(*v++, 3)
	AMTRS_TEST_EQ(*v++, 4)
	AMTRS_TEST_EQ(*v++, 5)
	AMTRS_TEST_EQ(*v++, 6)
	AMTRS_TEST_EQ(*v++, 7)
	AMTRS_TEST_EQ(*v++, 8)
	AMTRS_TEST_EQ(*v++, 9)
	AMTRS_TEST_EQ(*v++, 10)
}


AMTRS_TEST_F(_PRFX(2_iterator))
{
	static constexpr char	src[] = {0,1,2,3,4,5,6,7,8,9,10};
	view<char const>	v(src);
	AMTRS_TEST_EQ(*++v, 1)
	AMTRS_TEST_EQ(*++v, 2)
	AMTRS_TEST_EQ(*++v, 3)
	AMTRS_TEST_EQ(*++v, 4)
	AMTRS_TEST_EQ(*++v, 5)
	AMTRS_TEST_EQ(*++v, 6)
	AMTRS_TEST_EQ(*++v, 7)
	AMTRS_TEST_EQ(*++v, 8)
	AMTRS_TEST_EQ(*++v, 9)
	AMTRS_TEST_EQ(*++v, 10)
}


AMTRS_TEST_F(_PRFX(3))
{
	{
		static constexpr char	src[] = {1,2,3,4,5,6,7,8,9,10};
		view<char const>	v(src, 10);
		AMTRS_TEST_EQ(v[0], 1)
		AMTRS_TEST_EQ(v[9], 10)
		AMTRS_TEST_EQ(v.back_at(), 10)
		AMTRS_TEST_EQ(v.front_at(), 1)
	}
	{
		char		src[] = {1,2,3,4,5,6,7,8,9,10};
		view<char const>	v1(std::begin(src), std::end(src));
		view<char const>	v2(&*std::begin(src), &*std::end(src));
		view<char const>	v3(&src[0], sizeof(src));

		char const	src2[] = {1,2,3,4,5,6,7,8,9,10};
		view<char const>	v4(std::begin(src2), std::end(src2));
		view<char const>	v5(&*std::begin(src2), &*std::end(src2));
		view<char const>	v6(&src2[0], sizeof(src2));
	}
	{
		wchar_t			src[] = {1,2,3,4,5,6,7,8,9,10};
		view<wchar_t>		v1(std::begin(src), std::end(src));
		view<wchar_t>		v2(&*std::begin(src), &*std::end(src));
		view<wchar_t>		v3(&src[0], sizeof(src));

		wchar_t const	src2[] = {1,2,3,4,5,6,7,8,9,10};
		view<wchar_t const>	v4(std::begin(src2), std::end(src2));
		view<wchar_t const>	v5(&*std::begin(src2), &*std::end(src2));
		view<wchar_t const>	v6(&src2[0], sizeof(src2));
	}
	{
		struct	test_struct
		{
			int	x;
			int	y;
		};

		test_struct			src[] = { {1,2}, {3,4}, {5,6}, {7,8}, {9,10} };
		view<test_struct>	v1(std::begin(src), std::end(src));
		view<test_struct>	v2(&*std::begin(src), &*std::end(src));
		view<test_struct>	v3(&src[0], sizeof(src));
	}
}


AMTRS_TEST_F(_PRFX(4_substr))
{
	text	v("Hello world");
	AMTRS_TEST_EQ(v.trim("Hd"), "ello worl")
	AMTRS_TEST_EQ(v.substr(v.find_first_of(" ")), " world")
	AMTRS_TEST_EQ(v.substr(v.find_first_of("l")), "llo world")
	AMTRS_TEST_EQ(v.substr(v.find_last_of("l")), "ld")
	AMTRS_TEST_EQ(v.substr(v.find_first_of("le")), "ello world")
	AMTRS_TEST_EQ(v.substr(1, 4), "ello")
	AMTRS_TEST_EQ(v.substr(10), "d")
	AMTRS_TEST_EQ(v.substr(10, 99), "d")
	AMTRS_TEST_EQ((++v.substr(10, 99)).empty(), true)
	AMTRS_TEST_EQ(v.in_bounds(v.substr(4).data()), true)
	AMTRS_TEST_EQ(v.in_bounds(nullptr), false)
	AMTRS_TEST_EQ(v.out_bounds(v.substr(4).data()), false)
	AMTRS_TEST_EQ(v.out_bounds(nullptr), true)
}


AMTRS_TEST_F(_PRFX(5_reader))
{
	text	v("Hello world");

	char	buff[100];
	{
		auto	r	= io::make_reader(v);
		auto	s	= r->read(buff, sizeof(buff));
		buff[s]	= 0;
		AMTRS_TEST_EQ((text)buff, "Hello world")
	}

	{
		auto	r	= io::make_reader(v);
		r->seekg(1, std::ios_base::beg);
		auto	s	= r->read(buff, sizeof(buff));
		buff[s]	= 0;
		AMTRS_TEST_EQ((text)buff, "ello world")
	}

	{
		auto	r	= io::make_reader(v);
		r->seekg(-1, std::ios_base::end);
		auto	s	= r->read(buff, sizeof(buff));
		buff[s]	= 0;
		AMTRS_TEST_EQ((text)buff, "d")
	}

	{
		auto	r	= io::make_reader(v);
		r->read(buff, 2);
		r->seekg(-1, std::ios_base::cur);
		auto	s	= r->read(buff, 5);
		buff[s]	= 0;
		AMTRS_TEST_EQ((text)buff, "ello ")
	}
}



AMTRS_TEST_F(_PRFX(6_writer))
{
	char		t[] = "hELLO_WORLD";
	view<char>	v(t);
	auto		w	= io::make_writer(v);
	auto		s	= w->write("Hello world", sizeof("Hello world"));
	AMTRS_TEST_EQ((text)v, "Hello world")
}


AMTRS_TEST_F(_PRFX(7_constractor))
{
	std::string	s1("Hello world");
	text		v1(s1.data(), s1.data() + s1.size());
	text		v2(s1);
	AMTRS_TEST_EQ(s1, "Hello world")
	AMTRS_TEST_EQ(v1, "Hello world")
	AMTRS_TEST_EQ(v2, "Hello world")
}


AMTRS_TEST_F(_PRFX(8_constractor))
{
	using	it	= std::string::const_iterator;

	std::string				s1("Hello world");
	view<char const, it>	v1(s1.begin(), s1.end());
	view<char const, it>	v2(s1);
	AMTRS_TEST_TRUE(s1 == "Hello world")
	AMTRS_TEST_TRUE(v1 == "Hello world")
	AMTRS_TEST_TRUE(v2 == "Hello world")

	auto					v3(v2.substr(1, 6));
	std::string				s2(v3.begin(), v3.end());
	AMTRS_TEST_EQ(s2, "ello w")
}


AMTRS_TEST_F(_PRFX(9_find_fist))
{
	AMTRS_TEST_EQ(text("abcd****abcd").find_first_of("*"), 4)
	AMTRS_TEST_EQ(text("----****abcd").find_first_not_of('-'), 4)
	AMTRS_TEST_EQ(text("abcd****abcd").find_first_not_of("abcde"), 4)
}


AMTRS_TEST_F(_PRFX(9_trim))
{
	AMTRS_TEST_EQ(text("            ").trim_first(' '), "")
	AMTRS_TEST_EQ(text("            ").trim_first(" 123"), "")
	AMTRS_TEST_EQ(text("            ").trim_first("123 "), "")
	AMTRS_TEST_EQ(text("            ").trim_first("1 23"), "")

	AMTRS_TEST_EQ(text("            ").trim_last(' '), "")
	AMTRS_TEST_EQ(text("            ").trim_last(" 123"), "")
	AMTRS_TEST_EQ(text("            ").trim_last("123 "), "")
	AMTRS_TEST_EQ(text("            ").trim_last("1 23"), "")

	AMTRS_TEST_EQ(text("    ****    ").trim("\r\n\t "), "****")
	AMTRS_TEST_EQ(text("    ****    ").trim(" \t\r\n"), "****")
	AMTRS_TEST_EQ(text("abcd****abcd").trim("123abcd"), "****")
	AMTRS_TEST_EQ(text("abcd****abcd").trim_first("abcdefg"), "****abcd")
	AMTRS_TEST_EQ(text("abcd****abcd").trim_last("abcdefg"), "abcd****")
	AMTRS_TEST_EQ(text("            ").trim(' '), "")
}

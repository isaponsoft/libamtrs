/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/tests.hpp>
#include <amtrs/string/split_iterator.hpp>
#include <amtrs/system/log.hpp>
using namespace amtrs;

#define	_PRFX(_name)	string_split_iterator_##_name



AMTRS_TEST_F(_PRFX(1_chars))
{
	auto	i		= split_chars("Hello world.", "eo");
	auto	e		= std::end(i);
	int		count	= 0;

	AMTRS_TEST_EQ(*i++, "H")
	AMTRS_TEST_EQ(*i++, "ll")
	AMTRS_TEST_EQ(*i++, " w")
	AMTRS_TEST_EQ(*i++, "rld.")
	AMTRS_TEST_EQ_S(i, e, "Not terminate")
}

AMTRS_TEST_F(_PRFX(2_chars2))
{
	auto	i		= split_chars("Hello world.", "l");
	auto	e		= std::end(i);
	int		count	= 0;

	AMTRS_TEST_EQ(*i++, "He")
	AMTRS_TEST_EQ(*i++, "")
	AMTRS_TEST_EQ(*i++, "o wor")
	AMTRS_TEST_EQ(*i++, "d.")
	AMTRS_TEST_EQ_S(i, e, "Not terminate")
}


AMTRS_TEST_F(_PRFX(3_string))
{
	auto	i		= split_string("Hello world.", "o ");
	auto	s		= std::begin(i);
	auto	e		= std::end(i);
	int		count	= 0;

	AMTRS_TEST_EQ(*i++, "Hell")
	AMTRS_TEST_EQ(*i++, "world.")
	AMTRS_TEST_EQ_S(i, e, "Not terminate")
}


AMTRS_TEST_F(_PRFX(4_string2))
{
	auto	i		= split_string("Hellllo world.", "ll");
	auto	s		= std::begin(i);
	auto	e		= std::end(i);
	int		count	= 0;

	AMTRS_TEST_EQ(*i++, "He")
	AMTRS_TEST_EQ(*i++, "")
	AMTRS_TEST_EQ(*i++, "o world.")
	AMTRS_TEST_EQ_S(i, e, "Not terminate")
}


AMTRS_TEST_F(_PRFX(5_size))
{
	AMTRS_TEST_EQ(split_string("Hello world.", " ").size(), 2)
	AMTRS_TEST_EQ(split_string("Hello world.", "l").size(), 4)
}


AMTRS_TEST_F(_PRFX(6_to_array))
{
	auto	ary	= simplearray<text>(split_string("a b c d e f g", " "));
	auto	it	= std::begin(ary);
	AMTRS_TEST_EQ((text)*it++, "a");
	AMTRS_TEST_EQ((text)*it++, "b");
	AMTRS_TEST_EQ((text)*it++, "c");
	AMTRS_TEST_EQ((text)*it++, "d");
	AMTRS_TEST_EQ((text)*it++, "e");
	AMTRS_TEST_EQ((text)*it++, "f");
	AMTRS_TEST_EQ((text)*it++, "g");
	AMTRS_TEST_TRUE(it == std::end(ary));
}

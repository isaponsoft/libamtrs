/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/tests.hpp>
#include <amtrs/system/log.hpp>
#include <amtrs/memory/hash_map.hpp>
#include <amtrs/string/sstring.hpp>
#include <amtrs/io/reader.hpp>
#include <amtrs/io/writer.hpp>
#include <amtrs/system/log.hpp>
using namespace amtrs;
#define	_PRFX(_name)	memory_hash_map_##_name

AMTRS_TEST_F(_PRFX(1_emplace))
{
	hash_map<sstring, sstring>	m;

	{
		// new recode
		auto	e1	= m.emplace((std::make_pair((sstring)"hello", (sstring)"world")));
		AMTRS_TEST_TRUE(e1.second)
		AMTRS_TEST_EQ((text)e1.first->second, "world")

		// override recode
		auto	e2	= m.insert((std::make_pair((sstring)"hello", (sstring)"world.")));
		AMTRS_TEST_FALSE(e2.second)
		AMTRS_TEST_EQ((text)e1.first->second, "world.")
		AMTRS_TEST_EQ((text)e2.first->second, "world.")

		auto	i1	= m.find("hello");
		AMTRS_TEST_FALSE(i1 == m.end())
		AMTRS_TEST_EQ((text)i1->second, "world.")

		auto	i2	= m.find("Hello");
		AMTRS_TEST_TRUE(i2 == m.end())
		AMTRS_TEST_EQ(m.size(), 1)

		m.erase("hello");
		m.erase("hello");
		AMTRS_TEST_EQ(m.size(), 0)
	}

	{
		auto	e1	= m.emplace("hello", "world");
		AMTRS_TEST_TRUE(e1.second)
		AMTRS_TEST_EQ((text)e1.first->second, "world")
		AMTRS_TEST_EQ(m.size(), 1)

		m.erase("hello");
		AMTRS_TEST_EQ(m.size(), 0)
	}
}

AMTRS_TEST_F(_PRFX(1_emplace_hint))
{
	{
		// new recode
		hash_map<sstring, sstring>	m;

		auto	e1	= m.emplace_hint(m.cbegin(), (sstring)"hello1", (sstring)"world1");
		AMTRS_TEST_EQ((text)e1->second, "world1")
		auto	i1	= m.begin();
		AMTRS_TEST_EQ((text)i1->second, "world1")	++i1;
		AMTRS_TEST_TRUE(i1 == m.end())

		auto	e2	= m.emplace_hint(m.cbegin(), (sstring)"hello2", (sstring)"world2");
		AMTRS_TEST_EQ((text)e2->second, "world2")
		auto	i2	= m.begin();
		AMTRS_TEST_EQ((text)i2->second, "world2")	++i2;
		AMTRS_TEST_EQ((text)i2->second, "world1")	++i2;
		AMTRS_TEST_TRUE(i2 == m.end())

		auto	e3	= m.emplace_hint(e1, (sstring)"hello3", (sstring)"world3");
		AMTRS_TEST_EQ((text)e3->second, "world3")
		auto	i3	= m.begin();
		AMTRS_TEST_EQ((text)i3->second, "world2")	++i3;
		AMTRS_TEST_EQ((text)i3->second, "world3")	++i3;
		AMTRS_TEST_EQ((text)i3->second, "world1")	++i3;
		AMTRS_TEST_TRUE(i3 == m.end())

		// override
		auto	e4	= m.emplace_hint(e1, (sstring)"hello2", (sstring)"world2:2");
		AMTRS_TEST_EQ((text)e4->second, "world2:2")
		auto	i4	= m.begin();
		AMTRS_TEST_EQ((text)i4->second, "world2:2")	++i4;
		AMTRS_TEST_EQ((text)i4->second, "world3")	++i4;
		AMTRS_TEST_EQ((text)i4->second, "world1")	++i4;
		AMTRS_TEST_TRUE(i4 == m.end())

		auto	i5	= m.rbegin();
		AMTRS_TEST_EQ((text)i5->second, "world1")	++i5;
		AMTRS_TEST_EQ((text)i5->second, "world3")	++i5;
		AMTRS_TEST_EQ((text)i5->second, "world2:2")	++i5;
		AMTRS_TEST_TRUE(i5 == m.rend())

		// erase
		auto	r1	= m.erase(e4);
		AMTRS_TEST_FALSE(r1 == m.end())
		AMTRS_TEST_FALSE(r1 == m.rend())
		AMTRS_TEST_EQ((text)r1->second, "world3")
	}
}

AMTRS_TEST_F(_PRFX(1_try_emplace))
{
	{
		// new recode
		hash_map<sstring, sstring>	m;

		auto	e1	= m.emplace_hint(m.cbegin(), (sstring)"hello1", (sstring)"world1");
		AMTRS_TEST_EQ((text)e1->second, "world1")
		auto	i1	= m.begin();
		AMTRS_TEST_EQ((text)i1->second, "world1")	++i1;
		AMTRS_TEST_TRUE(i1 == m.end())

		auto	e2	= m.emplace_hint(m.cbegin(), (sstring)"hello2", (sstring)"world2");
		AMTRS_TEST_EQ((text)e2->second, "world2")
		auto	i2	= m.begin();
		AMTRS_TEST_EQ((text)i2->second, "world2")	++i2;
		AMTRS_TEST_EQ((text)i2->second, "world1")	++i2;
		AMTRS_TEST_TRUE(i2 == m.end())

		auto	e3	= m.emplace_hint(e1, (sstring)"hello3", (sstring)"world3");
		AMTRS_TEST_EQ((text)e3->second, "world3")
		auto	i3	= m.begin();
		AMTRS_TEST_EQ((text)i3->second, "world2")	++i3;
		AMTRS_TEST_EQ((text)i3->second, "world3")	++i3;
		AMTRS_TEST_EQ((text)i3->second, "world1")	++i3;
		AMTRS_TEST_TRUE(i3 == m.end())

		// override
		auto	e4	= m.try_emplace((sstring)"hello2", (sstring)"world2:2");
		AMTRS_TEST_EQ((text)e4.first->second, "world2")
		auto	i4	= m.begin();
		AMTRS_TEST_EQ((text)i4->second, "world2")	++i4;
		AMTRS_TEST_EQ((text)i4->second, "world3")	++i4;
		AMTRS_TEST_EQ((text)i4->second, "world1")	++i4;
		AMTRS_TEST_TRUE(i4 == m.end())

		auto	i5	= m.rbegin();
		AMTRS_TEST_EQ((text)i5->second, "world1")	++i5;
		AMTRS_TEST_EQ((text)i5->second, "world3")	++i5;
		AMTRS_TEST_EQ((text)i5->second, "world2")	++i5;
		AMTRS_TEST_TRUE(i5 == m.rend())
	}
}



AMTRS_TEST_F(_PRFX(1_insert))
{
	hash_map<sstring, sstring>	m;

	m.insert({
		std::make_pair((sstring)"hello1", (sstring)"world1"),
		std::make_pair((sstring)"hello2", (sstring)"world2"),
	});

	auto	i1	= m.find("hello1");
	AMTRS_TEST_FALSE(i1 == m.end())
	AMTRS_TEST_EQ((text)i1->second, "world1")

	auto	i2	= m.find("hello2");
	AMTRS_TEST_FALSE(i2 == m.end())
	AMTRS_TEST_EQ((text)i2->second, "world2")
}


AMTRS_TEST_F(_PRFX(2_emplace))
{
	static constexpr size_t	count	= 10000;

	simplearray<bool>				numlist(count);
	hash_map<sstring, sstring>		m;

	// 値を埋める
	for (size_t i = 0; i < count; ++i)
	{
		sstring	str(format("{p}", {(int)i}));
		m.emplace((std::make_pair(str, str)));
		numlist[i]	= true;
	}

	// ところどころ削除
	for (size_t i = 1; i < count; i += 3)
	{
		if (!numlist[i]) continue;
		m.erase(format("{p}", {(int)i}));
		numlist[i]	= false;
	}

	// 残ったものを削除
	for (size_t i = 0; i < count; ++i)
	{
		if (!numlist[i]) continue;
		m.erase(format("{p}", {(int)i}));
		numlist[i]	= false;
	}
}


AMTRS_TEST_F(_PRFX(3_iterator))
{
	// iterator
	{
		hash_map<sstring, sstring>	m;
		m.emplace((std::make_pair((sstring)"hello1", (sstring)"1")));
		m.emplace((std::make_pair((sstring)"hello2", (sstring)"2")));
		m.emplace((std::make_pair((sstring)"hello3", (sstring)"3")));

		auto	it	= m.begin();
		AMTRS_TEST_EQ((text)it->second, "1")	++it;
		AMTRS_TEST_EQ((text)it->second, "2")	++it;
		AMTRS_TEST_EQ((text)it->second, "3")	++it;
		AMTRS_TEST_TRUE(it == m.end())
		--it;	AMTRS_TEST_EQ((text)it->second, "3")
		--it;	AMTRS_TEST_EQ((text)it->second, "2")
		--it;	AMTRS_TEST_EQ((text)it->second, "1")
		--it;	AMTRS_TEST_TRUE(it == m.rend())

		m.erase("hello2");

		it	= m.begin();
		AMTRS_TEST_EQ((text)it->second, "1")	++it;
		AMTRS_TEST_EQ((text)it->second, "3")	++it;
		AMTRS_TEST_TRUE(it == m.end())
		--it;	AMTRS_TEST_EQ((text)it->second, "3")
		--it;	AMTRS_TEST_EQ((text)it->second, "1")
		--it;	AMTRS_TEST_TRUE(it == m.rend())

	}

	// revere iterator
	{
		hash_map<sstring, sstring>	m;
		m.emplace((std::make_pair((sstring)"hello1", (sstring)"1")));
		m.emplace((std::make_pair((sstring)"hello2", (sstring)"2")));
		m.emplace((std::make_pair((sstring)"hello3", (sstring)"3")));

		auto	it	= m.rbegin();
		AMTRS_TEST_EQ((text)it->second, "3")	++it;
		AMTRS_TEST_EQ((text)it->second, "2")	++it;
		AMTRS_TEST_EQ((text)it->second, "1")	++it;
		AMTRS_TEST_TRUE(it == m.rend())
		--it;	AMTRS_TEST_EQ((text)it->second, "1")
		--it;	AMTRS_TEST_EQ((text)it->second, "2")
		--it;	AMTRS_TEST_EQ((text)it->second, "3")
		--it;	AMTRS_TEST_TRUE(it == m.end())

		m.erase("hello2");

		it	= m.rbegin();
		AMTRS_TEST_EQ((text)it->second, "3")	++it;
		AMTRS_TEST_EQ((text)it->second, "1")	++it;
		AMTRS_TEST_TRUE(it == m.rend())
		--it;	AMTRS_TEST_EQ((text)it->second, "1")
		--it;	AMTRS_TEST_EQ((text)it->second, "3")
		--it;	AMTRS_TEST_TRUE(it == m.end())
	}
}

AMTRS_TEST_F(_PRFX(4_iterator_keep))
{
	hash_map<sstring, sstring>	m;
	auto	it1	= m.emplace((std::make_pair((sstring)"hello1", (sstring)"1"))).first;
	auto	it2	= m.emplace((std::make_pair((sstring)"hello2", (sstring)"2"))).first;
	auto	it3	= m.emplace((std::make_pair((sstring)"hello3", (sstring)"3"))).first;

	m.erase("hello2");	// it2 disable

	auto	it	= m.begin();
	AMTRS_TEST_TRUE(it == it1)	++it;
	AMTRS_TEST_TRUE(it != it2)
	AMTRS_TEST_TRUE(it == it3)	++it;

}

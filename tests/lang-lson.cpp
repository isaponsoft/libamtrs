/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/tests.hpp>
#include <amtrs/lang/lson.hpp>
using namespace amtrs;

#define	_PRFX(_name)	lang_lson_##_name


AMTRS_TEST_F(_PRFX(convert))
{
	static const char*	lson0	= R"({ary1[1+2+3{a+1'b+2]int1+1"bool1[]">bool2<str1'hello'str2'world})";
	static const char*	json0	= R"({"ary1":[1,2,3,{"a":1,"b":2}],"int1":1,"bool1[]":false,"bool2":true,"str1":"hello","str2":"world"})";
	static const char*	json1	= R"({ary1:[1,2,3,{a:1,b:2}],int1:1,"bool1[]":false,bool2:true,str1:"hello",str2:'world'})";
	static const char*	json2	= R"({
		ary1:
		[
			1,2,3,
		{a:1,b:2}
],
		"int1": 1,
	"bool1[]":    false,
bool2:				true,
// Comment
str1:"hello",'str2':'world'
}
)";

	char*		cur;
	char const*	src;
	int			len;

	char		buff0[1024];
	cur	= buff0;
	src	= json0;
	len	= lson::json_to_lson(&cur, std::end(buff0), &src, text(json0).end());
	buff0[len]	= 0;
	AMTRS_TEST_EQ(text(buff0), lson0)

	char		buff1[1024];
	cur	= buff1;
	src	= json1;
	len	= lson::json_to_lson(&cur, std::end(buff1), &src, text(json1).end());
	buff1[len]	= 0;
	AMTRS_TEST_EQ(text(buff1), text(buff0))

	char		buff2[1024];
	cur	= buff2;
	src	= json2;
	len	= lson::json_to_lson(&cur, std::end(buff2), &src, text(json2).end());
	buff2[len]	= 0;
	AMTRS_TEST_EQ(text(buff1), text(buff0))

	char		buff3[1024];
	cur	= buff3;
	src	= buff2;
	len	= lson::lson_to_json(&cur, std::end(buff3), &src, text(json2).end());
	buff3[len]	= 0;
	AMTRS_TEST_EQ(text(json0), text(buff3))
}

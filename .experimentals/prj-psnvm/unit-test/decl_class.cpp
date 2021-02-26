/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <iostream>
#include <string>
#include <amtrs/amtrs.hpp>
#include <passion/psnvm.hpp>

using namespace amtrs;

using namespace amtrs;
using namespace psn;

int main(int, char**)
{
	ref<passion_struct>	s1;
	ref<passion_struct>	s2;
	{
		passion_var_info	vars[] = 
		{
			passion_var_info("test1", "int32_t", {4,0}),
			passion_var_info("test2", "int32_t", {4,0}),
			passion_var_info("test3", "int32_t", {4,0}),
			passion_var_info("test4", "int",     {0,1})
		};

		s1	= new passion_struct("MyStruct", false, vars);

		std::cout << s1->name() << std::endl;
		std::cout << s1->size().compute_size() << "bytes" << std::endl;
		for (auto& v : s1->variables())
		{
			std::cout << "  " << v.name << " : " << v.position.compute_size() << std::endl;
		}
	}

	{
		passion_var_info	vars[] = 
		{
			passion_var_info("test5", "int32_t", {4,0}),
			passion_var_info("test6", "int32_t", {4,0}),
			passion_var_info("test7", "int32_t", {4,0}),
			passion_var_info("test8", "int",     {0,1})
		};


		passion_extends		exts[] = 
		{
			passion_extends(s1)
		};

		s2	= new passion_struct("MyStruct2", false, vars, exts);

		std::cout << s2->name() << std::endl;
		std::cout << s2->size().compute_size() << "bytes" << std::endl;
		for (auto& v : s2->variables())
		{
			std::cout << "  " << v.name << " : " << v.position.compute_size() << std::endl;
		}
	}

	return	0;
}

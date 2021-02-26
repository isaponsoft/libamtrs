/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <iostream>
#include <list>
#include <string>
#include <amtrs/amtrs.hpp>
#include <amtrs/filesystem/@>
#include <passion/passion.hpp>
#include <passion/vm.hpp>

using namespace amtrs;
namespace psn	= PASSION_NAMESPACE;

#define	PASSION_LANGUAGE_VERSION		1

PASSION_NAMESPACE_BEGIN

struct	arguments
{
	int							version			= 0;		//!< 0 is unknown.
	std::string					cmd;
	std::vector<std::string>	sources;

	arguments() = default;
	arguments(int _argc, char** _args);
};


arguments::arguments(int _argc, char** _args)
{
	version	= 0;				// unknown.
	cmd		= _args[0];

	for (int i = 1; i < _argc; )
	{
		++i;
	}
}



/*
struct	function : vm_traits<vm>
{
	string_type		identify;
	function_entry*	function	= nullptr;
};
*/

template<class...>
struct	native_bridge;


template<class R, class... Args>
struct	native_bridge<R(Args...)>
{


};


PASSION_NAMESPACE_END

void test()
{
	printf("OK\n");
}

void ntbridge(vmcontext* _ctx)
{
}

int main(int _argc, char** _args)
{
	if (_argc == 1)
	{
		std::cerr << "nothing filename." << std::endl;
		return	1;
	}

	psn::arguments	args(_argc, _args);

	psn::vmcontext	ctx;


	psn::function	f;

	f.identify	= "void tes()";
	f.function	= &ntbridge;
	f.value		= new brdiege(&test);
/*

	auto	fl	= amtrs::filesystem::fileloader::get_instance();
	fl->push_back(amtrs::filesystem::stdvfs::get_instance());

	std::string	filename(_args[1]);
	if (!amtrs::filesystem::is_regular_file(filename))
	{
		std::cerr << filename << " not found." << std::endl;
		return	1;
	}
*/
/*
	vmsourcecode	src(vmsourcecode::load(filename));
	lexar			lex(src);


	int	e;
	if (auto tn = read_typename(e, lex); !tn.empty())
	{
		std::cout << "typename \"" << tn << "\"" << std::endl;
	}
*/
/*
//	vm				vm;
	stntaxparser	parser;
	parser.typeList.setup_fundamental();
//	parser.vm		= &vm;
	parser.lex		= &lex;
	parser.set_root(new declaration_node());

	parser.parse();

	std::cout << "types ----------------------" << std::endl;
	for (auto t : parser.typeList)
	{
		std::cout << t.first << "(" << t.second.size() << ")" << std::endl;
	}
	std::cout << "----------------------" << std::endl;
*/
	return	0;
}

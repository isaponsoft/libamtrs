#include <iomanip>
#include <iostream>
#include <amtrs/filesystem/util.hpp>
#include <amtrs/memory/simplearray.hpp>
#include <amtrs/string/format.hpp>
#include <amtrs/string/sstring.hpp>
#include <amtrs/lang/lson.hpp>
using namespace amtrs;
using namespace crypto;

int main(int _argc, char* _args[])
{
	if (_argc != 2)
	{
		std::cerr << "filename nothing." << std::endl;
		return	1;
	}

	auto				j	= filesystem::file_get_contents<simplearray<char>>(_args[1]);
	simplearray<char>	l(j.size() + 1);

	char*				cur	= l.data();
	char const*			src	= j.data();
	auto				len	= lson::json_to_lson(&cur, l.end(), &src, j.end());
	std::cout << text(l.data(), cur) << std::endl;

	return	0;
}

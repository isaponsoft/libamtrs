#include <iostream>
#include <amtrs/filesystem/util.hpp>
#include <amtrs/memory/simplearray.hpp>
#include <amtrs/string/format.hpp>
#include <amtrs/string/sstring.hpp>
using namespace amtrs;

int main(int _argc, char* _args[])
{
	if (_argc != 2)
	{
		std::cerr << "filename nothing." << std::endl;
		return	1;
	}

	auto	data	= filesystem::file_get_contents<simplearray<uint8_t>>(_args[1]);
	int		count	= 0;
	for (auto& c : data)
	{
		std::cout << (text)format("0x{02x}, ", {c});
		if (++count >= 32)
		{
			count	= 0;
			std::cout << "\n";
		}
	}
	std::cout << "\n";
	return	0;
}

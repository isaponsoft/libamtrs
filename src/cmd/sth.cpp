#include <iomanip>
#include <iostream>
#include <amtrs/filesystem/util.hpp>
#include <amtrs/memory/simplearray.hpp>
#include <amtrs/string/format.hpp>
#include <amtrs/string/sstring.hpp>
#include <amtrs/crypto/hash-sth.hpp>
using namespace amtrs;
using namespace crypto;

int main(int _argc, char* _args[])
{
	if (_argc != 2)
	{
		std::cerr << "filename nothing." << std::endl;
		return	1;
	}

	auto	data	= filesystem::file_get_contents<simplearray<uint8_t>>(_args[1]);
	char	buff[256 / 8];
	sth<uint64_t>().compute(buff, sizeof(buff), data.begin(), data.end());

	std::cout << "STH (" << _args[1] << ") = " << std::hex << std::setw(2) << std::setfill('0');
	for (auto& c : buff)
	{
		std::cout << ((int)c & 0xff);
	}
	std::cout << std::endl;

	return	0;
}

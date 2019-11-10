#include <stdlib.h>
#include <chrono>
#include <iostream>
#include <iomanip> 
#include <random>

#include <inarikvs/inarikvs.hpp>
#include <inarikvs/kv.hpp>
#include <inarikvs/kvlist.hpp>
#include <inarikvs/hash.hpp>
#include <inarikvs/value.hpp>

using namespace inari;



int main(int _argc, char** _args)
{
	auto	test = [](std::string_view _txt)
	{
		uint64_t	hash	= hash_util{}.compute_hash((char const*)_txt.data(), _txt.size());
		uint32_t	len		= hash_util{}.compute_length(hash);
		std::cout << std::hex << std::setw(16) << std::setfill('0') << hash << " : ";
		std::cout << _txt << "(" << std::dec << len << ")";
		std::cout << std::endl;
	};
	for (int i = 1; i < 10; ++i)
	{
		char	buff[100];
		::sprintf(buff, "UserInfo:%d", i);
		test(buff);
	}

	return	0;
}

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


static constexpr std::size_t	RootSize	= 1024;

int main(int _argc, char** _args)
{
	std::random_device					rdev;
	std::mt19937						rnd(rdev());
	std::uniform_int_distribution<int>	dist(1, 100000);
	kvlist								kvl[RootSize];

	intmax_t							testCount	= 1024 * 1024;
	if (_argc == 2)
	{
		testCount	= 0;
		for (int i = 0; _args[1][i]; ++i)
		{
			testCount = testCount * 10 + _args[1][i] - '0';
		}
	}
	std::cout << "test count = " << testCount << std::endl;


	auto		start		= std::chrono::system_clock::now();
	for (intmax_t i = 0; i < testCount; ++i)
	{
		char	buff[100];
		::snprintf(buff, sizeof(buff), "Hello%d", (int)i);

		kv	val(value((std::string_view)buff), value((std::string_view)buff));

		kvlist&	kvroot	= kvl[val.key().hash()%RootSize];
		kvroot.insert(std::move(val));
	}
    auto	dur		= std::chrono::system_clock::now() - start;
    double	msec	= std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
    std::cout << (msec/1000.0) << "sec" << std::endl;



	std::size_t	count	= 0;
	for (auto& k : kvl)
	{
		count = std::max(count, k.max_depth());
	}
	std::cout << "max depth = " << count << std::endl;


	return	0;
}

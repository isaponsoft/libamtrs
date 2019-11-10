#include <amtrs/logging/@>
#include <amtrs/io/@>
#include <iostream>
#include <fstream>

using namespace amtrs;


int main()
{
	char	buff[100];
	char	src[100];

//	io::read(buff, io::iterator<char>(src));


	std::fstream	f("io.cpp");
	io::read(buff, f);

	io::io_traits<std::fstream>	a;

	


//	view<char>	mem(buff);

	auto	in2	= io::make_stream_in(view<char>(buff));

	auto	in	= io::make_limit(std::move(in2), 14);
//
	auto	rs	= io::read(src, in);
	std::cout << rs << std::endl;

	return	0;
}

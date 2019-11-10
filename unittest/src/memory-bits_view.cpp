#include <fstream>
#include <iostream>
#include <iomanip>
#include <amtrs/memory/bits_view.hpp>
#include <amtrs/io/@>

using namespace amtrs;




int main(int _argc, char* _args[])
{
#if	0
	uint8_t						buff[]	= {
									0x07, 0x00, 0xff, 0x01, 0x10, 0x18, 0x10, 0x00,
									0x01, 0xee,
									0x03, 0xaa, 0xbb, 0xcc,
								};
	auto						strm	= io::make_stream_in(make_view(buff));
	image_data<decltype(strm)>	in(strm);



	for (int i = 0; i < 10; ++i)
	{
		int	t;
		in.read<int>(t, 9);
		std::cout << std::dec  << t << ":";
	}
	std::cout << std::endl;
#else
	uint8_t						buff[]	= {
									0x00, 0xff, 0x01, 0x10, 0x18, 0x10, 0x00,
									0xee,
									0xaa, 0xbb, 0xcc,
								};

	bit_view	view(buff, sizeof(buff)*8);

	int	c = 0;
	for (auto b : view)
	{
		std::cout << (b ? "1" : "0");
		c = ++c;
		if (c == 9) { c = 0; std::cout << std::endl; }
	}
	std::cout << std::endl;


	auto	it	= view.begin();

	std::cout << std::dec << it.read(9) << std::endl; it += 9;
	std::cout << std::dec << it.read(9) << std::endl; it += 9;
	std::cout << std::dec << it.read(9) << std::endl; it += 9;
	std::cout << std::dec << it.read(9) << std::endl; it += 9;
	std::cout << std::dec << it.read(9) << std::endl; it += 9;

#endif
	return	0;
}




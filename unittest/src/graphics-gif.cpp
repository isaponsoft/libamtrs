#include <fstream>
#include <iostream>
#include <amtrs/graphics/gif/@>
#include <amtrs/memory/bits_view.hpp>
#include <amtrs/memory/shared_buffer.hpp>
#include <amtrs/graphics/bmp.hpp>

using namespace amtrs;
using namespace amtrs::gif;


AMTRS_GRAPHICS_GIF_NAMESPACE_BEGIN



AMTRS_GRAPHICS_GIF_NAMESPACE_END




template<class Stream>
int extrac(Stream& _f, std::string _outname)
{
	gif::fileheader	head;
	if (!io::read(head, _f))
	{
		return	1;
	}

	auto		gcolor	= gif::read_color_table<shared_buffer<gif::color>>(_f, head.size_of_global_color_table());
	int			anim	= 0;
	uint8_t		code;
	io::read<endian::little>(code, _f);
	while (code)
	{
		if (code == 0x3b)
		{
			break;
		}

		switch (code)
		{
			case 0x2c :
			{
				gif::image_block	ib;
				io::read(ib, _f);

				++anim;

				auto					lcolor	= gif::read_color_table<shared_buffer<gif::color>>(_f, ib.size_of_local_color_table());
				uint8_t					lzwMinCodeSize;
				io::read<endian::little>(lzwMinCodeSize, _f);

				std::vector<uint8_t>	outbuff(ib.imageWidth * ib.imageHeight);
				lzwstream_in<Stream>	gif(_f, lzwMinCodeSize);

				auto*	out	= outbuff.data();
				auto	siz	= outbuff.size();
				while (!gif.eof())
				{
					gif.read(out, siz);
					siz -= gif.gcount();
					out += gif.gcount();
				}


				bitmap<rgba<uint8_t>>	img(bitmap<rgba<uint8_t>>::size_type(ib.imageWidth, ib.imageHeight));
				auto					px	= img.pixels().data();
				gif::color const*		plt	= ib.has_local_color_table()
											? lcolor.data()
											: gcolor.data();
				for (auto c : outbuff)
				{
					px->r	= plt[c].r;
					px->g	= plt[c].g;
					px->b	= plt[c].b;
					px->a	= 0;
					++px;
				}
				std::fstream	o(std::string("tmp/") + _outname + format<std::string>("-%d.bmp", anim), std::ios::binary|std::ios::out);
				o << bmp<decltype(img)>(img);

				break;
			}

			case 0x21 :
			{
				uint8_t		label;
				io::read<endian::little>(label, _f);

				switch (label)
				{
					case gif::application_extenssion::label_code :
					{
						gif::application_extenssion			ext;
						std::vector<shared_buffer<char>>	buf;

						io::read(ext, _f, true, [&](size_t _size) -> char*
						{
							shared_buffer<char>	buff(_size);
							buf.push_back(buff);
							return	buff.data();
						});
						break;
					}

					case gif::controll_extenssion::label_code :
					{
						gif::controll_extenssion				ext;
						io::read(ext, _f, true);
						break;
					}

					default:
					{
						std::cout << "unkown label:" << std::hex << (unsigned int)label << std::endl;
						return	1;
					}
				}
				break;
			}

			default:
			{
				std::cout << "unkown introdoctor:" << std::hex << (unsigned int)code << std::endl;
				return	1;
			}
		}
		io::read<endian::little>(code, _f);
	}


	return	0;
}


void load(std::string _name)
{
	std::cout << "file " << _name << std::endl;
	std::fstream	f(_name, std::ios::binary|std::ios::in);
	if (!f.good())
	{
		std::cout << "Open error" << std::endl;
		return;
	}
	extrac(f, _name);
}



int main(int _argc, char* _args[])
{

//	load("test.gif");
//	load("test3.gif");
//	load("test2.gif");
//	load("_test.gif");
	load("test.gif");

//	load("black.gif");

	return	0;
}

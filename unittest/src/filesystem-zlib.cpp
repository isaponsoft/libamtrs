#include <iostream>
#include <fstream>
#include <amtrs/opt/io/zlib-stream_in.hpp>
#include <amtrs/filesystem/@>
#include <amtrs/filesystem/zip/@>




int main()
{
	auto	f	= std::fstream("data/example.zip", std::ios::in|std::ios::binary);
	if (!f.good())
	{
		std::cout << "example.zip can't open." << std::endl;
		return	1;
	}


	auto	z	= amtrs::filesystem::zip::make_zipfs(f);
	for (auto e : z)
	{
		std::cout << std::string(e.filename());

		auto	es	= z.open(e);

		// 書き込み先
		auto	out	= std::ofstream(std::string("tmp/") + std::string(e.filename()), std::ios::binary);

		// Inflate(ZIP)を展開しながら読み込むストリーム
		auto	in	= amtrs::io::inflate_stream_in<decltype(es), decltype(es)&>(es);

		// in => out へコピーする。
		amtrs::io::stream_copy(out, in);

		std::cout << " crc32 : " << std::hex << in.checksum() << "/" << e.crc32 << std::endl;
	}



	return	0;
}

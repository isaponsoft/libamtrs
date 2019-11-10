#include <fstream>
#include <iostream>
#include <amtrs/net/@>
#include <amtrs/net/http/@>
#include <amtrs/net/socket/@>
#include <amtrs/logging/@>
#include <amtrs/string/@>
#include <amtrs/io/@>
#include <amtrs/filesystem/@>

using namespace amtrs;
using namespace amtrs::net;


int main(int _argc, char* _args[])
{
	for (int i = 1; i < _argc; ++i)
	{
		net::socket		sock;
		uri::info<char>	uinf;
		if (net::uri::parse(uinf, (std::string_view)_args[i]).empty())
		{
			std::cout << _args[i] << " uri error." << std::endl;
			continue;
		}

		sock	= net::socket::connect(&uinf, _args[i]);
		if (sock.empty())
		{
			std::cout << uinf.host << " connection error." << std::endl;
			continue;
		}
		//std::cout << uinf.host << " connection." << std::endl;

#if	AMTRS_SSL_SUPPORTED
		io::vstream<char>	strm	= uinf.scheme == "https"
									? io::make_vstream(net::ssl_stream<char>(sock.get()))
									: io::make_vstream(net::socket_stream(sock.get()));
#else
		io::vstream<char>	strm	= io::make_vstream(net::socket_stream(sock.get()));
#endif

		std::string		req = "GET ";
		req += uinf.uri;
		req += " HTTP/1.1";
		req += "\r\n";
		req += "Host: ";
		req += uinf.host;
		req += "\r\n";
		req += "User-Agent: libamtrs\r\n";
		req += "Accept: */*\r\n";
		req += "\r\n";
		//std::cout << "GET " << uinf.uri << std::endl;


		size_t	ws	= 0;
		while (!strm.bad() && !strm.eof() && (ws != req.size()))
		{
			strm.write(req.data() + ws, req.size() - ws);
			ws += strm.pcount();
		}


		auto	filename	= trim(uinf.path, "/").empty()
							? std::string(uinf.host)
							: std::string(trim(filesystem::filename(uinf.path), "/"));
		net::http::parser	psr(uinf.scheme == "https");
		auto	o			= std::fstream(std::string("tmp/") + filename + ".header", std::ios::binary|std::fstream::out|std::fstream::trunc);
		psr.header(strm, [&](char const* _line, size_t _size)
		{
			auto	line	= trim(std::string_view(_line, _size), "\r\n");
			o << line << std::endl;
			if (filesystem::extension((std::string_view)filename).empty() || trim(uinf.path, "/").empty())
			{
				if (starts_with((std::string_view)lowercase<std::string>(line), (std::string_view)"content-type:"))
				{
					auto	mime	= trim(line.substr(line.find_first_of(":")), " \t");
					auto	ext		= mime.substr(mime.find_first_of('/') + 1);
					if (auto pos = ext.find_first_of(';'); pos != std::string_view::npos)
					{
						ext	= ext.substr(0, pos);
					}
					filename	+= ".";
					filename	+= ext;
				}
			}
			return	true;
		});

		o			= std::fstream(std::string("tmp/") + filename, std::ios::binary|std::fstream::out|std::fstream::trunc);
		if (!o.good())
		{
			std::cout << "Can't create " << filename << std::endl;
		}


		if (strm.good())
		{
			do
			{
				char	buff[1000];
				auto	rs	= psr.read(buff, strm, sizeof(buff));
				if (rs == 0)
				{
					break;
				}
				o << std::string_view(buff, rs);
			} while (strm.good() && psr.contents_rest() > 0);
		}
		if (o.good())
		{
			std::cout << uinf.uri << "  : save to 'tmp/" << filename << "'." << std::endl;
		}
	}
	
	return	0;
}




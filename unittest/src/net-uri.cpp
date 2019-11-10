#include <iostream>
#include <amtrs/net/@>
#include <amtrs/logging/@>
#include <amtrs/string/@>

using namespace amtrs;
using namespace amtrs::net;



void test(std::string_view _uri)
{
	std::cout << "IN: " << _uri << std::endl;

	uri::info<char>	buff;
	auto	path	= uri::parse(buff, _uri);
	std::cout << "[URI      ] ";
	if (path.empty())
	{
		std::cout << "NOT URI" << std::endl;
	}
	else
	{
		std::cout << path << std::endl;
	}
	std::cout << "[scheme   ] " << buff.scheme    << std::endl;
	std::cout << "[authority] " << buff.authority << std::endl;
	std::cout << "[userinfo ] " << buff.userinfo  << std::endl;
	std::cout << "[username ] " << buff.username  << std::endl;
	std::cout << "[password ] " << buff.password  << std::endl;
	std::cout << "[host     ] " << buff.host      << std::endl;
	std::cout << "[port     ] " << buff.port      << std::endl;
	std::cout << "[path     ] " << buff.path      << std::endl;
	std::cout << "[query    ] " << buff.query     << std::endl;
	for (auto& q : uri::query_iterator(buff.query))
	{
		auto	field	= q.field();
		auto	value	= q.value();
		std::cout << "            +- ";
		if (field.empty())
		{
			std::cout << value << std::endl;
		}
		else
		{
			std::cout << field << " = " << value << std::endl;
		}
	}
	std::cout << "[fragment ] " << buff.fragment  << std::endl;
	std::cout << std::endl;
}


int main(int _argc, char* _args[])
{
	if (_argc == 1)
	{
		test("foo://MyName::MyPassword0:@example.com:12345/path/pathB/pathC?query1=1&query2=2#test/?");
		test("ftp://ftpuser:@ftp.Ftp.example.com/path/pathB/pathC?query1=1&query2=2");
		test("foo+bar://foo:bar@[1111]::1:2:3:4:192.168.100.10/pathA?quey#header");

		// faile test
		test("foo+bar://foo:bar@[0:1:2:3:4:5:6:7]::/pathA?quey#header");
	}
	else
	{
		for (int i = 1; i < _argc; ++i)
		{
			test(_args[i]);
		}
	}
	return	0;
}



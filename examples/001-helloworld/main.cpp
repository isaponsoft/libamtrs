#include <amtrs/filesystem.hpp>
#include <amtrs/string.hpp>

using namespace AMTRS_NAMESPACE;

namespace fs = AMTRS_NAMESPACE::filesystem;

int main()
{
	// helloworld.txt is "Hello world!".
	std::string	filedata = fs::file_get_contents<std::string>("helloworld.txt");

	// wildcard match.
	if (wcmatch<std::string_view>(filedata, "*ello*d!"))
	{
		std::cout << "Hello world!" << std::endl;
	}
	return	0;
}
# Welcome!

簡単に使える c++17 クロスプラットフォームライブラリ。

## Example

#### main.cpp

```c++
#include <amtrs/filesystem.hpp>
#include <amtrs/string.hpp>

using namespace AMTRS_NAMESPACE;

namespace fs = AMTRS_NAMESPACE::filesystem;

int main()
{
	// helloworld.txt is "Hello world!".
	std::string	filedata = fs::file_get_contents<std::string>("helloworld.txt");

	// wildcard match.
	if (wcmatch(filedata, "*ello*d!"))
	{
		std::cout << "Hello world!" << std::endl;
	}
	return	0;
}
```

#### CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.0.2)
project(helloworld)
set(AMTRS_APPNAME "helloworld")

# Please set libamtrs copied directory!
set(libAmtrs_DIR ../libamtrs)
```

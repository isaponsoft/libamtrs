# Welcome!

簡単に使える c++17 クロスプラットフォームライブラリ。

tested, ```windows10```, ```FreeBSD 14-CURRENT```, ```Ubuntu 20.04.2 LTS```

# Build & install

Build and install.

## Windows (Microsoft Visual C++)

```shell
> git clone https://github.com/isaponsoft/libamtrs.git
> cmake -G "NMake Makefiles" -DCMMAKE_INSTALL_PREFIX=. -DCMAKE_BUILD_TYPE=Release .
> nmake install
```

## Unix like (clang++ or gnuc++, compatibles)

```shell
% git clone https://github.com/isaponsoft/libamtrs.git
% cmake -DCMMAKE_INSTALL_PREFIX=. -DCMAKE_BUILD_TYPE=Release  .
% make install
```

## Example

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
cmake_minimum_required(VERSION 3.16)
project(helloworld)

set(AMTRS_INSTALLED_DIR "path/to/libamtrs/install/directory")

if(MSVC)
set(CMAKE_CXX_STANDARD			20)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
else()
set(CMAKE_CXX_STANDARD			17)
endif()


add_executable(helloworld main.cpp)


target_include_directories(helloworld PUBLIC ${AMTRS_INSTALLED_DIR}/include)
target_link_libraries(helloworld ${AMTRS_INSTALLED_DIR}/lib/amtrs_static.lib)
```

#### Helloworld.txt

```
Hello world!
```

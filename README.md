# Welcome!

Cross platform library for c++17. c++20 ready.

tested, ```windows10```, ```FreeBSD 14-CURRENT```, ```Ubuntu 20.04.2 LTS```

# Build & install

Build and install.


```bash
git clone https://github.com/isaponsoft/libamtrs.git
cmake -DCMMAKE_INSTALL_PREFIX=. -DCMAKE_BUILD_TYPE=Release .
msbuild.exe INSTALL.vcxproj # Windows(MSVC)
make install                # Unix like
```


## Generated files.

|path|note|
|--|--|
|```bin/```|amtrs contribute tools|
|```include/```|libamtrs c++ header files|
|```lib/amtrs_static(.lib\|.a)```|libamtrs static library|


## Example

```shell
> cd helloworld
> cmake . -DCMAKE_PREFIX_PATH=/path/to/amtrs/install
```
has ```/path/to/amtrs/install/include/amtrs```, ```/path/to/amtrs/install/lib/amtrs(.a|_static.lib)```

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
cmake_minimum_required(VERSION 3.16)
project(helloworld)

if(MSVC)
set(CMAKE_CXX_STANDARD			20)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
else()
set(CMAKE_CXX_STANDARD			17)
endif()

set(Amtrs_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
find_package(Amtrs)

add_executable(helloworld main.cpp)
target_include_directories(helloworld PUBLIC ${AMTRS_INCLUDE_DIRS})
target_link_libraries(helloworld ${AMTRS_LIBRARY})
```

#### Helloworld.txt

```
Hello world!
```

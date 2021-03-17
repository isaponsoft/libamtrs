# Welcome!

c++17向けのクロスプラットフォームライブラリです. c++20 も視野に入れています.

# Build & install

ビルドとインストール。

## Windows (Microsoft Visual C++)

```shell
> git clone https://github.com/isaponsoft/libamtrs.git
> cmake -DCMMAKE_INSTALL_PREFIX=.  .
> nmake install
```

## Unix like (clang++ or gnuc++, compatibles)

```shell
% git clone https://github.com/isaponsoft/libamtrs.git
% cmake -DCMMAKE_INSTALL_PREFIX=.  .
% make install
```

## 生成されるファイル

|path|note|
|--|--|
|```bin/```|amtrs contribute tools|
|```include/```|libamtrs c++ header files|
|```lib/amtrs_static(.lib\|.a)```|libamtrs static library|


## 使用例

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

add_execute(helloworld main.cpp)

set(AMTRS_INSTALLED_DIR "path/to/libamtrs/install/directory")

target_inclide_directories(helloworld PUBLIC ${AMTRS_INSTALLED_DIR}/install)
target_link_libraries(helloworld ${AMTRS_INSTALLED_DIR}/lib/amtrs_static)

```

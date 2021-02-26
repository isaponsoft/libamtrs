# libamtrs

## Overview

libamtrsはクロスプラットフォーム開発が簡単に行えることを目標としたライブラリ群です。Window, Linux, *BSD, macos, Android, iOS 向けの開発を行うことが出来ます。

## Index

* [Class manual](classes.md)

## cmake

[cmake](http://cmake.org)を使うことで複雑な設定を行うことなくビルドすることが出来ます。

## Tutorial

実際に使ってみましょう。

### 1. Download

まずはlibamtrsをダウンロードしてください。gitコマンドでクローンしても良いですし、zipをダウンロードして展開しても構いません。下記の例は gitコマンドによるクローンです。

```shell
git clone https://github.com/isaponsoft/libamtrs libamtrs
```

### 2. Project

次に簡単なソースコードとCMakeLists.txtを用意します。チュートリアルでは単にファイルを読み込んで表示するだけのものです。

#### main.cpp

```c++
#include <amtrs/amtrs.hpp>
#include <amtrs/filesystem.hpp>
#include <iostream>
#include <string>

int main(int argc, char** args)
{
	std::string data
                 = amtrs::filesystem::file_get_contents<std::string>(args[1]);
	std::cout << data << std::endl;
	return	0;
}
```

#### CMakeLists.txt

CMakeLists.txtは最初はとっつきにくいところがありますが、単純な構成のアプリケーションをビルドするためのテンプレートを libamtrs に用意してあります。以下のようなCMakeLists.txtを作り、libamtrsの中のtemplate.app-console.cmakeをincludeすると簡単にアプリケーションをビルドできます。

```cmake
cmake_minimum_required(VERSION 3.0.2)
project(tutorial)

set(AMTRS_APPNAME "tutorial")

set(libAmtrs_DIR libamtrs)
include(${libAmtrs_DIR}/template.app-console.cmake)
```

## 3. Build

それではビルドしてみます。cmakeを実行し、開発環境の標準のビルドコマンドを使うだけです。うまくビルドが通り、CMakeLists.txt の中身が表示されたら完成です。

#### for Visual Studio 2019 (Windows)

```shell
cmake.exe
msbuild.exe tutorial.vcxproj
Debug\tutorial.exe CMakeLists.txt
```

#### for clang or gcc

```shell
cmake
make
./tutorial CMakeLists.txt
```

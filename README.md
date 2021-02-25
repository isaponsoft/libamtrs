# Welcome!

Cross platform library and program language passion.

日本語 : README-jp.md


# Getting Started

## System Requirements

Microsoft Windows, macOS, Linux, *BSD, Android, iOS.

## Build & test passion


### Build system Requirements

c++17 compiler(clang++ >= 6.0.0, VisualC++ >= 2019, etc...)



### for Windows

```shell
cd libamtrs\prj-psnvm
build.bat
test.bat
```


### for Linux, *BSD, macOS

```shell
cd libamtrs/prj-psnvm
sh build.sh
sh test.sh
```



| OS      | Intel 32bit | Intel 64bit   |
| ------- | ----------- | ------------- |
| macos   | Darwin-x86  | Darwin-x86_64 |
| FreeBSD | FreeBSD-x86 | FreeBSD-amd64 |
| Linux   | Linux-x86   | Linux-x86_64  |

## Run passion 

rename to psnvm.exe(Windows) or psnvm(Other OS).

```sh
psnvm SCRIPT-FILE-NAME
```

## Passion  coding

Read libamtrs/docs directory.Passion is alpha version.

## Depenency libraries

Depenency libs copy to "libamtrs/deps/xxx" or "libamtrs/../xxx".

|LIB|directory|Type|Platform|URL|
|--|--|--|--|--|
|JPG|libjpeg-turbo|source|all||
|OGG|libogg, libvorbis|source|all||
|OpenCV|OpenCV-android-sdk|binary|android||
|OpenCV|openc-win|binary|windows||
|OpenCV|opencv2.framework|binary|ios||
|PNG|libpng|source|all||
|SSL|libressl|source|all||
|ZLIB|zlib|source|all||

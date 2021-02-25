# Welcome!

クロスプラットフォームライブラリlibamtrsとプログラム言語passionです。

English document (README.md)

# はじめかた

## システム要求

Microsoft Windows, macOS, Linux, *BSD, Android, iOS.

## Build & test passion

### for Windows(Visual Studio 2019)

Visual C++ 2019 が呼び出せる環境でビルドスクリプトを実行します。

```shell
cd libamtrs/prj-psnvm
build.bat
test.bat
```

libamtrs/binディレクトリに「psnvm-win-AMD64.exe」もしくは「psnvm-win-x86.exe」が生成されます。通常はpsnvm.exeにリネームしてご利用ください。

### for Linux, *BSD, macOS

clang++を各プラットフォームの推奨する方法でパッケージから追加してください。

```shell
cd libamtrs/prj-psnvm
sh build.sh
sh test.sh
```

libamtrs/binディレクトリに「psnvm-[OS]]-[ARCHITECTURE]」というバイナリが生成されます。ＯＳとアーキテクチャの対応表は以下のようになります。

| OS      | Intel 32bit | Intel 64bit   |
| ------- | ----------- | ------------- |
| macos   | Darwin-x86  | Darwin-x86_64 |
| FreeBSD | FreeBSD-x86 | FreeBSD-amd64 |
| Linux   | Linux-x86   | Linux-x86_64  |

## Passion のスクリプトを実行する

ビルドしたpsnvmの実行ファイルを psnvm.exe もしくは psnvm にリネームしたとします。

```sh
psnvm スクリプトファイル名
```

## Passion の書き方

libamtrs/docs に記載しています。アルファ版ですので未対応のもの不具合のあるも、仕様が変更される可能性があるものが含まれています。


## 依存ライブラリ

依存するライブラリを "libamtrs/deps/xxx" または "libamtrs/../xxx" にコピーしてください。

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

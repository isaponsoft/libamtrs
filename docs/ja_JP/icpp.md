# icpp - Instant Run C++

C++のソースコードをスクリプトライクに即席で呼び出すためのソフトウェアです。いくつかの独自のルールがありますが、基本的にはc++のソースコードをビルドして実行します。cmakeが実行可能な環境とcmakeが認識可能なコンパイラが必要です。

推奨する拡張子は .cpx (C Plusplus eXecute) です。

## 作ったきっかけ

UnixライクなOS(特にFreeBSDを好んで使っていますが)を使っている際にシェルスクリプトを書くのが面倒だなーと思ったことです。シェルによって方言があったり書き方がやっぱり独特だったり。出来ればc++でシェルスクリプトを書ければ楽なのに…と思ったのがきっかけです。

あと、Windowsでもシェルスクリプトを使えたらいいのに…！WSLは便利ですが、やっぱ生のWindowsと*BSD/Linuxで共通して動くものが使いたかったのです。


## 使い方

```shell
icpp [OPTIONS] icppSCRIPT [SCRIPT-ARGS...]
```

## セットアップ

icppはcmakeやコンパイラを検索して実行します。これらのパッケージが見つからなかった場合、自動的にインストールを行うかどうかをユーザーに問い合わせます。

ただし、次のように```--setup```オプションを指定すると任意のタイミングでセットアップを行うことが出来ます。```--setup-gui```オプションは問い合わせ内容をコンソールではなくGUIウィンドウに表示します(Unix系ではXで実行している必要があります)。

```shell
icpp --setup
icpp --setup-gui
```



## ディレクトリ

icppはlibamtrsの一部として以下のディレクトリ内にファイルを配置します。アプリケーションを完全に削除する場合は以下のディレクトリを丸ごと削除してください。WindowsやosxではInstallディレクトリをPATHに登録しておくと便利です。

|OS|Type|Path|
|--|--|--|
|Windows|Cache|```%USERPROFILES%\AppData\Local\libamtrs\.cache\icpp```|
|Windows|Libs|```%USERPROFILES%\AppData\Local\libamtrs\.local\icpp```|
|Windows|Config|```%USERPROFILES%\AppData\Local\libamtrs\.config\icpp```|
|Windows|Install|```%USERPROFILES%\AppData\Local\libamtrs\bin```|
|Unix|Cache|```$HOME/.cache/libamtrs/icpp```|
|Unix|Libs|```$HOME/.local/libamtrs/icpp```|
|Unix|Config|```$HOME/.config/libamtrs/icpp```|
|Unix|Install|```/usr/local/bin```|
|osx|Cache|```$HOME/Library/libamtrs/.cache/icpp```|
|osx|Libs|```$HOME/Library/libamtrs/.local/icpp```|
|osx|Config|```$HOME/Library/libamtrs/.config/icpp```|
|osx|Install|```$HOME/Library/libamtrs/bin```|


## スクリプトの書き方

```main()``` 関数の代わりに ```imain()``` 関数を書きます。```imain()```は```main()```関数から直接呼ばれます。

```c++
int imain()
{
	std::cout << "Hello world." << std::endl;
	return	0;
}
```

*シバン(shebang)* にも対応しています。 

```c++
#!env icpp
int imain()
{
	std::cout << "Hello world." << std::endl;
	return	0;
}
```

```imain()``` は複数バージョンがあり、好きなものを定義して使用できます。単にコマンドライン引数をどのような形で受け取るか以外の差はありませんので好きなものを使用してください。

```c++
#!env icpp

int imain();
int imain(int c, char** v);
int imain(int c, char* v[]);
int imain(ssu::array<std::string> arg);
```

## 依存モジュール

```icpp``` ではOpenCVなどビルドに時間のかかる機能を拡張モジュールと呼び指定がない限りは使わないようになっています。使用したいモジュールは ```@depenence``` というディレクティブで書きます。icppは
 ```@depenence``` を見つけると、ビルドシステムにモジュールのビルドを追加します。

```c++
#!env icpp
@depenence Amtrs::OpenCV, Amtrs::CMark

int imain()
{
	std::cout << "Hello world." << std::endl;
	return	0;
}

```

|Module|Note|
|--|--|
|```Amtrs::OpenCV```|OpenCV|
|```Amtrs::CMark```|Commonmark gfm|
|```Amtrs::Vorbis```|Ogg audio format|
|```Amtrs::SSL```|LibreSSL|

### 仕組み

```@depenence``` を見つけると、モジュール名をリストアップしソースコードから除外します。続けてモジュール名に対応した cmake module を検出し、ビルドシステムにインポートします。

また次のようにcmakeのコマンドを埋め込むことが出来ます。

```c++
#!env icpp --spec 1
@cmake::external_module(mymodule foo.h foo http://github.com/foo/bar.git CMAKE_XYZ=Foo CMAKE_ABC=Bar)
@cmake::include(mymodule2.cmake)
@cmake::start
set(MYLIB_INCLUDE_DIRS	mylib/include)
set(MYLIB_LIBRARIES		mylib/lib/mylib${CMAKE_LIBRARY_POSTFIX})
target_link_library(${ICPP_TARGET} ${MYLIB_LIBRARIES})
target_include_directories(${ICPP_TARGET} PUBLIC ${MYLIB_INCLUDE_DIRS})
@cmake::end
int imain()
{
	std::cout << "Hello world." << std::endl;
	return	0;
}
```


## そのほかのオプション

### ```--spec```

icppの仕様を選択します。省略時はicpp内での最新の状態です。

### ```--no-predelete```

icppはアプリをビルドする際に、事前に実行ファイルを削除します。このオプションを指定した場合はファイルを削除しないようにします。

### ```--keep-buildwork```

ビルド時に使用した作業ファイルを削除せずに残します。スクリプトの開発中はこのオプションを有効にするとリビルド時間が短縮されます。

### ```--show-build-message```

ビルド中のメッセージを表示します。デフォルトではビルド中のメッセージはコンパイルエラーが発生した時のみ表示します。

### ```--clean-build```

作業ディレクトリを削除し、きれいな状態でビルドしなおします。

### ```--test```

実際にはビルドも実行もしませ。替わりに実行予定のコマンドを表示します。

### ```--eval```

コマンドラインから入力された内容を直接実行します。キャッシュディレクトリ内にはコマンドの内容をMD5でハッシュ化したファイル名が使用されます。

### ```--output```

ビルド結果を指定したファイル名で保存します。

### ```--install```

ビルド結果をプラットフォームのデフォルトのパスに保存します。OSによっては特権ユーザーで実行する必要があります。```--output```にデフォルトパスを指定したものと同じ結果になります。

### ```--output-project```

ソースコードとCMakeLists.txtを指定した位置に出力します。


## 関数

icpp.hpp によりいくつかの関数が追加されます。シェルスクリプトで便利になるものが中心です。icpp.hppの機能はlibamtrsで実装されているため、icpp以外からでも利用可能です。

### ```array_shift(list)```

std::list<> の先頭の要素を取り出し、listの先頭をpopします。list.empty()の場合は空のデータを返します。

### ```ls()```, ```cat()```

UnixライクなOSで使われる ```ls``` や ```cat``` コマンドを再現します。

### ```pkg.install()```, ```pkg.search()```, ```pkg.delete()```

パッケージやアプリケーションのインストールを行います。```github.com/isaponsoft/icpp-packageinfo/``` の内容をもとに検索を行います。

### ```reg_match()```, ```reg_replace()```

正規表現を用いてマッチ、もしくは置換を行います。

### ```exec()```, ```run()```

外部のプログラムを実行します。```exec()``` の場合は出力結果を文字列で返します。```run()```の場合は出力をパイプします。

### ```mkdir()```, ```rmdir()```, ```cd()```, ```pwd()```

### ```dirname()```, ```abspath()```, ```extensions()```, ```stem()```, ```filename()```


## Excample

```c++
#!env icpp

auto	command_name	= array_shift(args);

std::cout << "command is " << command_name << std::endl;

for (auto file : args)
{

}
```


## Example



```c++
#!env icpp
#include <stdlib.h>

auto	command_name	= array_shift(args);
printf("%s\n", command_name.c_str());
```

```c++
#include <icpp/icpp.hpp>
#include <stdlib.h>

int proccess(icpp::global& g)
{
	auto	args			= g.args();
	auto	command_name	= array_shift(args);
	printf("%s\n", command_name.c_str());
}
```

## Cache files

|Type|Win|FreeBSD|Linux|
|--|--|--|--|
|Config|```AppData\Local\libamtrs\icpp\.icpp```<br/>```AppData\Roaming\libamtrs\icpp\.icpp```|```.config/libamtrs/icpp/.icpp```<br />```.icpp```|```.config/libamtrs/icpp/.icpp```<br />```.icpp```|
|Install|```AppData\Roaming\libamtrs\icpp```|```/usr/local/bin```|```/usr/bin```|
|Cache|```AppData\Local\libamtrs\icpp\cache```|```.cache/libamtrs/icpp```|```.cache/libamtrs/icpp```|



## Idea note

以下はまだ検討中の項目です。

### Manifest

```c++
#!env icpp
manifest { // yaml subset.
	name: MyAppli1
    license: 
		type: BSD license
		url: http://foo.bar/myapp1/LISENCE.txt
	cache:
		- mode: unzip
		  url: http://foo.bar/myapp1.zip
		  dir: data
	sources:
		- file: data/libs/myfunctions.cpp
}

std::cout << myfunctions_get_helloworld() << std::endl;
```

```c++
#include <string>

std::string myfunctions_get_helloworld()
{
	return "Hello World";
}
```

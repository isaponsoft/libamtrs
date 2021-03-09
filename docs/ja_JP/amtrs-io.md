# io

|class|node|
|---|---|
|vstreambuf|仮想化したstd::streambuf|
|vstream|仮想化したstd::iostream|
|xxx_streamif|仮想関数を一切含まないiostream。basic_streamifのエイリアス|
|vstreamif|仮想化されたvstreamif|


## streamif と vstreamif

```io::streamif``` は何かしらのバッファに対するアクセスを共通化するためのテンプレートです。オーバーヘッドは最適化によってほとんどありません。使い方は std::iostream とよく似ています。

```io::streamif``` を抽象化したものとして ```io::vstreamif``` があります。仮想関数を使うことで呼び出しのオーバーヘッドが多少増えますが実装を隠ぺいすることが出来ます。それ以外の使い化は ```io::streamif``` とほぼ同じです。

### Example

```c++
#include <amtrs/io.hpp>
using namespace amtrs;
using namespace amtrs::io;

streamif<std::string_view>  in(std::string_view("Test"));
while (in)
{
	// getc は呼び出しごとに read() を呼ぶため
	// コストが増大するので注意
	char   c = in.getc();
	if (c != in.eof())
	{
		std::cout << c;
	}
}
std::cout << std::endl;
```

#### amtrs::io::streamif と std::iostream の違い

||std::iostream|amtrs::io::streamif&lt;T&gt;|
|--|--|--|
|read|```operator >>```,```read```, ```get```, ```get```,```peek```,```getline```,```readsome```|```read```, ```getc```|
|write|```operator <<```, ```write```, ```put```|```write```|
|seek|```seekg```, ```seekp```|```seek```|
|tell|```tellg```, ```tellp```|```tell```|
|size|-|```size```|
|count|```gcount```,```pcount```|```read```, ```write``` return value|
|format|✓|-|
|locale|✓(```imbue```)|-|
|buffer|✓(```std::streambuf```)|-|
|read cache|✓|-|
|event callback|✓|-|
|over head|large|none|

### Example

```c++
#include <amtrs/io.hpp>

auto in = io::make_streamif("Hello world");
std::string s = io::read<std::string>(in, io::size(in));
std::cout << s << std::endl;
```


```c++
#include <amtrs/io.hpp>
#include <amtrs/net.hpp>

net::uri::info<char>	info;
auto	sock	= io::make_streamif(net::socket::connect(&info, "http://github.com", SOCK_STREAM));
auto	ws		= io::write(sock, "GET /\r\n\r\n");
while (sock)
{
	auto	rs	= io::read<std::string>(sock, 2000);
	std::cout << rs;
}
```


## streamif_traits

```c++
template<class T, class CharT, class Traits>
struct	streaim_traits<T, CharT, Traits>;
```

|member|note|
|---|---|
|seek|位置変更をサポートす場合に宣言する|
|tell|位置取得をサポートする場合に必須|
|read|読み取りをサポートする場合に宣言する|
|write|書き込みをサポートする場合に宣言する|
|size|サイズ取得をサポートする場合に宣言する。sizaが無くてもseekとtellをサポートする場合、streamifはそれを用いる|

## streamif

|member|note|
|--|--|
|read|バッファにデータを読み取る|
|write|バッファのデータを書き込む|
|seek|読み書きの位置を変更する|
|tell|読み書きの位置を取得する|
|size|サイズを取得する|


## make_streamif()

```c++
io::streamif<decltype(data)>  s = make_streamif(std::move(data));
io::vstreamif<decltype(data)> s = make_vstreamif(std::move(data));
```

## streamif::read(), vstreamif::read()

1. ```read(char_type* dest, STREAM& in, size_t size)```
2. ```read(char_type (&dest)[N], STREAM& in)```
3. ```read<DEST>(STREAM& in, size_t size)```

in からデータ読み取ります。

1. sizeで指定される量のデータを読み取ります。
2. ```read(&dest[0], in, N)``` と同等です。
3. DESTで示すクラスのオブジェクトを生成し、その中にデータを読み込んで返します。

### Parameters

### Example

```c++
(1)
char buf[100];
io::read(buf, in, 100);

(2)
char buf[100];
io::read(buf, in);

(3)
std::string s = io::read<std::string>(in, size);
std::vector<char> s = io::read<std::vector<char>>(in, size);
```


## write

```c++
char buf[100];
io::write(out, buf, 100);

char buf[100];
io::write(out, buf);

io::write(out, std::string_view("Test"));
```

## size

```c++
size_t s = io::size(in);
```

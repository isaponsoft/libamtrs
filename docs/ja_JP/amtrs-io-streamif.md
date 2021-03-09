# amtrs::io::basic_streamif

```c++
template<
    class Source,
    class Elm,
    class Traits = streamif_traits<Source, Elm>
> class basic_streamif;

template<
    class Elm,
> class basic_vstreamif;
```

配列列に対して連続したアクセスを行うための共通インターフェースを提供します。ロケールなどは考慮せず純粋な配列として扱います。またテンプレートのみで実装されているためオーバーヘッドが少なくなっています。


|Type|Definition|
|--|--|
|```streamif<S>```|```basic_streamif<S, char, stream_traits<S>>```|
|```vstreamif```|```basic_vstreamif<char>```|

## Template parameters

|name|note|
|--|--|
|Source|入力元のオブジェクト型。|
|Elm|扱う要素の型。|
|Traits|Sourceの扱い方を示すクラス。|

## Member types

|Member type|Definition|
|--|--|
|source_type|```Source```|
|char_type|```Elm```|
|traits_type|```std::char_traits<char_type>```|
|source_traits|```Traits```|
|fpos_type|```source_traits::fpos_type```|
|size_type|```source_traits::size_type```|

## Member functions

|Function|Note|
|--|--|
|(constractor)||
|read||
|write||
|seek||
|tell||
|size||

## Example

```c++
#include <amtrs/io.hpp>
#include <fstream>

char			buf[100];
size_type       r;

// OK. 1
{
    // move された場合、streamifは完全な std::ifstream を内包する。
    // streamif が破棄されるとifstreamも破棄される。
	std::ifstream	f("test.txt");
	auto			i	= io::make_streamif(std::move(f));
	r	= io::read(buf, i);
	std::cout << std::string_view(buf, r) << std::endl;
}

// OK. 2
{
    // streamifは std::ifstream へのポインタを持つ。
    // stremif が破棄されても ifstream は破棄されない。
	std::ifstream	f("test.txt");
	auto			i	= io::make_streamif(&f);
	r	= io::read(buf, i);
	std::cout << std::string_view(buf, r) << std::endl;
}

// NG. 3
{
    // streamif にはコピーを渡せるかどうかはソース型に依存する。
    // ifstreamはコピー禁止なので、この場合は OK.2 のように
    // ポインタ渡しにする。
	std::ifstream	f("test.txt");
	auto			i	= io::make_streamif(f);
	r	= io::read(buf, i);
	std::cout << std::string_view(buf, r) << std::endl;
}


```


## See also

* [amtrs-io-stream_traits](amtrs-io-stream_traits)
# amtrs::io::stream_traits

```c++
template<
    class Source,
    class Elm
> class stream_traits;
```

Sourceで示すオブジェクトが要素Elmに対してどのようにアクセスを行うのかを定義します。


## Template parameters

|name|note|
|--|--|
|Source|入力元のオブジェクト型。|
|Elm|扱う要素の型。|

## Member types

|Member type|Definition|
|--|--|
|source_type|```Source```|
|char_type|```Elm```|
|value_type|```Elm```|
|fpos_type|```source::fpos_type``` or ```std::fpos<std::char_traits<char_type>::state_type>```|
|size_type|```Source::size_type``` or ```std::streampos```|

## Member functions

|Function|Note|Require|
|--|--|--|
|(constractor)|protected|Require|
|close|protected|Optional|
|read|protected|Optional|
|write|protected|Optional|
|seek|protected|Optional|
|tell|protected|Optional|
|size|protected|Optional|

## constractor

```stream_traits(source_type* source)```

source には streamif が使用しているストリームのポインタが渡されます。必要に応じて初期化に使用して下さい。また、この値は保持して使用しても構いません。

## close

```void close(source_type& source)```

streamif が破棄される際に呼び出されます。source で指定されるストリームデータの開放処理を行ってください。source_typeがデストラクタを持つ場合や明示的な開放が不要の場合はこの関数を実装しなくても構いません。

## read

```streambase::iostat read(source_type& source, size_type& r, char_type* data, size_type size)```

streamif::read() から呼ばれます。ストリームからデータを読み取り、 data へ格納します。もし読み取りをサポートしない場合はこの関数を省略できます。

## write

```streambase::iostat write(source_type& source, size_type& r, char_type const* data, size_type size)```

streamif::wite() から呼ばれます。data のデータをストリームへ格納します。もし書き込みをサポートしない場合はこの関数を省略できます。


## seek, seekg, seekp

```streambase::iostat seek(source_type& source, fpos_type pos, std::ios::seekdir dir)```

ストリームのアクセス位置を変更します。読み込み位置書き込み位置が別々に管理されている場合はseekの替わりにseekg, seekpを宣言することが出来ます。


## tell, tellg, tellp

```fpos_type tell(source_type& source)```

ストリームのアクセス位置を取得します。読み込み位置書き込み位置が別々に管理されている場合はtellの替わりにtellg, tellpを宣言することが出来ます。

## size

```size_type size(source_type& source)```

ストリームの大きさを返すことが可能な場合はそのサイズを返します。分からない場合は宣言しなくても構いません。もしsize()が無くても tell, tellg, tellp, seek, seekg, seekp が宣言されている場合はstreamifがそれらを用いてエミュレーションします。



## See also

* [amtrs-io-stream_traits](amtrs-io-stream_traits)
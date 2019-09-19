# string library

文字列に関するライブラリ。STLがc++専用のライブラリであるのに対し、amtrsライブラリはc/c++で互換のあるプログラムを書くことができるようになっています。c++版のライブラリはc版のライブラリをラップした構造になっているため、コードサイズがSTLに比較して小さくなりやすいという特徴もあります。

## amtrs_string, amtrs::string

amtrs_stringはc言語ベースの文字列オブジェクトです。amtrs_string_で始まる関数を使用することで様々な操作が可能です。amtrs::stringはamtrs_stringをc++向けにラッピングしたクラスライブラリです。

### amtrs::stringとstd::stringの違い

std::stringは文字列が短い場合はメモリを確保せずにstd::string内のバッファを文字列バッファとして利用します。しかしamtrs::stringは常にconst char*のポインタだけ(実際にはamtrs_string::txt)を保持します。そのため、イレギュラーではありますが、以下のようなプログラムを書いた場合c言語のライブラリと互換を持ったプログラムを書くことができます。

```c++
amtrs::string	s = "Hello";
char const* p = *reinterpret_cast<char const**>(&s);
printf("%s", s);   // Hello
```

また、最初からc++17を前提とした設計であるがゆえ、std::stringよりも内部構造がシンプルになっていますのでSTLよりもコードサイズが小さくなりやすいという特徴があります。

## amtrs_string_view, amtrs::string_view

amtrs_string_viewは言わばc言語で使用できるstd::string_viewです。amtrs::string_viewはamtrs_string_viewとstd::string_viewの相互変換を行うためのメタクラスです。


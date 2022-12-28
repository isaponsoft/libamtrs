# c++ vs pasnvm

ここで言うc++はc++20を指します。

psnvm は c++ のサブセットのような位置づけで設計されています。そのため大半の c++ の構文は psnvm でもそのまま使用できます。ただしサブセットですのですべてがそのまま使用できるわけではありません。

また、プリプロセッサの違いについてはプリプロセッサのマニュアルを参考にしてください。

## なぜ上位互換ではないのか

c++の文法解析はかなり複雑です。また、psnvmがスクリプト言語として簡単に書いて実行できるように、あいまいな記述を行えるようになっています。

ただし、psnvmは極力c++のソースコードを流用できることを良しとしていますので今後サポートされる書式も出てくる可能性があります。

## psnvm にのみ存在する機能

### embebed string

c++では文字列リテラルは文字列リテラルでしかなく、配列もしくはポインタです。psnvmでは文字列リテラルは、組み込み文字列型として扱われ、文字列の長さなどの付加情報や数々のメンバ関数を所有します。

### prototype

psnvmでは動的リンクを行うなどの一部特殊な場合を除き、プロトタイプ宣言や前方宣言が不要です。そのためテンプレート引数の解析にいくつかルールがあります。

### string extract

文字列中に ```${}``` で囲まれた範囲を式として展開します。例えば下記 (1) は (2) と同じ結果になります。


```c++
(1) "Hello${1 + 2}world."
(2) "Hello" + (1 + 2) + "world."
```


## c++ と意図的に異なる文法

### typedef

psnvmでは ```typedef``` はサポートされません。代わりに ```using``` を使用してください。

### ポインタの宣言

c++では ```int  *a``` のようにその変数がポインタ（や参照など）は変数名側のサフィックスとして宣言します。しかし psnvm では ```int*   a```のように型側に記述します（ただし空白は無視されるので事実上同じ結果になります）。

ただし、以下のような宣言があった場合は異なる結果になります。

```c++
int*	a, b;
```

c++では```a```はポインタですが```b```は単なる```int```です。しかしpsnvmでは```a```も```b```もポインタになります。psnvmではc++で書くと以下のような判定と等しくなります。また次の書き方はpsnvmでも同じ結果になります。

```c++
using	int_ptr	= int*;
int_ptr	a, b;
```

### function pointer

関数ポインタの型を宣言するとき c++ と psnvm では書式が異なります。下記の(1)はc++の書き方です。(2)はpsnvmでの書き方になります。ただしpsnvmでは(1)の書き方も互換のためにサポートしています。

```c++
(1)	using	function_ptr	= void(*)(int, short);
(2)	using	function_ptr	= void(int, short)*;
```

### sepecifier

c++では static や volatile などの修飾子は型名の前後に自由に記述することができます。しかしpsnvmでは型名の前に記述する必要があります。



```c++
(1) static const inline char* volatile f();  // c++ only
(2) static inline volatile const char* f();  // compatible
(3) static inline volatile char const* f();  // compatible
```

### conversion function

変換関数に指定できる型の自由度が高くなっています。

c++では関数ポインタ型に変換する関数を書く場合、using でエイリアスを使う必要があります。

```c++
(1) using	func_type		= int();
    using	func_ptr_type	= func_type*;
    operator func_ptr_type ();
(2) using	func_ptr_type	= int(*)();
    operator func_ptr_type ();
```

psnvm ではc++の書き方に加えて直接書くことができます。

```c++
(1) operator int()* ();
(2) operator int(*)() ();
```

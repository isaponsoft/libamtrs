# passionとc/c++との違い

passionはc/c++とある程度互換性のある書式になるように設計されています。一部の構文にさえ気を付ければpassionとc++でコンパイル可能なソースコードを書くことも可能で相互に再利用することができます。
しかし、伝統的な書式の中でも混乱しやすいものや直感的ではない書式(または構文解析しにくい書式)は意図的に排除しています。

|                | c                      | c++                                                          | passion                                                      |
| -------------- | :--------------------- | ------------------------------------------------------------ | :----------------------------------------------------------- |
| 変数のポインタ | int   *a, *b;<br />    | using  intptr = int*;<br />intptr a, b;                      | int* a;<br />int* b;                                         |
| 関数のポインタ | int (*a)(char, short); | uinsg  ftype = int(char, short);<br />ftype* a;              | int(char, short)* a;                                         |
| classとstruct  |                        | 同じもの(*1)                                                 | class は参照カウンタにより寿命管理されnewによって確保する必要がある。<br />structはc/c++と同じ扱い。 |
| int型          | 処理系に依存           | 処理系に依存                                                 | void*と同サイズ(intmax_tと同じ)                              |
| short,long型   | 処理系に依存           | 処理系に依存。<br />サイズを保証したい場合は代わりに int8_t, int16_t, int32_t, int64_t を用いる。 | 使用できない。<br />代わりに int8_t, int16_t, int32_t, int64_t を用いる。 |
| 前方宣言       | 必要                   | 必要                                                         | 一部を除き不要                                               |

*1) デフォルトのアクセス修飾子の違いのみ

## 文字列

passionでは文字列を組み込み型として扱います。そのためc/c++に比べて文字列の扱いがいくらか楽になっています。

```cpp
string a = "Hello ";
string b = a + "world.";
printf(b + "\n");  // Hello world.
```

また文字列中に "${}" を書くとその中に書いた式を展開し文字列に埋め込みます。

```cpp
string a = "Hello";
string b = "world";
printf("${a} ${b}.\n"); // Hello world.
```

関数や、その他の式も利用できます。

```cpp
int one() { return 1; }
printf("${one() + 1} = 2");  // 1 + 1 = 2
```



## ポインタの宣言

c/c++では変数の前に*を付けることでその変数がポインタになります。
つまり、下の例では変数 a は int*型であり、 b は int型です。


```cpp
int		*a, b;
```

しかし、typedefやusingを使った場合は少し異なり、下の例ではa,b共にint*型です。

```cpp
typedef	int	*intptr;		// c++なら using intptr = int*; でもよい
intptr	a, b;
```

passionではポインタは「型名の後ろにつける」というルールになっているため、下のような書き方はa,b共にポインタとなります。

```cpp
int		*a, b;
```
しかし、c/c++との微妙な違いが混乱を招くため以下のように複数行に分けなくてはなりません。

```cpp
int*		a;
int*		b;
```



## 関数ポインタの宣言

ポインタの宣言が型側に依存するため関数ポインタの宣言方法も変わってきます。c/c++では関数ポインタは以下のように宣言します。

```cpp
int (*variable) (int, int, int);
```

c++では using を用いると passion と同様の書き方が可能です(この書き方はpassionでも有効なのでポータビリティのあるプログラムを書くならこの書き方を推奨します)。

```cpp
using	type	= int(int, int, int);
type*	variable;
```

passionでは型の最後に*を付与するため、using を用いらなくても次のように書くことができます。


```cpp
int(int, int, int)* variable;
```

## 前方宣言(プロトタイプ)

passionは２パスコンパイルを行うためpassionでは特定の条件を除き前方宣言を必要としません。例えば下のプログラムはc++だとエラーになりますがpassionでは正しく解析されます。

```cpp
int main()
{
    return function1("Hello World");
}

int function1(string s)
{
    printf("${s}\n");
    return 0;
}
```

passionは１パス目でソースコードのすべての「クラス」「関数」「変数」の定義の解析を行います。その後の２パス目で関数内の書式の解析を行います。このうちプロトタイプが必要になるのは１パス目の解析時です。つまりプロトタイプが必要になるケースは次の通りです。以下のプログラムはプロトタイプがない場合エラーになります。

```cpp
class MyClass;   // プロトタイプ宣言。

// 関数の引数に MyClass^ を指定している。
// プロトタイプ宣言があるため MyClassが何を示しているのかコンパイラが判定できる。
// 仮にMyClassのプロトタイプ宣言がない場合は構文エラーとなる。
int test(MyClass^ c)
{
    return 0;
}

class YouClass
{
    // クラスのメンバ変数にMyClassを指定している。
    // これもプロタイプ宣言があるので問題ない。
    MyClass^	m;
};

// MyClassの実態の定義
class MyClass {};
```

もちろん、プロトタイプ宣言をせずに先に実態を定義しても構いません。

```cpp
class MyClass {};

int test(MyClass^ c)
{
    return 0;
}

class YouClass
{
    MyClass^	m;
};
```



### gcポインタ

ポインタとして "*" ではなく "^" を使うと参照カウンタを用いたオブジェクトの寿命の自動管理を行います（これはC++/CLIに似ています）。次のように書きます。下の例ではcが削除されるとMyClassのインスタンスが持つ参照カウンタを減少させ参照がなくなったのを確認するとその場でデストラクタを呼び出しインスタンスを削除します。

ガベージコレクションをもつ言語（JavaやC#やgoなど）では参照が外れたオブジェクトは即時に開放はされずメモリが不足した際にまとめて解放処理が行われるのに対し、passionでは参照が外れたオブジェクトは即座にメモリが解放されます。そのためプログラマがいつオブジェクトが削除されるのかを予測しコントロールすることができます。

```cpp
class MyClass
{
    ~MyClass() { printf("Delete\n"); }
};

void main()
{
    f();
    printf("main end");
}

void f()
{
  MyClass^ c = new MyClass();
}
```

上記のサンプルコードの実行結果は次のようになります。

```shell
Delete
main end
```

## classの内部実装

passionのclassをc++のclassで表すなら、passionのclassは必ず参照カウンタを管理するReferenceCounterクラスを仮想継承したのと近い構造になります。

```cpp
// passion上での宣言。
class BaseClass1{};
class BaseClass2{};
class MyClass : public BaseClass1, public BaseClass2 {};
```

```cpp
// c++に置き換えてみた場合のイメージ。すべてのクラスは必ず ReferenceCounterクラスを仮想継承している。
struct ReferenceCounter
{
    virtual ~ReferenceCounter() {}
    std::atomic<int>  _counter;
};
class BaseClass1 : virtual public ReferenceCounter {};
class BaseClass2 : virtual public ReferenceCounter {};
class MyClass
     : virtual public ReferenceCounter
     , public BaseClass1
     , public BaseClass2 {};
```

## プリプロセッサ

passionのプリプロセッサは良くも悪くもc++のプリプロセッサよりも複雑な事ができます。例えば define の中にdefineを書くことができます。

```cpp
#define DEF_MACRO(_name, _number) #define _name _number
DEF_MACRO(m1, 1) // #define m1 1
DEF_MACRO(m2, 2) // #define m2 2
```


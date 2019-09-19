#  passion文法

基本的には多くの文法はc/c++を参考・準拠しています。

# if式

式の結果によって処理を分けたいときに使用する式です。

```cpp
if (CONDITIONS) { TRUE-STATEMENT }
if (CONDITIONS) { TRUE-STATEMENT } else { FALSE-STATEMENT }
```

if文はCONDIRIONSに書かれた式が true になった場合に TRUE-STATAMENT を実行するように結果を生成します。else {} を書くことでCONDITIONSの結果がfalseだった時に実行すべき内容を記載できます。また、次のようにelse if を書くと複数の条件を書くことができます。

```cpp
if (CONDITIONS) { TRUE-STATEMENT } else if (CONDITIONS2) { TRUE-STATEMENT2 } else if (CONDITIONS3) { ... 略
```

c++14以降で採用されたif文の初期化式にも対応しています。";" を書くことで初期化式をまとめて記載できます。

```cpp
if (INIT-STATEMENT ; CONDITIONS) { TRUE-STATEMENT } 以下略
```

ifはc/c++と異なり分ではなく式として評価します。そのため、式の右辺値として評価できます。右辺値として結果を返したい場合はanser文を用います。次の例は参考演算子をif式を用いて書いたものです。

```c++
// a = (x == 1 ? true : false) と同等。
bool a = if (x == 1) {
    		answer true;
		} else {
    		answer false;
		};
```

```c++
// このように文を混在してもよい。
bool a = if (x == 0) {
    		answer false;
		} else {
    		for (int i = 0; i < x; ++i) {
                if (table[i] == 1) {
                    answer true;
                }
            }
    		answer false;
		};
```

```c++
// x が 0 以外の場合の解答がないのでエラー
int a = if (x == 0) { answer 1; }

// 解答が存在しない場合があるのでエラー
int b = if (x == 0) {
    		answer false;
		} else {
    		for (int i = 0; i < x; ++i) {
                if (table[i] == 1) {
                    answer true;
                }
            }
    		// ここに来る可能性がある
		};

// 必ず回答が得られるのでこれはＯＫ。
// ただしこのプログラムはバッファーオーバーランの可能性を持っている。
int idx = for (int i = 0; ++i) {
  		 	 if (table[i]) { answer i; }
		};
```





### c++との違い

c++ではif分の後ろに書く実行したい内容に {} を省略することができますがpassionでは{}の省略は許されていません。

```cpp
if (get_status() > 100) printf("r > 100\n");   // c++ では有効だが passion では構文エラーになる。
if (get_status() > 100) { printf("r > 100\n"); } // c++でもpassionでも有効。
```

### 例文

```cpp
if (int r = get_status(); r > 100)
{
    printf("r > 100\n");
}
else
{
    printf("r <= 100\n");
}
```

# forループ(for loop)

```c++
for (INIT-STATEMENT; CONDITIONS; ITERATION) { LOOP-STATEMENT }
```

# 範囲ベースのforループ(ranged base for loop)

```c++
for (DECLARATION : RANGE) { LOOP-STATEMENT }
for (INIT-STATEMENT; DECLARATION : RANGE) { LOOP-STATEMENT }
```

# whileループ(while loop)

```c++
while (CONDITIONS) { LOOP-STATEMENT }
while (INIT-STATEMRNT; CONDITIONS) { LOOP-STATEMENT }
```

# do-whileループ(do-while loop)

```c++
do { LOOP-STATEMTNT } while (CONDITIONS);
do (INIT-STATEMENT) { LOOP-STATEMTNT } while (CONDITIONS);
```

2つめの書式は以下の書き方と同じです。do-whileの中でだけ使用したい作業用変数の宣言に向いています。

```c++
{
    INIT-STATEMENT;
    do
    {
        LOOP-STATEMENT
    } while (CONDITIONS);
}
```

# 文式(statement expression)

if文やswitch文などの選択文は式として評価することができます。

```c++


string b = switch (y) {
    case 1 : answer "Hello";
	case 2 : answer "World";
    default: answer "Unkown";
};
```

式としての選択文を使う場合は値を返さない経路があってはなりません。そのため条件式を伴う繰り返し文を使用することができません。繰り返し文を使用したい場合は次のようにif文などと組み合わせて使用します。

```c++
int c = if (1) {
    		for (int i = 0; i < values.size(); ++i) {
    			answer i;
			};
    	     answer -1;
        };
```

条件式が存在しないもしくは定数の場合、ループを抜けることがない保証がある場合は次のように書くこともできます。

```c++
// for を抜けることはない
int c = for (int i = 0; ; ++i) {
    	if (--x == 0) { return i; }
	};
```





ラムダ式を使うと以下のようになります。

```c++
bool a = [](int _x) {
    		if (_x == 1) {
                return true;
            } else {
                return	false;
            }
		}(x);

// キャプチャを使う場合
bool a = [&]() {
    		if (x == 1) {
                return	true;
            } else {
                return	false;
            }
		}();

```





# ジャンプ文

ジャンプ文はフロー制御を無条件に転送します。

## break

## continue

## return

## answer

answer文は少々特殊です。選択文や繰り返し文は、文そのものを右辺値にすることが可能です。answer文は右辺値となった文の結果を左辺値に返すための文です。

```c++
bool a = if (user_input == "OK") {
			answer true;
		} else {
			answer false;
    	};
```

```c++
string a =	switch (v) {
    case 1 : answer "Hello";
    case 2 : answer "World";
};
```







## ポインタ

### c++との違い

c++では "*" を変数の前に書くことでその変数がポインタになります。しかしpassionでは型の後ろに "*" を書くことでポインタになります。これが大きく影響するのは関数ポインタです。

```cpp
int    *a;   // c/c++
int*    a;   // passion

int (*b)(MyClass*);  // c/c++
int(MyClass*)* b;    // passion

// 次の書き方はc++とpassionの両方で使用可能。
using type = int(MyClass*);
type*  c;
```



## GCポインタ

passionでは参照カウンタによるガベージコレクションを実装しています。ポインタを示す記号に "*" の代わりに "^" を使うことでGCポインタとなります。GCポインタの場合はメンバへのアクセスはアロー演算子（->）ではなくどっと演算子（.）を使用します。

```cpp
class MyClass
{
public:
    int f1() { return 1; }
};

MyClass^ mc = new MyClass();
mc.f1();
```

GCポインタの場合は operator のオーバーロードが使用できます。

```cpp
class MyClass
{
public:
    int operator + (int a);
};

MyClass^ mc = new MyClass();
int r = mc + 1;   // r == 2
```

ただしoperator = は使用できません。

```cpp
class MyClass
{
public:
    MyClass^ operator = (MyClass^);   // NG
};

// 代入はあくまでも変数への代入として扱われる。
MyClass^ mc = new MyClass();
mc = new MyClass();  // mc の内容が書き換わる
```

ポインタ型へのキャストが可能です。ただしその逆は許可されていません。

```cpp
MyClass^ rm = new MyClass();
MyClass* pm = rm;               // OK
MyClass^ rm2 = pm;              // NG ポインタは参照カウンタを無視している状態のため、ポインタからGCポインタへ代入すると参照カウントの整合性が合わなくなるケースが存在するため禁止している。
```

## 配列

型名の後ろに[]を書くと配列になります。配列は内部的にはGCポインタとして扱われるため参照が複数ある場合は内容が保持されます。

```cpp
int[]  a = new int[100];
int[][]  a = new int[100][100];
int[][][]  a = new int[100][100][100];
```

配列はいくつか関数を持っています。class文のように書くと次のようになります。

```cpp
template<class T>
class T[]
{
public:
	int size() const noexcept; // 配列のサイズを返す。
	T* begin() noexcept;       // 配列の先頭アドレスを返す。
	T* end() noexcept;         // 配列の終了アドレスを返す（無効な位置を指しているので注意）。
	T[] operator + (T[] b);    // 連結した配列を新規に作成する。
};
```

# 型

## アドレス型（またはポインタ）

コンピュータで扱う数値や文字列などのデータと呼ばれるものは基本的にはコンピュータという機械に搭載されているメインメモリに保存されています。そしてメインメモリのどこに保存しているか、つまりはデータの場所を示す言葉が「アドレス（住所）」です。アドレス型とはつまりデータがある場所を示す値ということになります。

### アドレス型の書き方

型名の後ろに * を書くとアドレス型という型になります。例えば、以下のように書きます（※c/c++とは異なり * を型の方に書くので注意してください。

| 型            | アドレス型 |
| ------------- | ---------- |
| int           | int*       |
| class MyClass | MyClass*   |

データのアドレスを取得するには & を付けます。次のサンプルはint型の変数aのアドレスをbに保存しています。これにより変数bには「aがある場所を示す数値」が入ります。

```cpp
int   a;   // int型の値を保存できる変数。つまりint型の変数。
int*  b;   // int型へのアドレスを保存できる変数。つまりintアドレス型の変数。
b = &a;    // 変数aのアドレスを取得し、変数bに保存する。
```

アドレス型の変数が示している場所のデータを読んだり書いたりする場合はアドレス型の変数の前に * を書きます。

```cpp
int   c;
c = *b;    // b が示す場所のデータを読み取る。つまり a の中身を読み取ることができる。
*b = c;    // 書き込む場合も同じ。
```

### c/c++のポインタ

c/c++のポインタの場合、ポインタかどうかであるかについては基本的には変数側に委ねます。例えば以下のプログラムがわかりやすい例となります。以下の例では「変数 a は int型」で「変数 b は int型へのポインタ」となります。ポインタ記号(*)があくまでも、型ではなく変数側に対する修飾子であることに由来します。

```c
int		a, *b;
```

つまり

```c
int*     a, b;
```

と書いているつもりでも

```c
int      *a, b;
```

となっているわけです(typedefを使うとまた事情が異なってきますがここでは省略します)。

## 関数ポインタ

| 内容                           | 書式                                                         |
| ------------------------------ | ------------------------------------------------------------ |
| 関数の定義                     | int function_name(string) { return 0; }                      |
| 関数型                         | int(string)                                                  |
| 関数参照                       | int(string)&   function_ref = function_name;                 |
| 関数参照での呼び出し           | function_ref("Hello")                                        |
| 関数ポインタ                   | int(string)*    function_ptr = &function_name;               |
| 関数ポインタでの呼び出し       | (*function_ptr)("Hello")                                     |
| メンバ関数の定義               | int Class::function_name(string) { retutn 0; }               |
| メンバ関数型                   | int Class::(string)                                          |
| メンバ関数型参照               | int Class::(string)&  function_ref = Class::function_name;   |
| メンバ関数型参照での呼び出し   | (class_ptr->*function_ref)("Hello")                          |
| メンバ関数ポインタ             | int Class::(string)*   function_ptr = &Class::function_name; |
| メンバ関数ポインタでの呼び出し | class_ref.function_ref("Hello")                              |





## c/c++との違い

passionの関数ポインタはc/c++の関数ポインタの書き方と少しだけ異なります。

|                    | c/c++                  | passion              |
| ------------------ | ---------------------- | -------------------- |
| 関数               | int(string)            | int(string)          |
| 関数ポインタ       | int(*)(string)         | int(string)*         |
| メンバ関数         | 該当なし               | int Class::(string)  |
| メンバ関数ポインタ | int (Class::*)(string) | int Class::(string)* |

この違いに異論を上げるc/c++プログラマーは多いと思います（実際私もこれには書きなれたc/c++と異なることに対しての不満を覚えますが、それでも敢えてオリジナルの文法を取り入れました）。

しかしc/c++で次のような使い方を想定してみてください。以下の例ではfunc_typeに関数の型を記載し、func_pointer_typeはfunc_typeのポインタ型、つまり関数ポインタになります。この時、慣れた上級者以外（いいえ上級者ですらも）見た目上 func_pointer_type = int(string)* であるような錯覚を覚えます。

```cpp
using	func_type           = int(string);
using	func_pointer_type   = func_type*;
```

### 関数ポインタを使った関数の呼び出し

関数ポインタを使った関数の呼び出しはc/++とほとんど変わりません。

|                    | c/c++              | passion            |
| ------------------ | ------------------ | ------------------ |
| 関数ポインタ       | (*fp)("hello")     | (*fp)("hello")     |
| メンバ関数ポインタ | (cp->*fp)("hello") | (cp->*fp)("Hello") |


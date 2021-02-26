# プリジェネレータ

## コンセプト

プリジェネレータはc/c++のプリプロセッサに似ていますが、その哲学は大きく異なります。プリジェネレータはその名の通り、ソースコードのコンパイルより先に実行される「ジェネレータ」です。これにより複雑なビルドシステムを構築しなくてもpassion単体でソースコードジェネレータを作ることができます。

## psnvmコマンド

psnvmコマンドはpassionのソースコードをコンパイル、実行することができます。また、プリジェネレータを単体で実行することも可能です。

```shell
# code.psn に対してプリジェネレータのみを実行します。
$ psvm -E code.psn
```

```shell
# code.psn に対してプリジェネレータを実行し、シェルで実行します。
$ psnvm --system -E code.psn
```







## C/C++との違い

一般的にプリプロセッサを過度に使用することはソースコードの可読性を失う悪しき文化だと言われています。しかし、passionのプリプロセッサはc/c++のプリプロセッサよりも機能も多く強力かつ複雑です。

しかし２０２０年現在のプログラムはプリプロセッサが貧弱であるがために、ビルドシステムを複雑怪奇にしてしまっているように感じています。それであるなら、いっそプリプロセッサを強力にすべきというのがpassionの哲学です。

### 再帰的な展開の許可

passionのプリプロセッサはこれを再帰的に展開しようとします。そのため、以下のマクロは永久ループとなり、結果ネスト震度が深すぎて失敗します。

```c++
#define TEST	TEST
TEST
```

### マクロのオーバーロード

引数が異なるマクロをオーバーロードできます。

```c++
#define	M		m1
#define	M(a)	m2
#deifne	M(a,b)	m3
```

### undef

同名のオーバーロードをすべて削除します。

### ifdef

同名のマクロが一つでもあれば true になります。

### if

## 基本的な書き方

```c++
#ディレクティブ 内容
#ディレクティブ(内容1;内容2;内容3)
-#任意
%{式}
```

### 1 の書き方

```c++
#define  NAME   VALUE
```

よく見るプリプロセッサの書式です。

### 2の書き方

```c++
#foreach(I V ; "," ; one,two,three)
```

foreachなど複数のパラメータを必要とする場合の書き方です。パラメータを ; で区切って指定します。

### 3の書き方

```c++
#-define NAME VALUE
```

-#(ハイフン・シャープ)を書くとジェネレータは#以降のテキストをプリプロセスし単純にハイフンを取り除きます。c/c++のソースコードを生成するときなどに向いています。

```c++
#define M1  test
-#define  M1  "hello"
------
#define test  "hello"
```

### 4の書き方

${}の中を式とみなし、即座に計算して結果に置き換えます。

```c++
#define M1  1
#define M2  2
value=%{(M1) + (M2) + (defined(M3) ? M3 : 0)}
--------
value=3
```

## マクロ名

マクロ名はc/c++のものに加えて2文字目以降に .(ピリオド) を含むことができます。つまり、以下の名前はpassionでは有効です。

```c++
#define	MyApp.Setting.Dirname	/usr/local/etc/myapp/
```



## Directive

### define

```c++
#define	NAME	text
#define	NMAE()	text
#define	NMAE(a)	text
#define	NMAE(a, b, c)	text
```

マクロを定義します。c/c++と異なり引数の数によるオーバーロードが可能です。また、以下のようにマクロの中にプリプロセッサ命令を記載することも可能です。

```c++
#define	NAME(name, value)	#define name value
```

### undef

```c++
#undef	NAME
```

NAMEで指定されるマクロをすべて削除します。同名のマクロがない場合はエラーになります。

### include

```c++
#include "file"
#include <file>
```

passionのプリプロセッサにおけるincludeのファイルの検索方法はc/c++のものとは多少異なります。ダブルコーテーションで囲まれたファイルは常にinclueを記載したファイルからの相対パスでのみ検索します。<>で囲まれたファイルは INCLUDEPATH から検索を行います。

### env

```c++
#env(ENVNAME)
```

```c++
#define	_M(v)	#v
#define	M(v)	_M(v)
string homedir = M(#env(HOME));
```

```c++
string homedir = "/home/user";
```

### foreach

```c++
#foreach(COUNT, VALUE, SPLIT)	VALUES
#endif
```

VALUESの内容をSPLITで分割し、繰り返し処理を行います。COUNTで指定する名前にループ回数、VALUEには値が順次格納されます。

```c++
#foreach(I, V, ",")		"One","Two","Three"
  #if (I+1) % 2 == 0
even V
  #else
odd V
  #endif
#endif
==== OUTPUT
odd "One"
even "Two"
odd "Three"
```

### invoke

```c++
#invoke(RETURN,OUTPUT)	COMMAND
```

プリプロセッサに内蔵されている（プラグインのような）拡張された機能を呼び出します。戻り値がRETURNに出力結果がOUTPUTに格納されます。

### exec

```c++
#exec(ls -al)
```

### error

### format

```c++
#format(OUT; FORMAT; MSG, CODE ) 
```

```c++
#define MSG  file not found
#define CODE 1003
#format(OUT ; "error!! code=%2, msg=%1" ; MSG CODE)
==== OUTPUT
error!! code=1003, msg=file not found
```

### update

```c++
#update "FILENAME" "TEMPLATE"
```

TEMPLATEで指定するファイルを読み込み、プリプロセスした結果と FILENAME で指定したファイルの中身を比較し、異なれば上書を行います。

```c++
=== file1.psn ===
#define USERNAME #env(USER)
#define UPDATEFILE(name) MAKE_STR(#env(PWD) ## name)
#update UPDATEFILE(build.log) "build.log.template"
=== build.log.template ===
build user is #USERNAME
```

## 特殊記号

### 式展開

%{式} という塊が現れるとプリプロセッサは式展開しようとします。

```c++
#define MACRO1  "Hello world"
%{MACRO1}
#define MACRO2  1 + 1
%{MACRO2}
-------
"Hello world"
2
```

次のように ?: （条件演算子）を使用するとマクロ未定義時に代理テキストを使用できます。

```c++
#undef MACRO1
value=%{defined(MACRO1) ? MACRO1 : Nothing}
-------
value=Nothing
```







## 文字列

```c++
string = "TEXT"
string = R"DELIM(TEXT)DELIM"
```

### ヒアドキュメント

```c++
string = DOC"DELIM(
   TEXT1
   TEXT2
   )DELIM";
```

Rの代わりにDOCから始めるとヒアドキュメントとして扱われます。RAW文字列に似ていますが、ヒアドキュメントはDOC"DELIM(の直後にある改行と)DELIM"の直前にある改行は無視します。)DELIM"の前に空白またはタブがある場合、それをインデントとみなし、インデントを持つすべての行から削除します。つまり上記のヒアドキュメントは同じ値になります。

```c++
string = "TEXT1\nTEXT2";
```

## 式

ifなどで使用できる式とその中で使用できる関数群です。

### defined

```c++
defined(NAME)
```

NAMEで指定されるマクロがある場合 1、ない場合は 0 を返します。

### trim

```c++
trim("  Hello world  ")
--------
"Hello world"
```

前後についている空白、改行を取り除きます。

### format

文字列をフォーマットします。

```c++
format("%{2}, %{1} is %{3}, %{4:03}", "A", "B", "Test", 1)
--------
B, A is Test, 001
```





## PPS

ppsとはコード中に <?pps ?> で囲むことによってスクリプトライクなプリプロセス処理を行うことができる特殊なタグです。

```c++
int nums[] = {<?pps if (DEBUG) {
    for (i = 0; i < 10; ++i) {
        echo "${i},";
    }
?>10};
```

```c++
int nums[] = {0,1,2,3,4,5,6,7,8,9,10};
```

### ppsとマクロの優先順位

プリプロセッサは <?pps ?>を見つけるとまずその部分を抽出します。次に抽出した部分に対してマクロの置換処理を行います。

つまり以下コードは

```c++
#define DEBUG	999
#define DEBUG2	debug2!!
<?pps if (DEBUG) { echo "DEBUG2"; } ?>DEBUG
```

まず <?pps ?> の内側だけ置換を行います。つまりDEBUG識別子がマクロで置き換えられます。

```c++
<?pps if (999) { echo "DEBUG2"; } ?>DEBUG
```

次に<?pps ?>の内部を実行します。echo "DEBUG2" に従って DEBUG2 を出力します。この時注意すべき点として、<?pps ?>で出力された部分はその部分単体でプリプロセッサにかけられることです。つまり、以下のような出力になります。

```c++
DEBUG2 DEBUG
```

DEBUG2がプリプロセス処理にかけられ次のような出力になります。

```c++
debug2!! DEBUG
```

すなわち結果は以下のようになります。

```c++
debug2! 999
```













### calc

```c++
int  a = <?pps= calc(1 + 2) ?>
```

```c++
int  a = 3;
```

### env

```c++
string username = "<?pps env(ENVNAME) ?>";
```

()の中で囲まれた環境変数に置換します。これはソースコードの更新日時を変更せずにビルドした環境をバージョン情報やデバッグ情報に含めたい時などに便利です。文字列化したい場合は次のように書きます。

### foreach - endfor

```c++
<?pps foreach(iterator : values...) {?>
<?pps } ?>
```

iteratorで示すマクロ名に valuesの値を入れながらループ展開します。

```c++
<?pps foreach(PERM : "INTERNET" "ACCESS_NETWORK_STATE") { ?>
android.permission.PERM = #calc(PERM_foreach_count + 1)
<?pps } ?>
```

```c++
android.permission.INTERNET = 1
android.permission.ACCESS_NETWORK_STATE = 2    
```



## コンパイル時実行(compiling execution)

### プリプロセッサとの違い

CEはプリプロセッサよりはるかに強力で複雑なこともできます。ただし、CEはあくまでもpassionの言語機能のひとつであり、passionのプログラムをコンパイルしているときにしか効果を発揮しません。プリプロセッサはCEに比べると貧弱ですが、反面プリプロセッサを単体で実行することもできるため、他のプログラム言語のプログラミングに混ぜて使用することもできます。

### 書き方

<?pps ?> で囲まれた部分は通常のプログラムのように扱いますが、プリプロセッサ処理中に実行されます。例えば以下のように書くことができます。

```c++
string get_builddate()
{
    return std::to_string(<?pps print("\"${time::now().to_string("Y/m/d")}\""); ?>);
}
```

以下のように展開されます。

```c++
string get_builddate()
{
    return std::to_string("2020/4/5");
}
```

プリプロセッサの#ifの代わりに使うこともできます。

```c++
<?pps if (DEBUG) { ?>
void debug_printf(string txt);
<?pps } ?>
```




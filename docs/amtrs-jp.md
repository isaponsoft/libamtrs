# libAmtrs

libAmtrs(アマテラス)は様々な機能を持ったc++17（それ以降）向けのクロスプラットフォームライブラリ集です。libAmtrsには他にもpassionやinariKVS等のプロダクトも含まれていますが、ここでは特に記載はない限りはクロスプラットフォームライブラリ部分の説明となります。

## 名前の由来

libAmtrs(天照)は特定の用途だけではなく様々な機能を持ったライブラリ群です。プログラムの世界では多くの機能を持ってしまったクラスを皮肉を込めてGodClass（ゴッドクラス）などと呼びますがlibAmtrsはそうしたゴッドクラスに近い側面もあり日本神道における最高神の名前をお借りしています。特に宗教的・信仰的な意味合いはありません。

※日本神道の神様はキリスト教など一神教におけるＧＯＤ（神）よりもむしろＳｐｉｒｉｔｓ（精霊）に近いとも言えますので、こうした側面からもlibAmtrsは神でありＧＯＤではないというちょっとしたジョークを含めているつもりです。※プログラマーの世界はあまり思想や宗教などを持ち込む人は多くはありませんが、だからこそこうした宗教的な名前をあえて持ち込んでみました。

# Start

c++のソースコードからlibamtrs/include/amtrsにあるヘッダファイルをincludeしてください。基本的にはこれだけで利用できます。ただし、AndroidやiOSで使用する場合は次の設定が必要です。

## Android

Android向けの開発を行う場合、どうしても一部Java（もしくはKotlin）で書かれたソースコードが必要となります。こうしたソースコードはlibamtrs/src/androidの中に含まれていますので、build.gradleのjava.srcにlibamtrs/src/androidを追加してください。

```groovy
sourceSets {
	main {
		java {
			srcDirs	= [ 'src/main/java', '../../libs/libamtrs/src/android' ]
		}
	}
}
```

## iOS

### プロジェクトの設定

iOSの開発を行う場合c++からObjective-c++の書式を呼び出せるようにするといろいろと捗ります。そこでプロジェクトの設定からcplusplus flags に -x objective-c++ というフラグを追加してください。これによりc++のソースコードをObjective-c++のソースコードとして解釈できるようになります。

```xml
OTHER_CPLUSPLUSFLAGS = (
	"$(OTHER_CFLAGS)",
	"-x",
	"objective-c++",
	"-fexceptions",
);
```

### ソースコードの追加

Amtrsは基本的にincludeすれば使えるものとなっていますがObjective-cの仕様上includeだけで完結するとプログラムのリンク時にシンボルが競合してしまう部分が出てきます。そうしたプログラムはlibamtrs/src/iosディレクトリ内にcppファイルとして分離してありますので、この中のソースコードをプロジェクトに追加して下さい。

## そのほかのＯＳ

WindowsやLinuxなどc++のみで完結できるプラットフォームでは面倒な設定などは不要です。

# HelloWorld

ではAmtrsを実際に使用しています。まずはAmtrsと何にも関係のないHelloWorldを作ってみます。

```c++
#include <iostream>

int main(int, char**)
{
    std::cout << "Hello world" << std::endl;
    return	0;
}
```

次にAmtrsをincludeしてみます。コンパイル時にlibamtrs/includeをINCLUDE_PATHに追加しているものとします。

```c++
#include <iostream>
#include <amtrs/amtrs.hpp>

int main(int, char**)
{
    std::cout << "Hello world" << std::endl;
    return	0;
}
```

特に何も変わらずコンパイルし、実行できると思います。さて、大切なことなのですがAmtrsは原則として「使用しない限りはオーバーヘッドを作らない」というコンセプトを掲げています）ちなみにこのコンセプトはc++そのものの哲学でもあったりします）。

つまりヘッダファイルをincludeだけしても何も起きない。何も邪魔しない。ということです。これは日本のことわざで「触らぬ神（精霊）に祟りなし」という言葉がありますが、ただincludeしただけで実際に呼び出さない限りはなにもしないのです。

## 自動寿命管理

Amtrsにはref_objectというクラスがあり、ほとんどの機能はこのref_objectに依存しています。このクラスはrefクラスとセットで使用することでオブジェクトの寿命管理を自動で行います。以下のようにref_objectを継承したオブジェクトのポインタをrefに代入するとrefが破棄されたときに自動的にオブジェクトを削除します。

```c++
struct MyClass : public amtrs::ref_object
{
	~MyClass() { std::cout << "destroy" << std::endl; }
};

int main()
{
	amtrs::ref<MyClass> m = new MyClass();
    std::cout << "main end." << std::endl;
	return	0;
}
```

## 参照カウンタ

```c++
ref<MyClass>	a = new MyClass();
ref<MyClass>	b = a;
a = nullptr;	// bからも参照されているので MyClass() は破棄されない。
b = nullptr;    // MyClassを参照するものがなくなったので、ここで破棄される。 
```



# 基本設計

Amtrsの中にはいくつかの機能がカテゴリーごとに分けて作られています。例えばグラフィックス向けのライブラリ群はamtrs/graphicsディレクトリに、サウンド関係はamtrs/soundディレクトリに格納されています。ここでは比較的小さなsoundを例にとりますがディレクトリの中身は凡そ次のような構成になっています。

## @ファイル

ディレクトリ内のすべての機能（ただし一部例外あり）をまとめてincludeします。例えばsound機能を使う場合はsound/@をincludeすればよいので、以下のようなコードを挿入します。

```c++
#include <amtrs/sound/@>
```

## "-プラットフォーム" ファイル

soundを例にとると sound_device.hpp のほかに sound_device-android.hpp や sound_device-win32.hpp のようにプラットフォーム名がついたファイルが存在することがあります。これは同じsound_deviceでもプラットフォームによって呼び出すＡＰＩやＳＤＫが異なっていたり使用するヘッダファイルが異なる場合に用意されたもので、プラットフォーム固有の機能が分離されて書かれています（ラッパーライブラリ、またはドライバーと呼んでいます）。

しかしユーザーはこれらを気にする必要はなく、単にsound_device.hppをincludeすでば必要に応じてラッパーやドライバーをamtrsが選択してincludeします。したがって、次のように書くだけで構いません（もちろんsound/@をincludeするだけでもＯＫです）。

```c++
#include <sound/sound_device.hpp>
```

# ライブラリ

| 名前     | 機能                                                         |
| -------- | ------------------------------------------------------------ |
| sound    | サウンド出力・フォーマット                                   |
| node     | ノードのウィンドウシステム。ゲーム開発やグラフィカルなアプリ開発に力を発揮します。 |
| g3d      | ３Ｄ描画システム。内部でOpenGLやVulkan,MetalなどのＡＰＩを呼び出します。 |
| graphics | グラフィックス計算のためのライブラリ。ベクトルやマトリックス、クォータニオンを扱います。 |
| crypto   | 主に暗号化やそれに類する目的で使用するライブラリです。       |
|          |                                                              |


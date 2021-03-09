# テストフレームワーク

libamtrsにはシンプルな単体テストフレームワークである tests.hpp があります。

高性能ではありませんが、とても単純です。


単体テストを行うにはまず、以下のような main関数を用意します。このプログラムは単にlibamtrsの単体テストフレームワークを呼び出すためだけのものです。

```c++
#include <amtrs/tests.hpp>
int main(int _argc, char** _args)
{
	return	AMTRS_TEST_NAMESPACE::tests(_argc, _args);
}
```

次に以下のようなテスト関数を書きます。この単体テスト関数は```main```のあるソースコードとは異なるソースコードでも構いません。

```AMTRS_TEST_F()``` は ```void name()``` 型の関数を定義し、テストフレームワークに登録します。また、テストは名前順にソートされて実行されます。

```AMTRS_TEST_EQ(a,b)``` は ```a```と```b```を比較し一致すればパス、一致しなければエラーと見なします。

```AMTRS_TEST_NOTEQ(a,b)``` は ```a```と```b```を比較し一致しなければパス、一致すればエラーと見なします。

```AMTRS_TEST_TRUE(a)``` は ```a``` がtrueならパス、falseならエラーと見なします。

```AMTRS_TEST_FALSE_(a)``` は ```a``` がfalseならパス、trueならエラーと見なします。

これらの比較は c++ の書式として比較可能であればどんな式でも構いません。

```c++
AMTRS_TEST_F(mytest_1)
{
	AMTRS_TEST_EQ(1, 1)
}

AMTRS_TEST_F(mytest_2)
{
	AMTRS_TEST_NOT_EQ(2, 1)
}
```

## 実行方法

テストプログラムを ```mytest.exe``` というバイナリにコンパイルしたと仮定した場合、次のようにテストできます。

```shell
mytest.exe
```

また、次のように ```--filter WILDCARD``` という引数を渡すとワイルドカードに一致するテストのみを行います。

```shell
mytest.exe --filter "mytest_*"
```

## オプション

### ```--filter ワイルドカード```

ワイルドカードにマッチするテストのみを行います。

### ```--stop```

テストに失敗した場合、そこで中断します。デフォルトではテストに失敗した項目があっても最後までテストを行います。

### ```--no-print-pass```

成功したテストはログに表示しないようにします。

# ライセンスについて

libamtrsをビルド＆インストールすると include/amtrs/amtrs-license.hpp ならびに include/amtrs/LICENSES.txt の二つのファイルが生成されます。これらにはビルド時に内包したライブラリのライセンス表記が含まれるようになります。

アプリなどに組み込み際に、これらのライセンスファイルを含め、表示すると良いでしょう。

## amtrs-license.hpp

amtrs-license.hpp には次のような内容が書き込まれます。amtrs自身にはlicenseクラスなどはありませんので、アプリケーション側で実装して下さい。

```c++
license TAG1("title", R"***(license text)***");
license TAG2("title", R"***(license text)***");

license* licenses[] ={&TAG1,&TAG2,nullptr};
```


### Example

amtrs-license.txt は以下のように使うことが出来ます。

```c++
struct	license
{
	char const*	name;
	char const*	text;
};
#include <amtrs/amtrs-license.hpp>

for (auto l : licenses)
{
	if (l)
	{
		std::cout << l->name << std::endl;
		std::cout << l->text << std::endl;
	}
}

```

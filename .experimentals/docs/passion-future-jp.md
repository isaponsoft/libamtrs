# passion future

今後追加してみたい、もしくは面白そうな機能メモ。

## constexpr bool is_enable(EXPR)

EXPRで指定する式が有効かどうか調べる。EXPRは文法上のエラーではない限り、基本的にはどのような式も受け入れ、trueを返す。ただし、式は実行可能かどうかのみチェックされ実行するコードは生成しない。また、EXPRの式が無効な場合はコンパイルエラーの代わりにfalseを返す。

```c++
int  a;
int  b;
is_enable(a == b)  // true. a == b は有効な式である。 

struct B{};
is_enable(B() == 2) // false. struct B には operator == (int) が存在しない。
```

## auto引数

passionでは引数の型にautoを指定できる。autoを指定した場合は template を使用したのと同じ扱いになる。例えば、以下の二つは同じ意味になる①。ただし、virtual関数の場合で規定関数の場合はautoを使うことはできない②。また、overrideした仮想関数の場合、継承元の関数の型にマッピングされる③。

```c++
int my_function(auto& a);

template<class T>
int my_function(T& a);
```

```c++
class Base
{
    virtual func(auto);  // error
};
```

```c++
class Base
{
    virtual int func(int);
    virtual int func(int, string);
    virtual int func2(int, string);
    virtual int func2(string, string);
};

class Extends : public Base super
{
public:
    override auto func(auto);   // int func(int);
    override auto func(auto, auto);   // int func(int, string);
    override auto func2(auto, auto);   // error
};
```

## カスタムパーサー

スクリプト言語を独自に定義する。

```c++
class MyScript : public passion::compiler::parser<MyScript>
{
public:
    override token lex(source_code _source)
    {
    	if (auto m = _source.match<number>()) { return token(m); }
    	if (auto m = _source.match(",")) { return token(m); }
    	if (_source.size() > 1)
    	     { _mys.error(_source, "Unkown character '%c'.", _source.c_str()[0]); }
    	else { _mys.error(_source, "Unkown EOF."); }
    	return {};
    }
    
    override phase parse(token _token)
	{
		if (first) { first = false; this->output += "\""; }
		if (_token.is_terminate())
		{
			this->output += "\"";
		}
		if (_token.is_number())
		{
			this->output += string::format("num is %d", _token.to_integer());
		}
		else
		{
			this->output += ":";
		}
	}
	bool first = true;
};
```

@クラス名でカスタムパーサーを呼び出すことができる。{}で囲まれた範囲がカスタムパーサーのクラスに渡され、カスタムパーサーがpassionの構文に変換することで独自のスクリプト言語を定義できる。この機能はゲームプログラマがチームのスキルやゲームの性質に合わせて編集しやすい独自のスクリプト言語を簡単に実装することに役にたつ。

```c++
string x = @MyScript {
	123,225,776,1,
};
// 次のように置き換えられる。
string x = "num is 123:num is 225:num is 776:num is 1:":
```




/*
 * クラスが使用できます。
 * クラスは new 演算子でインスタンス化し、クラス名^ 型の変数に代入できます。
 * インスタンスは参照カウンタで管理されており、参照がなくなると破棄されます。
 */
int main()
{
	// Typename^ と書くと gc pointer (ガベージコレクタで管理するポインタ)となります。
	// gc pointer は参照カウンタで管理され、自動的にメモリ開放が行われます。
	// ガベージコレクタが管理しないポインタは Typename* と書きますが、passion上では
	// 今のところ非サポートです。
	TestClass^	t;

	t		= new TestClass();

	// メンバ変数に値をセット
	// GCポインタの参照先へのアクセスは . (ドット演算子)を使います。
	t.value2	= "test";

	// メンバ関数を呼び出し
	t.test("Test class");


	// private 変数へは外からはアクセスできない。
//	t.value1	= 1;
//	t.f2("test");

	return	0;
}


class	TestClass
{
public:
	string		value2;

	void test(string msg)
	{
		value2	= "[" + value2 + "]";
		printf(":test:${value2}");
		f2(msg);
	}

private:
	int			value1;

	void f2(string msg)
	{
		printf(":f2:${value2} ${msg}\n");
	}
};

/*
 * decltype()
 * decltypeは()内の式の結果と同じ型を宣言します。
 */
int main()
{
	string		a;

	decltype(a)	b;		// decltype(a) は変数 a と同じ型に置き換わります。
						// つまり string b と書いたのと同じになります。


	// int	c	= b;	// b は string 型なので、int への代入は行えません。
						// そのため、int c = b は構文エラーになります。

	// decltypeは関数にも使用できます。
	// test() の戻り値は string 型なので、この場合
	// string c = "Test";
	// と書いたのと同じ結果になります。
	decltype(test())	c	= "Test";

	// int は式ではないので、decltypeに渡すことはできません。
	// 代わりに int d; と書いてください。
	//decltype(int)		d;


	printf(c + "\n");

	return	0;
}


string test()
{
	return	"OK";
}

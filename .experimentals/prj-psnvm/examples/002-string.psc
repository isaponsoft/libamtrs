int main()
{
	printf("0 ... 1 ... 2\n");


	// 文字列と整数の連結
	int	v	= 1;
	printf("0 ... " + v + " ... 2\n");

	/*
	 * string interpolation
	 * 文字列への式の埋め込み
	 * 文字列の中に ${} で式を書くとコンパイル時に文字列と連結します。
	 * この機能は正確には passion の構文ではなく、プリプロセッサによる事前処理です。
	 * 実際には次のように展開されます。
	 *	printf("0 ... " + (v) + " ... 2\n");
	 */
	printf("0 ... ${v} ... 2\n");

	// ${}の中に、関数呼び出しを入れたりすることもできます。
	// プリプロセッサにより次のように展開されます。
	// printf("2 ... " + (get_one()) + " ... 0\n");
	printf("2 ... ${get_one()} ... 0\n");




	// 文字列オブジェクトはいくつかの関数を持っています。
	if ("Test".starts_with("Tes"))
	{
		printf("match\n");
	}

	if ("*Test*".substr(1, 4) == "Test")
	{
		printf("substr ok\n");
	}

	return	0;
}


int get_one()
{
	return	1;
}

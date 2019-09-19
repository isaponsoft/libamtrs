/*
 * プログラムは原則、main() 関数から始まります。
 * main() 関数もオーバーロードが可能で、psnvmに渡されたコマンドラインの
 * 引数の数によって呼び分けることができます。
 */


//
// mainの第１引数には自信のプログラム名が入ります。
//
int main(string self_name)
{
	printf("${self_name} has not argument\n");
	return	0;
}


//
// 第２引数以降を指定するとコマンドラインから引数を受け取ることができます。
//
int main(string self_name, string arg)
{
	printf("${self_name} have a argument ${arg}\n");
	return	0;
}


//
// 引数無しの main() 関数。
// 引数の数がどれともマッチしない場合に呼ばれます。
//
int main()
{
	// vmpath() は組み込み関数で、psnvmのパスが入ります。
	string	vm	= vmpath();

	printf("usage)\n${vm} program-name\n");
	return	0;
}



/*
可変引数：未実装
int main(string self_name, string arg, string... opts)
{
	printf("${self_name} have a argument ${arg}");

	// foreach ... 未実装
	for (auto m : opts)
	{
		printf(m);
	}
	printf("\n");

	return	0;
}
*/
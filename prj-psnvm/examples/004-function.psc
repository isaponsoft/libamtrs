/*
 * passion では引数の数や型が異なれば同じ名前の関数を宣言することができます。
 * 「オーバーロード(overload)」と呼びます。
 */
int main()
{
	string	t1	= f1("main()");
	printf(t1 + "\n");

	string	t2	= f1(999);
	printf(t2 + "\n");

	return	0;
}


string f1(string m)
{
	return	m + " -> f1()";
}



string f1(int m)
{
	return	"m is ${m}";
}

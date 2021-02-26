int main()
{
	int		a	= 1;
	int		b	= 2;

	test(a, b);

	printf("a = ${a}, b = ${b}\n");

	int&	c	= a;
	printf("a = ${a}, c = ${c}\n");
	c	= 99;
	printf("a = ${a}, c = ${c}\n");


	// 配列の要素に対する参照のテスト
	int[]	ary	= new int[3];
	int&	d	= ary[2];
	d	= 3;
	printf("ary[2] = ${ary[2]}\n");

	// 参照の参照(すなわち単なる参照)
	int&	e	= d;		// e = ary[2] と同義
	e	= 9;
	printf("ary[2] = ${ary[2]}\n");


	return	0;
}

// 第１引数 x は int& 、つまり int型の変数への参照になっています。
// 参照型の変数に代入を行うと、呼び出し元の変数が書き換わります。
// passion ではポインタを禁止していますが、c++ のポインタ風に描くと
// test(int* x, int y)
// {
//		*x = 100;
// と同義になります。
void test(int& x, int y)
{
	printf("x = ${x}, y = ${y}\n");
	x	= 100;
	y	= 200;
	printf("x = ${x}, y = ${y}\n");
}

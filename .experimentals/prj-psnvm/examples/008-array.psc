/*
 * 配列
 */
int main()
{
	int[]	a	= new int[100];
	for (int i = 0; i < 100; ++i)
	{
		a[i] = i;
	}

	// 配列.size() で配列の大きさを取得できます。
	for (int i = 0; i < a.size(); ++i)	
	{
		printf("${a[i]},");
	}
	printf("\n");

/*
	foreach : 未実装
	for (auto v : a)
	{
		printf("${a},");
	}

*/

	test_string();
	test_object();
	test_multi_dimention();

	return	0;
}



void test_string()
{
	string[]	x	 = new string[2];
	for (int i = 0; i < x.size(); ++i)
	{
		x[i]	= "test ${i}";
	}

	for (int i = 0; i < x.size(); ++i)
	{
		printf("${x[i]}\n");
	}
}




class	Z
{
public:

	int	x;

};


void test_object()
{
	Z^[]	x	 = new Z^[2];
	for (int i = 0; i < x.size(); ++i)
	{
		x[i]	= new Z();
		x[i].x	= i;
	}

	for (int i = 0; i < x.size(); ++i)
	{
		printf("${x[i].x}\n");
	}
}



void test_multi_dimention()
{
	int[][]		x	= new int[2][];
	int[]		y	= new int[3];

	x[0]	= y;
	x[0][2]	= 1;


	printf("${x[0][2]}\n");
}


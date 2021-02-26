class	A1;
class	B1;

int main()
{
	C1^	c	= new C1();		// 関数内ではプロトタイプは不要
	c.b1_	= "b1 dayo";
	c.b1("main:");
	c.b2();
	c.b3();
	c.b3(1);
	test(c);

	return	0;
}

void test(B1^ b)			// 定義部分では事前に型宣言が必要。
{
	b.b1_	= "b1 dayo2";
	b.b1("test:");
	b.b2();
	b.b3();
//	b.b3(1);			// B1.b1(int) は存在しないので、呼び出すことはできない。
}



class	A1
{
public:
};

class	B1
{
public:
	string	b1_;

	int b1(string m)
	{
		printf("${m} ${b1_}\n");
		return	11;
	}

	int b2()
	{
		printf("B1.b2 ${b1_}\n");
		return	11;
	}

	int b3()
	{
		return	11;
	}

};


class	C1
		: public A1
		, public B1
{
public:
	int b2()
	{
		printf("C1.b2 ${b1_}\n");
		return	11;
	}

	int b3(int a)
	{
		return	11;
	}
};

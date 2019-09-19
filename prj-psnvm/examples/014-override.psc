class	A1
{
public:
	A1()
	{
		v	= "Test";
	}

	virtual void f1()
	{
		printf("A1.f1(${v})\n");
	}

	void f2()
	{
		printf("A1.f2(${v})\n");
	}

	string	v;
};

class	C1
		: public A1
{
public:
	C1()
	{
	}

	~C1()
	{
	}

	virtual void f1()
	{
		printf("C1.f1(${v})\n");
	}

	void f2()
	{
		printf("C1.f2(${v})\n");
	}
};


int main()
{
	C1^	c	= new C1();
	A1^	a	= c;


	a.f1();
	c.f1();

	a.f2();
	c.f2();


	printf("ok\n");
	return	0;
}

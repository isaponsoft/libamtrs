/*
 * メンバ変数へのアクセスのテスト。
 * 仮想関数、非仮想関数から正しくメンバ変数へアクセスできるかどうかのテスト。
 */
#define	V1			"value is a1"
#define	V2			"value is b1"
#define	ERR()		error = 1; printf("${__FILE__}(${__LINE__})\n");

int	error;


int main()
{
	error	= 0;
	test1();
	test2();
	return	error;
}

void test1()
{
	C1^	c	= new C1();
	B1^	b	= c;
	A1^	a	= c;

	// Variable set with base class.
	a.a1_	= V1;
	b.b1_	= V2;

	c.f();
	b.f();
	c.vf();
	b.vf();
}


void test2()
{
	C1^	c	= new C1();
	B1^	b	= c;
	A1^	a	= c;

	// Variable set with extends class.
	c.a1_	= V1;
	c.b1_	= V2;
	c.f();
	b.f();
	c.vf();
	b.vf();
}



class	A1
{
public:
	string	a1_;
};

class	B1
{
public:
	string	b1_;

	~B1()
	{
		if (b1_ != V2)
		{
			ERR();
		}
	}

	int f()
	{
		if (b1_ != V2)
		{
			ERR();
		}
		return	11;
	}

	virtual int vf()
	{
		ERR();
		return	11;
	}
};


class	C1
		: public A1
		, public B1
{
public:

	~C1()
	{
		if (a1_ != V1)
		{
			ERR();
		}
		if (b1_ != V2)
		{
			ERR();
		}
	}

	int f()
	{
		if (a1_ != V1)
		{
			ERR();
		}
		if (b1_ != V2)
		{
			ERR();
		}
		return	11;
	}

	virtual int vf()
	{
		if (a1_ != V1)
		{
			ERR();
		}
		if (b1_ != V2)
		{
			ERR();
		}
		return	11;
	}
};

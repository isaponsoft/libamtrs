#define	CHECK(x, y)	if (x != y) { printf("${__FILE__}(${__LINE__})\n"); return 1 };

int main()
{
	{
		auto	r	= test1();
		if (r)
		{
			return	r;
		}
	}

	{
		auto	r	= test2();
		if (r != 1)
		{
			return	r;
		}
	}

	return	0;
}


int test1()
{
	int	a	= 0;
	int	b	= a == 0;			// b is 1
	CHECK(b, 1);
	if (b == 1)
	{
		a	= 2;
	}
	b	= (a == 2) ? -1 : 1;	// b is -1
	CHECK(b, -1);

	a	= b + 1;
	CHECK(a, 0);

	b	= a == 0 ? -1 : 1;		// b is -1
	CHECK(b, -1);

	b	= 10 + 2 * (3 + 1) + 9;
	CHECK(b, 27);

	return	b - 27;
}


int test2()
{
	int	a	= 0;
	for (int b = 0; b < 100; ++b)
	{
		++a;
	}
	return	a == 100 ? 1 : -1;
}

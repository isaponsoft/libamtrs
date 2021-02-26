int		error;

class	Class
{
public:
	int	x;
	int	y;

	Class(int a)
	{
		x	= 1;
		y	= a;
	}


	Class(int a, int b)
	{
		x	= 2;
		y	= a + b;
	}

	Class(int a, int b, int c)
	{
		x	= 3;
		y	= a + b + c;
	}
};


int main()
{
	error	= 0;

	{
		auto	c	= new Class(1);
		if (c.x != 1 || c.y != 1)
		{
			return	1;
		}
	}

	{
		auto	c	= new Class(1, 2);
		if ((c.x != 2) || (c.y != 3))
		{
			return	1;
		}
	}

	{
		auto	c	= new Class(1, 2, 3);
		if (c.x != 3 || c.y != 6)
		{
			return	1;
		}
	}

	{
		auto	c	= test1(1, 2);
		if ((c.x != 2) || (c.y != 3))
		{
			return	1;
		}
	}

	if (test2(1, 2, 3))
	{
		error	= 1;
	}


	return	error;
}


Class^ test1(int a, int b)
{
	return	new Class(a, b);
}


int test2(int v1, int v2, int v3)
{
	Class^	c	= new Class(v1, v2, v3);
	if (c.x == 3 && c.y == v1 + v2 + v3)
	{
		return	0;
	}
	return	1;
}


int	destroy;

class	Class
{
public:
	~Class()
	{
		++destroy;
	}
};


int main()
{
	test1();
	test2();
	return	(destroy == 4) ? 0 : 1;
}


void test1()
{
	Class^	d	= new Class();
	{
		Class^	c	= new Class();
	}
	return;
}


void test2()
{
	Class^	d	= new Class();
	{
		Class^	c	= new Class();
	}
}

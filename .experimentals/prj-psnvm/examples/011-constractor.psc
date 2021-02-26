class	Test
{
public:
	Test()
	{
		value	= "Test()";
	}

	Test(int a)
	{
		value	= "Test(${a})";
	}

	Test(int a, int b)
	{
		value	= "Test(${a}, ${b})";
	}

	string	value;
};

int main()
{
	auto	t1	= new Test();
	auto	t2	= new Test(1);
	auto	t3	= new Test(2, 3);

	printf("${t1.value}\n");
	printf("${t2.value}\n");
	printf("${t3.value}\n");

	return	0;
}

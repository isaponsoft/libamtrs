class	Test
{
public:
	~Test()
	{
		printf("~Test\n");
	}
};



Test^	sTest;
string	sPath;

int main()
{
	sPath	= "ABC";
	test();
	printf(sPath + "\n");
	return	0;
}



void test()
{
	printf(sPath + "\n");
	sPath	= "abc";
	sTest	= new Test();
}


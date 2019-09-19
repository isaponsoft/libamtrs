/*
 * 関数の引数に対するテスト
 */

int	error;
int	value;

int main()
{
	value	= 0;

	test1();
	test1(1);
	test1(1, 2, 3);
	test2("1", 2, "3");
	if (value != 3)
	{
		error	= 1;
	}
	return	error;
}

void test1()
{
	error	= 0;
}


void test1(int one)
{
	if (one != 1)
	{
		error	= 1;
		return;
	}
	++value;
}



void test1(int one, int two, int three)
{
	if (one != 1)
	{
		error	= 1;
		return;
	}

	if (two != 2)
	{
		error	= 1;
		return;
	}

	if (three != 3)
	{
		error	= 1;
		return;
	}
	++value;
}


void test2(string one, int two, string three)
{
	if (one != "1")
	{
		error	= 1;
		return;
	}

	if (two != 2)
	{
		error	= 1;
		return;
	}

	if (three != "3")
	{
		error	= 1;
		return;
	}
	++value;
}

/*
 * 関数の引数に対するテスト
 */

int	error;
int	value;

class	Test;


int main()
{
	value	= 0;

	auto	t	= new Test();

	t.test1();
	t.test1(1);
	t.test1(1, 2, 3);
	t.test2("1", 2, "3");
	if (value != 3)
	{
		error	= 1;
	}
	return	error;
}

class	Test
{
public:


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


};
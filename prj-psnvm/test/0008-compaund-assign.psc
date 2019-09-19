int main()
{
	int	a	= 0;
	a += 1;
	if (a != 1)
	{
		return	1;
	}

	a -= 2;
	if (a != -1)
	{
		return	2;
	}

	a *= 3;
	if (a != -3)
	{
		return	3;
	}

	a += 13;
	if (a != 10)
	{
		return	4;
	}

	a /= 5;
	if (a != 2)
	{
		return	5;
	}

	a %= 2;
	if (a != 0)
	{
		return	6;
	}

	return	0;
}

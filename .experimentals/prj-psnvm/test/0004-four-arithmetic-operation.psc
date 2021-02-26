/*
 * 四則演算のテスト
 */
int	error;

int main()
{
	error	= 0;

	int	x	= 1 + 8 * 5 / 4;
	if ("${x}" != "11")
	{
		error	= 1;
	}

	int	y	= (1 + 2) * 6 / 3;
	if ("${y}" != "6")
	{
		error	= 1;
	}

	return	error;
}

/*
 * if文、 for文、while文、do-while文
 * c/c++ のそれと同じです。
 */
int main()
{
	printf("if test\n");
	if_test(0);
	if_test(1);

	printf("for test\n");
	for_test(3);
	for_test_break(10, 3);
	for_test_continue(10, 3);

	printf("while test\n");
	while_test(3);
	while_test_break(9, 7);
	while_test_continue(9, 7);

	printf("do while test\n");
	do_while_test(3);
	do_while_test_break(9, 7);
	do_while_test_continue(9, 3);

	return	0;
}


void if_test(int v)
{
	if (v)
	{
		printf("v is true\n");
	}
	else
	{
		printf("v is false\n");
	}
}


void for_test(int c)
{
	for (int l = 0; l < c; l = l + 1)
	{
		printf("${l}");
	}
	printf("\n");
}



void for_test_break(int c, int b)
{
	for (int l = 0; l < c; l = l + 1)
	{
		if (l >= b)
		{
			break;
		}
		printf("${l}");
	}
	printf("\n");
}



void for_test_continue(int c, int b)
{
	for (int l = 0; l < c; l = l + 1)
	{
		if (l >= b)
		{
			printf("*");
			continue;
		}
		printf("${l}");
	}
	printf("\n");
}



void while_test(int c)
{
	while (c)
	{
		printf("${c}");
		c = c - 1;
	}
	printf("\n");
}


void while_test_break(int c, int b)
{
	while (c)
	{
		printf("${c}");
		c = c - 1;
		if (c < b)
		{
			break;
		}
	}
	printf("\n");
}


void while_test_continue(int c, int b)
{
	while (c)
	{
		printf("${c}");
		c = c - 1;
		if (c < b)
		{
			printf("*");
			continue;
		}
	}
	printf("\n");
}


void do_while_test(int c)
{
	do
	{
		printf("${c}");
		c = c - 1;
	} while (c);
	printf("\n");
}

void do_while_test_break(int c, int b)
{
	do
	{
		printf("${c}");
		c = c - 1;
		if (c < b)
		{
			break;
		}
	} while (c);
	printf("\n");
}

void do_while_test_continue(int c, int b)
{
	do
	{
		c = c - 1;
		if (c > b)
		{
			printf("*");
			continue;
		}
		printf("${c}");
	} while (c);
	printf("\n");
}


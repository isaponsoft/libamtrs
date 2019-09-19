/*
 * ハノイの塔
 */
int main()
{
	int disk	= 4;

	printf("disk：${disk}\n");
	printf("minimam turn：${hanoi_min(disk)}\n");

	printf("-------- History ---------\n");
	hanoi(disk, "A", "B", "C");

	return	0;
}

int hanoi_min(int n)
{
	if (n > 1)
	{
		return	(2*hanoi_min(n - 1) + 1);
	}
	else
	{
		return 1;
	}
}


void hanoi(int n, string start, string use, string end)
{
	if (n > 0)
	{
		hanoi(n - 1, start, end, use);
		printf("mov disk${n} ${start} to ${end}\n");
		hanoi(n - 1, use, start, end);
	}
}

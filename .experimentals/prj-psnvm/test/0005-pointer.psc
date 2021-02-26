#define	NUMBER	199

int main()
{
	int[]	a	= new int[2];
	a[1]	= NUMBER;
	int*	c	= &a[1];
	int		d	= *c;

	return	d == NUMBER ? 0 : 1;
}

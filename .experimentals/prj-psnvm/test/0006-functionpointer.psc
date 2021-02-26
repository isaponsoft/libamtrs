#define	STRING	"abcdefghijk"


int main()
{
	auto	a	= &test1;
	auto	b	= &test2;

	(*a)(STRING);
	(*b)(STRING, STRING);
	return	(*a)(STRING) + (*b)(STRING, STRING) - 2 == 0 ? 0 : 1;
}


int test1(string x)
{
	return	x == STRING;
}


int test2(string x, string y)
{
	return	(x + y) == (STRING + STRING);
}

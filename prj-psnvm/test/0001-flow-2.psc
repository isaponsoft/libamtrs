#define	LOOP_COUNT	2

int	count;

class	C
{
public:
	~C()
	{
		++count;
	}
};

int main()
{
	count	= 0;
	for (int c = 0; c < LOOP_COUNT; ++c)
	{
		if (auto x = new C(); x)
		{
		}
		else
		{
			return	1;
		}
	}
	return	count == LOOP_COUNT ? 0 : 1;
}

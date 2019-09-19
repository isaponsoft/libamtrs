#define	A
#define	B	0
#define	C	1
#define	D	1



int main()
{
	#ifdef	X
		printf("${__FILE__}(${__LINE__})\n");
		#ifdef	X
		printf("${__FILE__}(${__LINE__})\n");
		#endif
	printf("${__FILE__}(${__LINE__})\n");
	#elif	1
	printf("${__FILE__}(${__LINE__})\n");
	#else
	printf("${__FILE__}(${__LINE__})\n");
	#endif

	#if		B ? C : 0
	printf("${__FILE__}(${__LINE__})\n");
	#endif

	return	0;
}

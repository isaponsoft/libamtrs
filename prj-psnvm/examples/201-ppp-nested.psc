/*
 * TPP はネストされたマクロ展開を行うことができます。
 */
#define	DEFINE_MACRO(_name, _text)	#define _name _text

// 下記は #define Hello "hello world" に展開されます。
DEFINE_MACRO(Hello,  "hello world")

// 下記は #define Macro2 "Hello, world." に展開されます。
DEFINE_MACRO(Macro2, "Hello, world.")


int main()
{
	printf(Hello + "\n");
	printf(Macro2 + "\n");

	return	0;
}

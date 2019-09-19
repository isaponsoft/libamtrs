int main()
{
	auto	reg	= new regex();
	reg.patturn("abc");


	auto	r	= reg.replace("abcdefg", "---");
	printf("${r}\n");


	reg.patturn("^.*bc.*");
	auto	m	= reg.match("abcdefg");
	printf("${m.size()}\n");

	return	0;
}

/*
 * クラスは入れ子にすることができます。
 */
int main()
{
	// auto を使うこと、右辺値に合わせて自動的に型を
	// 決定します。
	TestClass::InnerClass^	t;
	t	= new TestClass::InnerClass();
	auto	m	= t.test("Test");

	printf(m + "\n");

	return	0;
}

class	TestClass
{
public:
	class	InnerClass
	{
	public:
		string test(string msg)
		{
			return	"test(${msg})";
		}
	};
};

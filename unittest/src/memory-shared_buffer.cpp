#include <iostream>
#include <tuple>
#include <amtrs/memory/ref_object.hpp>
#include <amtrs/type_traits/pair_base.hpp>

using namespace amtrs;



struct	mydata
{
	int	val	= -1;

	mydata()
	{
		std::cout << "new" << std::endl;
	}

	~mydata()
	{
		std::cout << "delete : " << val << std::endl;
	}
};

int main()
{
	{
	std::string	x("jello");
	std::string	y(std::begin(x), std::end(x));

	shared_buffer<char, std::string>	a(10, std::string(std::begin(x), std::end(x)));
	shared_buffer<char, std::string>	b(10, std::make_tuple(std::begin(x), std::end(x)));


	std::cout << a.header() << "/" << a.size() << std::endl;
	}

	{
		shared_buffer<mydata>				c(10);
		int	n	= 0;
		for (auto& i : c)
		{
			i.val	= n++;
		}

		for (auto it = c.rbegin(); it != c.rend(); ++it)
		{
			std::cout << it->val << ",";
		}
		std::cout << std::endl;
	}
	return	0;
}

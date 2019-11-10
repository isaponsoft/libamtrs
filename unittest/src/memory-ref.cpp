#include <iostream>
#include <amtrs/memory/ref_object.hpp>

using namespace amtrs;



struct	my_class
		: public ref_object
{
	~my_class()
	{
		std::cout << "~myClass" << std::endl;
	}

	static void destroy(my_class* _m)
	{
		std::cout << "destroy" << std::endl;
	}


	void dec_ref()
	{
		if (ref_object::dec_ref(0))
		{
			destroy(this);
		}
	}

};




int main()
{
	ref<my_class>	r2;
	{
		{
			ref<my_class>	r(new my_class());
			r2	= r;
		}
		std::cout << "OK" << std::endl;

	}
	std::cout << "OK2" << std::endl;
	r2	= nullptr;
	std::cout << "OK3" << std::endl;


	return	0;
}

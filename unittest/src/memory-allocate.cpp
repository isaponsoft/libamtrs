#include <iostream>
#include <amtrs/memory/ref_object.hpp>

using namespace amtrs;


template<class CharT, class Alloc = std::allocator<CharT>>
struct	my_class
		: public ref_object
		, private Alloc
{
	using	allocator_type	= Alloc;
	using	char_type		= CharT;

	static constexpr size_t	struct_size	= (sizeof(my_class) - sizeof(char_type) - 1) / sizeof(char_type);

	my_class(allocator_type&& _alloc)
		: allocator_type(_alloc)
	{}

	static ref<my_class> create()
	{
		allocator_type	alloc;
		char_type*		m		= std::allocator_traits<allocator_type>::allocate(alloc, struct_size);
		my_class*		t		= reinterpret_cast<my_class*>(m);
		std::allocator_traits<allocator_type>::construct(alloc, t, std::move(alloc));
		return	t;
	}



	static void destroy(my_class* _t)
	{
		std::cout << "destroy" << std::endl;
		std::allocator_traits<allocator_type>::destroy(*_t, _t);
		std::allocator_traits<allocator_type>::deallocate(*_t, reinterpret_cast<char_type*>(_t), struct_size);
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
	using	type	= my_class<char>;

	ref<type>	m	= type::create();


	return	0;
}

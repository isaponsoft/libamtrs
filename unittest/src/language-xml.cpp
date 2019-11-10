#include <iostream>
#include <fstream>
#include <amtrs/language/xml/@>
#include <amtrs/io/@>
#include <amtrs/string/@>
#include <amtrs/memory/shared_buffer.hpp>

using namespace amtrs;

AMTRS_LANGUAGE_XML_NAMESPACE_BEGIN
AMTRS_LANGUAGE_XML_NAMESPACE_END


int main(int _argc, char* _args[])
{
#if	1
	auto	ei	= lang::xml::element_iterator(io::readall<shared_buffer<char>>(std::fstream("data/example.xml")));

	auto	tag	= lang::xml::find_by_id(ei, (std::string_view)"Marsh");
	if (tag.empty())
	{
		std::cout << "nothing" << std::endl;
	}
	else
	{
		std::cout << (std::string_view)tag << std::endl;
	}

#else

	for (auto& e : lang::xml::element_iterator(io::readall<std::string>(std::fstream("data/example.xml"))))
	{
		std::cout << (e.tag() ? "tag" : "cnt") << " : " << (std::string_view)e << std::endl;
		if (e.tag())
		{
			for (auto& attr : lang::xml::attribute_iterator(lang::xml::trim_tag_chars(e)))
			{
				std::cout << "    +- " << attr.field() << " = " << attr.value() << std::endl;
			}
		}
	}
#endif
	return	0;
}


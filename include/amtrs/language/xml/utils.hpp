/******************************************************************************
Copyright (c) 2019, isaponsoft (Isao Shibuya)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the libamtrs project.
******************************************************************************/
#ifndef	__libamtrs__language__xml__utils___hpp
#define	__libamtrs__language__xml__utils___hpp
#include "attribute.hpp"
#include "element.hpp"
AMTRS_LANGUAGE_XML_NAMESPACE_BEGIN



template<class It, class CharT, class CharTraits>
auto find_by_id(It&& _first, It&& _last, std::basic_string_view<CharT, CharTraits> _id) -> basic_element<CharT>
{
	for (auto& e : make_bucket(std::forward<It>(_first), std::forward<It>(_last)))
	{
		if (e.tag())
		{
			for (auto& attr : attribute_iterator(lang::xml::trim_tag_chars(e)))
			{
				if ((attr.field() == "id") && (trim_quote(attr.value()) == _id))
				{
					return	e;
				}
			}
		}
	}
	return	basic_element<CharT>((std::basic_string_view<CharT>)*_last, elmtype::emptytype);
}


template<class Containor, class CharT, class CharTraits>
auto find_by_id(Containor&& _containor, std::basic_string_view<CharT, CharTraits> _id) -> basic_element<CharT>
{
	return	find_by_id(_containor.begin(), _containor.end(), _id);
}



AMTRS_LANGUAGE_XML_NAMESPACE_END
#endif

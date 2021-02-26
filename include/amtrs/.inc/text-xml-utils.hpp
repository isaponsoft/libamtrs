/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__language__xml__utils___hpp
#define	__libamtrs__language__xml__utils___hpp
AMTRS_LANGUAGE_XML_NAMESPACE_BEGIN



template<class It, class CharT, class CharTraits>
auto find_by_id(It&& _first, It&& _last, std::basic_string_view<CharT, CharTraits> _id) -> basic_element<CharT>
{
	for (auto& e : make_bucket(std::forward<It>(_first), std::forward<It>(_last)))
	{
		if (e.tag())
		{
			for (auto& attr : attribute_iterator(trim_tag_chars(e)))
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

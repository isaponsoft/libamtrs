/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__text_xml__hpp
#define	__libamtrs__text_xml__hpp
#include "amtrs.hpp"
#include "string.hpp"

#define	AMTRS_LANGUAGE_XML_NAMESPACE_BEGIN	AMTRS_NAMESPACE_BEGIN namespace text::xml {
#define	AMTRS_LANGUAGE_XML_NAMESPACE_END	} AMTRS_NAMESPACE_END


AMTRS_LANGUAGE_XML_NAMESPACE_BEGIN

template<class CharT>
class	basic_attribute;

template<class CharT>
class	basic_attribute_iterator;


using	attribute			= basic_attribute<char>;
using	attribute_iterator	= basic_attribute_iterator<char>;

AMTRS_LANGUAGE_XML_NAMESPACE_END

// keep order.
#include ".inc/text-xml-chars.hpp"
#include ".inc/text-xml-element.hpp"
#include ".inc/text-xml-attribute.hpp"
#include ".inc/text-xml-utils.hpp"
#endif

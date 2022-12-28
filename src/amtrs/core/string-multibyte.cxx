/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include  <cwchar>
#include <amtrs/string/multibyte.hpp>
AMTRS_STRING_NAMESPACE_BEGIN

text::size_type mbstrlen(text _text)
{
	text::size_type		len2	= _text.size();
	text::size_type		i		= 0;
	mbstate_t			mbs;
	while (i < len2 && _text[(int)i])
	{
		mbs	= {0};
		int	charLen	= (int)mbrlen(_text.s + i, _text.size() - i, &mbs);
		if (charLen <= 0)
		{
			break;
		}
		i += charLen;
	}
	return	i;
}

// 先頭からマルチバイトを考慮して有効な文字列を切り抜いて返す。
text mbtext(text _text)
{
	return	_text.substr(0, mbstrlen(_text));
}


AMTRS_STRING_NAMESPACE_END

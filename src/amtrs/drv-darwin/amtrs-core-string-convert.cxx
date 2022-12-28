/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
AMTRS_NAMESPACE_BEGIN

static sstring conv(text _text, text _to, text _from);

// locale => utf8
sstring cr_to_c8(text _text)
{
	return	conv(_text, "UTF-8", "UTF-8");
}

// utf8 => locale
sstring c8_to_cr(text _text)
{
	return	conv(_text, "UTF-8", "UTF-8");
}

static sstring conv(text _text, text _to, text _from)
{
	return	_text;
}

AMTRS_NAMESPACE_END

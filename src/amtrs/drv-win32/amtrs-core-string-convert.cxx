/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <windows.h>
AMTRS_NAMESPACE_BEGIN

static sstring to_char(UINT _codepage, wtext _in)
{
	sstring			dst;
	auto			len	= ::WideCharToMultiByte(_codepage, 0, _in.data(), (int)_in.size(), NULL, 0, NULL, NULL);
	dst.resize(len);
	::WideCharToMultiByte(_codepage, 0, _in.data(), (int)_in.size(), dst.data(), (int)dst.size(), NULL, NULL);
	return	dst;
}

static wsstring to_wchar(UINT _codepage, text _in)
{
	auto		len	= ::MultiByteToWideChar(_codepage, 0, _in.data(), (int)_in.size(), NULL, 0);
	wsstring	dst;
	dst.resize(len);
	::MultiByteToWideChar(_codepage, 0, _in.data(), (int)_in.size(), dst.data(), len);
	return	dst;
}


// wide char => UTF-8
sstring wc_to_c8(wtext _text)
{
	return	to_char(CP_UTF8, _text);
}

// UTF-8 => wide char
wsstring c8_to_wc(text _text)
{
	return	to_wchar(CP_UTF8, _text);
}

// locale => utf8
sstring cr_to_c8(text _text)
{
	return	to_char(CP_UTF8, to_wchar(GetACP(), _text));
}

// utf8 => locale
sstring c8_to_cr(text _text)
{
	return	to_char(GetACP(), to_wchar(CP_UTF8, _text));
}


AMTRS_NAMESPACE_END

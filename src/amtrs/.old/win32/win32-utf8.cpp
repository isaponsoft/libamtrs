/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/filesystem.hpp>
AMTRS_NAMESPACE_BEGIN

static std::string convert(UINT _st, UINT _dt, std::string_view _src)
{
	auto			len	= ::MultiByteToWideChar(_st, 0, _src.data(), _src.size(), NULL, 0);
	std::wstring	u16;
	u16.resize(len);
	::MultiByteToWideChar(_st, 0, _src.data(), _src.size(), u16.data(), len);

	std::string		dst;
	len	= ::WideCharToMultiByte(_dt, 0, u16.data(), u16.size(), NULL, 0, NULL, NULL);
	dst.resize(len);
	::WideCharToMultiByte(_dt, 0, u16.data(), u16.size(), dst.data(), dst.size(), NULL, NULL);

	return	dst;
}


std::string to_utf8(std::string_view _locale_string)
{
	return	convert(CP_ACP, CP_UTF8, _locale_string);
}


std::string to_current_locale(std::string_view _locale_string)
{
	return	convert(CP_UTF8, CP_ACP, _locale_string);
}

AMTRS_NAMESPACE_END

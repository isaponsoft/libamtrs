/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__string__get_quotetion_string__hpp
#define	__libamtrs__string__get_quotetion_string__hpp
AMTRS_NAMESPACE_BEGIN


/*! "string" => string
 * ----------------------------------------------------------------------------
 * [ s, a ] = get_quotetion_string(R"("hello \" " world")");
 * s => ["hello " "]
 * a => [ world]
 * ----------------------------------------------------------------------------
 *	_use_backspace_escape: true => enable \"
 *	_use_double_escape:    true => enable ""
 * return
 *		抜き出した文字列
 */
inline std::string_view get_quotetion_string(std::string_view _text, bool _use_backspace_escape = true, bool _use_double_escape = true)
{
	if (_text.size() < 2 || _text[0] != '"')
	{
		return	_text.substr(0, 0);
	}

	size_t	i	= 1;
	while (i < _text.size())
	{
		if (_text[i] == '\\' && _use_backspace_escape)
		{
			++i;
		}
		else if (_text[i] == '"')
		{
			if ((!_use_double_escape) || (_text.size() <= (i + 1)) || (_text[i+1] != '"'))
			{
				return	_text.substr(0, i+1);
			}
			++i;
		}
		++i;
	}
	return	_text.substr(0, 0);
}

AMTRS_NAMESPACE_END
#endif

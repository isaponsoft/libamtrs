/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__string__readline__hpp
#define	__libamtrs__string__readline__hpp
AMTRS_NAMESPACE_BEGIN

/*
 * １行を読み取ります。
 * 改行コード(\r\n)を含みます。
 *	_noempty
 *		true:  改行コードが見つからない場合でも文字列が残っていれば残りの文字列を返します。
 *		false: 改行コードが見つからない場合は文字列が残っていても empty() を返します。
 */
template<class CharT, class TraitsT>
auto readline(std::basic_string_view<CharT, TraitsT> _string, bool _noempty = false) -> std::basic_string_view<CharT, TraitsT>
{
	if (_string.empty())
	{
		return	_string;
	}

	auto	pos	= _string.find_first_of("\r\n");

	// 見つからない場合はnoemptyに従った値を返す。
	if (pos == std::basic_string_view<CharT, TraitsT>::npos)
	{
		return	_noempty
				? _string					// 残り全体を返す
				: _string.substr(0, 0);		// 空を返す
	}

	if ((pos + 1) <= _string.size())
	{
		auto	c	= _string[pos+1];
		if ((c == '\r' || c == '\n') && c != _string[pos])
		{
			++pos;
		}
	}
	return	_string.substr(0, pos+1);
}


AMTRS_NAMESPACE_END
#endif

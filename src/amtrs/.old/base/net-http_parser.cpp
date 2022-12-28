/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/net.hpp>
AMTRS_NET_HTTP_NAMESPACE_BEGIN

/*! http token.
 * ひとつのトークンを取得します。
 * _inからディレクティブを読み取り戻り値として返します。
 */
std::string_view http_get_token(std::string_view _in)
{
	if (_in.size() == 0)
	{
		return	_in.substr(0, 0);
	}

	if (!std::isalpha(_in.front()))
	{
		return	_in.substr(0, 0);
	}

	// alphabet & '-'
	int		i	= 1;
	while ((i < _in.size()) && (std::isalpha(_in[i]) || _in[i] == '-'))
	{
		++i;
	}
	return	_in.substr(0, i);
}


/*! http header directives.
 * _inからディレクティブとして認識可能な範囲を読み取り、戻り値として返します。
 * _name : ディレクティブ名
 * _value: 値(ダブルコーテーションが含まれる場合、ダブルコーテーションを含みます)
 * return: ディレクティブ以降の文字列
 * 		';(セミコロン)' で終了す場合、セミコロンの直前を返します。
 * 		これによりセミコロンで終わっているかどうかを呼び出し側が把握しやすくなります。
 */
std::pair<directive_result, std::string_view> http_get_directive(std::string_view _in)
{
	std::pair<directive_result, std::string_view>	retval;
	retval.first.name	= _in.substr(0, 0);
	retval.first.value	= _in.substr(0, 0);
	retval.second		= _in;

	// ディレクティブ名があるか調べる
	auto	dirname	= http_get_token(_in);
	if (dirname.empty())
	{
		return	retval;
	}
	retval.first.name	= dirname;
	retval.second		= _in.substr(dirname.size());
	retval.first.value	= retval.second.substr(0, 0);

	// 値を調べる
	if ((retval.second.size() >= 1) && (retval.second[0] == '='))
	{
		// have a value;	
		if ((retval.second.size() >= 3) && (retval.second[1] == '"'))
		{
			std::string_view	r	= get_quotetion_string(retval.second.substr(1));
			retval.first.value	= r;
			retval.second		= retval.second.substr(1 + r.size());
		}
		else
		{
			int		i	= 1;
			while ((i < retval.second.size()) && (retval.second[i] != ' ' && retval.second[i] != ';'))
			{
				++i;
			}
			retval.first.value	= retval.second.substr(1, i-1);
			retval.second		= retval.second.substr(i);
		}
	}
	return	retval;
}


bucket<directive_iterator> enumrate_directive(std::string_view _header_line)
{
	directive_iterator	begin;
	directive_iterator	end;
	{
		directive_result	tmp;
		tmp.name	= _header_line.substr(0, 0);
		tmp.value	= _header_line.substr(0, 0);
		begin		= directive_iterator(std::move(tmp), _header_line);
	}
	{
		directive_result	tmp;
		tmp.name	= _header_line.substr(_header_line.size());
		tmp.value	= _header_line.substr(_header_line.size());
		end			= directive_iterator(std::move(tmp), _header_line.substr(_header_line.size()));
	}
	++begin;
	return	{ begin, end };
}


directive_iterator& directive_iterator::operator ++ ()
{
	auto	r	= http_get_directive(mRemain);
	mDirective	= r.first;
	mRemain		= r.second;
	if (mDirective.name.empty() || (mRemain.size() == 0))
	{
		mRemain		= mRemain.substr(mRemain.size());
	}
	else if (mRemain[0] != ' ' && mRemain[0] != ';')
	{
		mRemain		= mRemain.substr(mRemain.size());
	}
	else
	{
		mRemain		= trim_first(r.second, " ;");
	}
	return	*this;
}

AMTRS_NET_HTTP_NAMESPACE_END

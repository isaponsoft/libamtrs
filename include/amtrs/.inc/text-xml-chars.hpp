/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__language__xml__chars___hpp
#define	__libamtrs__language__xml__chars___hpp
AMTRS_LANGUAGE_XML_NAMESPACE_BEGIN



inline bool isbasechar(char _c)
{
	return	std::isalpha(_c); // hiragana, katakana...
}


inline bool isideographic(char _c)
{
	return	false;	// kanji...
}


inline bool iscombiningcgar(char _c)
{
	return	false;	// umraut, accent mark...
}


inline bool isletter(char _c)
{
	return	isbasechar(_c) || isideographic(_c);
}


inline bool isextender(char _c)
{
	return	false;	// bar...
}


inline bool isattributefirst(char _c)
{
	return	isletter(_c);
}


inline bool isattributesecond(char _c)
{
	return	isletter(_c) || std::isdigit(_c) || iscombiningcgar(_c) || isextender(_c)
		||	(_c == '.')
		||	(_c == '-')
		||	(_c == '_')
		||	(_c == ':');
}


inline bool isspace(char _c)
{
	return	(_c == ' ') || (_c == '\t');
}


// ============================================================================
//! 空白をスキップする
// ----------------------------------------------------------------------------
template<class CharT, class CharTraits>
std::basic_string_view<CharT> skipspace(std::basic_string_view<CharT, CharTraits> _in)
{
	while (!_in.empty() && isspace(_in[0]))
	{
		_in	= _in.substr(1);
	}
	return	_in;
}



template<class CharT, class CharTraits>
auto trim_quote(std::basic_string_view<CharT, CharTraits> _in) -> std::basic_string_view<CharT>
{
	return	trim(_in, "\"\'");
}


// ============================================================================
//! xmlのタグとして開始できるかどうか
// ----------------------------------------------------------------------------
template<class CharT, class CharTraits>
std::basic_string_view<CharT, CharTraits> tag_starts_with(std::basic_string_view<CharT, CharTraits> const& _in)
{
	return	(_in.size() >= 2) && (_in[0] == '<') && (std::isalpha(_in[1]) || _in[1] == '/')
			? _in.substr(0, 2)
			: _in.substr(0, 0);
}


// ============================================================================
//! 
// ----------------------------------------------------------------------------
template<class CharT, class CharTraits, class Callback>
std::basic_string_view<CharT, CharTraits> find_view_if(std::basic_string_view<CharT, CharTraits> _in, Callback&& _callback)
{
	bool	escape	= false;
	bool	dquote	= false;
	bool	squote	= false;
	return	amtrs::find_view_if(_in, [&](auto _view) -> std::basic_string_view<CharT, CharTraits>
			{
				if (escape)
				{
					escape	= false;
					return	_view.substr(0, 0);
				}
				if (_view[0] == '\\')
				{
					escape	= true;
					return	_view.substr(0, 0);
				}
				if (_view[0] == '\"')
				{
					if (squote)
					{
						return	_view.substr(0, 0);
					}
					dquote	= !dquote;
				}
				if (_view[0] == '\'')
				{
					if (dquote)
					{
						return	_view.substr(0, 0);
					}
					squote	= !squote;
				}
				if (squote || dquote)
				{
					return	_view.substr(0, 0);
				}
				return	_callback(_view);
			});
}



// ============================================================================
//! タグを示す文字を除外します。
// ----------------------------------------------------------------------------
//! <tag xyz="abc" /> とあったばあい ' xyz="abc" ' だけを返します。
// ----------------------------------------------------------------------------
template<class CharT, class CharTraits>
auto trim_tag_chars(std::basic_string_view<CharT, CharTraits> _view) -> std::basic_string_view<CharT, CharTraits>
{
	// まずは頭を削る
	if ((_view.size() >= 1) && (_view[0] == '<'))
	{
		_view	= _view.substr(1);
		if ((_view.size() >= 1) && (_view[0] == '/'))
		{
			_view	= _view.substr(1);
		}
		while ((_view.size() >= 1) && (std::isalpha(_view[0])))
		{
			_view	= _view.substr(1);
		}
	}

	// 尻尾を削る
	if ((_view.size() >= 1) && (_view.back() == '>'))
	{
		_view	= _view.substr(0, _view.size() - 1);
		if ((_view.size() >= 1) && (_view.back() == '/'))
		{
			_view	= _view.substr(0, _view.size() - 1);
		}
	}

	return	_view;
}



AMTRS_LANGUAGE_XML_NAMESPACE_END
#endif

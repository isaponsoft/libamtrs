/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__passion__lexar__hpp
#define	__libamtrs__passion__lexar__hpp
#include "sourcecode.hpp"
PASSION_NAMESPACE_BEGIN



struct	utf8_traits
{
	using	pointer			= char*;
	using	const_pointer	= char const*;


	int32_t read(const_pointer& _cur, const_pointer _end)
	{
		const uint8_t*	cur	= reinterpret_cast<const uint8_t*>(_cur);
		uint8_t			c	= *cur;
		auto			s	= std::distance(_cur, _end);
		if ( c         <= 0x7F)
		{
			_cur	+= 1;
			return	c;
		}

		if ((c & 0xE0) == 0xC0)
		{
			if (s < 2)
			{
				return	0;
			}
			int32_t	x	= (static_cast<int32_t>(cur[0] & 0x1F) << 6)
						| (static_cast<int32_t>(cur[1] & 0x3F) << 0);
			_cur	+= 2;
			return	x;
		}

		if ((c & 0xF0) == 0xE0)
		{
			if (s < 3)
			{
				return	0;
			}
			int32_t	x	= (static_cast<int32_t>(cur[0] & 0x1F) <<12)
						| (static_cast<int32_t>(cur[1] & 0x3F) << 6)
						| (static_cast<int32_t>(cur[2] & 0x3F) << 0);
			_cur	+= 3;
			return	x;
		}

		if ((c & 0xF8) == 0xF0)
		{
			if (s < 4)
			{
				return	0;
			}
			int32_t	x	= (static_cast<int32_t>(cur[0] & 0x1F) <<18)
						| (static_cast<int32_t>(cur[1] & 0x3F) <<12)
						| (static_cast<int32_t>(cur[2] & 0x3F) << 6)
						| (static_cast<int32_t>(cur[3] & 0x3F) << 0);
			_cur	+= 4;
			return	x;
		}
		return	0;
	}
};





class	lexar
{
public:
	using	char_type		= int32_t;
	using	const_pointer	= vmsourcecode::const_pointer;
	using	char_traits		= utf8_traits;

	struct	cursor
	{
		const_pointer	cur;
		int32_t			line;
	};

	struct	compare_result
	{
		size_t			length = 0;
		bool empty() const noexcept { return length == 0; }
	};

	lexar(vmsourcecode& _source)
		: mSource(_source)
		, mCursor({_source.begin(), 1})
	{}

	void rollback()
	{
		mCursor	= mRollback;
	}

	cursor position() const noexcept { return mCursor; }
	void position(cursor _cur) { mCursor = _cur; }

	template<class T>
	compare_result peek_compare(cursor& _next, T _txt)
	{
		std::string_view	txt(_txt);

		cursor		cur(mCursor);
		cursor		tmp(mCursor);
		while (is_blunk{}(read(tmp)))
		{
			cur	= tmp;
		}
		if (txt.size() > (size_t)std::distance(cur.cur, end()))
		{
			return	{};
		}
		if (txt != std::string_view(cur.cur, txt.size()))
		{
			return	{};
		}
		cur.cur	+= txt.size();
		_next	= cur;
		return	{txt.size()};
	}

	template<class T>
	compare_result peek_compare(T _txt)
	{
		cursor	c;
		return	peek_compare(c, _txt);
	}

	template<class Rule, class... Rules>
	std::string_view read_string()
	{
		mRollback	= mCursor;

		// 現在の位置を保存
		cursor		backup(mCursor);

		// 空白をスキップ
		cursor		cur(mCursor);
		while (is_blunk{}(read(cur)))
		{
			mCursor	= cur;
		}

		cursor				beg(mCursor);
		std::string_view	retval;
		if (_read<Rule, Rules...>(retval, beg))
		{
			return	retval;
		}

		// 何も読み込めなかったので元に戻す
		mCursor	= backup;
		return	std::string_view(mCursor.cur, 0);
	}


	template<class Rule, class... Rules>
	std::string_view peek_string(cursor& _next)
	{
		_next		= mCursor;

		// 現在の位置を保存
		cursor		backup(mCursor);

		// 空白をスキップ
		cursor		cur(mCursor);
		while (is_blunk{}(read(cur)))
		{
			mCursor	= cur;
		}

		cursor				beg(mCursor);
		std::string_view	retval(mCursor.cur, 0);
		if (_read<Rule, Rules...>(retval, beg))
		{
			_next		= mCursor;
		}
		mCursor	= backup;
		return	retval;
	}


	template<class Token, class Rule, class... Rules>
	Token read_token()
	{
		mRollback	= mCursor;

		// 現在の位置を保存
		cursor		backup(mCursor);

		// 空白をスキップ
		cursor		cur(mCursor);
		while (is_blunk{}(read(cur)))
		{
			mCursor	= cur;
		}

		cursor		beg(mCursor);
		Token		retval;
		if (_read<Rule, Rules...>(retval, beg))
		{
			return	retval;
		}

		// 何も読み込めなかったので元に戻す
		mCursor	= backup;
		return	Token(mSource.filename, mCursor.line, {mCursor.cur, 0});
	}

	template<class Token, class Rule, class... Rules>
	Token peek_token()
	{
		cursor		backup(mCursor);
		Token		ret	= read_token<Token, Rule, Rules...>();
		mCursor		= backup;
		return	ret;
	}

	char_type read()
	{
		return	read(mCursor);
	}


	char_type read(cursor& _cursor)
	{
		char_type		c	= 0;
		c	= char_traits{}.read(_cursor.cur, end());
		if (c == '\n')
		{
			++_cursor.line;
		}
		return	c;
	}


	const_pointer end() const noexcept { return mSource.end(); }

	bool empty() const noexcept { return mCursor.cur == end(); }

protected:
	template<class Rule, class Token>
	bool _read(Token& _token, cursor const& _beg)
	{
		if (_read<Rule>())
		{
			_token	= Token(mSource.filename, _beg.line, {_beg.cur, (size_t)(mCursor.cur - _beg.cur)});
			return	true;
		}
		return	false;
	}

	template<class Rule>
	bool _read(std::string_view& _token, cursor const& _beg)
	{
		if (_read<Rule>())
		{
			_token	= std::string_view(_beg.cur, (size_t)(mCursor.cur - _beg.cur));
			return	true;
		}
		return	false;
	}


	template<class Rule, class Rules2, class... Rules, class Token>
	bool _read(Token& _token, cursor const& _beg)
	{
		if (_read<Rule>(_token, _beg))
		{
			return	true;
		}
		if (_read<Rules2, Rules...>(_token, _beg))
		{
			return	true;
		}
		return	false;
	}



	template<class Rules>
	bool _read()
	{
		cursor		cur(mCursor);
		Rules		tt;
		if (!tt.first(read(cur)))
		{
			return	false;
		}

		cursor	tmp(cur);
		while (tt.second(read(tmp)))
		{
			cur	= tmp;
		}

		if (!tt.terminate(read(tmp)))
		{
			return	false;
		}

		mCursor	= cur;
		return	true;
	}


	vmsourcecode&	mSource;
	cursor			mCursor;
	cursor			mRollback;
};


//! 式を読み取り正規化した文字列を返します。
vmstring read_expression(int& _error, lexar& _lexar);


//! 型名を読み取り正規化した文字列を返します。
vmstring read_typename(int& _error, lexar& _lexar);


PASSION_NAMESPACE_END
#endif

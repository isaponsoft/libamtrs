/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__passion__token_buffer__hpp
#define	__passion__token_buffer__hpp
#include <stack>
#include "token.hpp"
PASSION_NAMESPACE_BEGIN


// ============================================================================
//!	語句を蓄えておくバッファ。
// ----------------------------------------------------------------------------
//!	std::deque<> のラッパー。データが空っぽの時に EOFTOKEN を返す他、EOFTOKEN
//!	や空白、改行など文法に影響のないものを無視する。
// ----------------------------------------------------------------------------
class	tokenbuffer
{
public:
	using	value_type	= Token;
	using	token_type	= typename value_type::Type;

	static tokenbuffer create(const source_location& _source)
	{
		tokenbuffer		thiz;
		TextReader		reader(_source);
		reader.setCommentEnable(false);
		for (;;)
		{
			value_type	token = reader.read();
			if (token == token_type::EOFTOKEN)
			{
				break;
			}
			thiz.push_back(token);
		}
		return	thiz;
	}


	value_type fetch()
	{
		return	mTokens.size()
				? mTokens.front()
				: value_type(token_type::EOFTOKEN, "[EOFTOKEN]", { "Unkown", "", 0 });
	}

	value_type read()
	{
		if (!mTokens.size())
		{
			return	value_type(token_type::EOFTOKEN, "[EOFTOKEN]", { "Unkown", "", 0 });
		}
		value_type	token = mTokens.front();
		mTokens.pop_front();
		return	token;
	}

	//! 指定する種類のトークンをスキップします。
	void skip(unsigned int _types)
	{
		while (!empty() && (fetch().type() & _types))
		{
			read();
		}
	}

	//! 指定する種類のトークンをスキップします。
	//! スキップしたトークンを _buff に格納します。
	void skip(tokenbuffer& _buff, unsigned int _types)
	{
		while (!empty() && (fetch().type() & _types))
		{
			_buff.push_back(fetch());
			read();
		}
	}

	void pop_front()
	{
		mTokens.pop_front();
	}


	void push_back(const value_type& token) { if (token != token_type::EOFTOKEN) { mTokens.push_back(token); } }
	void push_back(const tokenbuffer& _other) { mTokens.insert(mTokens.end(), _other.mTokens.begin(), _other.mTokens.end()); }
	template<class It>
	void push_back(It _first, It _last) { mTokens.insert(mTokens.end(), _first, _last); }
	void push_front(const value_type& token) { if (token != token_type::EOFTOKEN) { mTokens.push_front(token); } }
	std::size_t count() const noexcept { return size(); }
	std::size_t size() const noexcept { return mTokens.size(); }

	bool empty() const noexcept { return size() == 0; }

	tokenbuffer() {}
	tokenbuffer(const tokenbuffer& ) = delete;
	tokenbuffer(      tokenbuffer&&) = default;
	tokenbuffer& operator = (const tokenbuffer& ) = delete;
	tokenbuffer& operator = (      tokenbuffer&&) = default;
private:
	std::deque<value_type>	mTokens;
};



class	tokenreader : public Token
{
public:
	using	value_type	= Token;
	using	token_type	= typename value_type::Type;

	tokenreader()
		: value_type(token_type::EOFTOKEN)
	{}


	tokenreader& operator ++ ()
	{
		if (empty())
		{
			return	*this;
		}
	
		mBuffers.top().pop_front();
		while (!mBuffers.empty() && mBuffers.top().empty())
		{
			mBuffers.pop();
		}
		if (mBuffers.empty())
		{
			*static_cast<Token*>(this)	= value_type(token_type::EOFTOKEN, "[EOFTOKEN]", { "Unkown", "", 0 });
		}
		else
		{
			*static_cast<Token*>(this)	= mBuffers.top().fetch();
		}
		return	*this;
	}

	//! 指定する種類のトークンをスキップします。
	void skip(unsigned int _types)
	{
		while (!empty() && (this->type() & _types))
		{
			++(*this);
		}
	}

	//! 指定する種類のトークンをスキップします。
	//! スキップしたトークンを _buff に格納します。
	void skip(tokenbuffer& _buff, unsigned int _types)
	{
		while (!empty() && (this->type() & _types))
		{
			_buff.push_back(*this);
			++(*this);
		}
	}


	void push(tokenbuffer&& _buffer)
	{
		if (!_buffer.empty())
		{
			mBuffers.push(std::move(_buffer));
			*static_cast<Token*>(this)	= mBuffers.top().fetch();
		}
	}

	void push(value_type _v)
	{
		if (mBuffers.empty())
		{
			mBuffers.push({});
		}
		mBuffers.top().push_front(_v);
	}


	bool empty() const noexcept { return *this == token_type::EOFTOKEN; }

private:
	std::stack<tokenbuffer>	mBuffers;
};





PASSION_NAMESPACE_END
#endif

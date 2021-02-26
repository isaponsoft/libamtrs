/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__xcode__pbxedit__hpp
#define	__xcode__pbxedit__hpp
#include <amtrs/amtrs.hpp>
#include <amtrs/crypto/sha256.hpp>
#include <amtrs/filesystem/@>
#include <amtrs/string/@>
using namespace amtrs;


template<class StringT>
class	basic_pbxroot;

template<class StringT>
class	basic_pbxarray;

template<class StringT>
class	basic_pbxdict;

template<class StringT>
class	basic_objects;

template<class ValueT, char Open, char Close>
class	basic_pbxkv;


template<class StringT>
class	basic_pbxedit
{
public:
	using	string_type	= StringT;

	basic_pbxedit()
		: mData(nullptr)
		, mPosition(0)
		, mLength(0)
	{}

	basic_pbxedit(basic_pbxedit const&) = default;
	basic_pbxedit& operator = (basic_pbxedit const&) = default;

	basic_pbxedit(string_type* _txt)
		: mData(_txt)
		, mPosition(0)
		, mLength(_txt->size())
	{}


	basic_pbxedit(string_type* _txt, size_t _position)
		: mData(_txt)
		, mPosition(_position)
		, mLength(_txt->size() - _position)
	{}


	basic_pbxedit(string_type* _txt, size_t _position, size_t _length)
		: mData(_txt)
		, mPosition(_position)
		, mLength(_length)
	{}

	template<class R>
	basic_pbxedit(R const& _r)
		: mData(_r.mData)
		, mPosition(_r.mPosition)
		, mLength(_r.mLength)
	{}



	bool good() const noexcept { return mData != nullptr; }

	bool empty() const noexcept { return mLength == 0; }
	char const* data() const noexcept { return mData->data() + mPosition; }
	size_t size() const noexcept { return mLength; }
	char const* begin() const noexcept { return data(); }
	char const* end() const noexcept { return begin() + mLength; }

	std::string_view view() const noexcept
	{
		return	{data(), size()};
	}

	basic_pbxedit substr(size_t _pos) const noexcept
	{
		return	basic_pbxedit(mData, mPosition + _pos, mLength - _pos);
	}

	basic_pbxedit substr(size_t _pos, size_t _len) const noexcept
	{
		return	basic_pbxedit(mData, mPosition + _pos, _len);
	}


	//! 最後尾にテキストを追加します。
	//! 文字列を追加した新しいエディタを返します。
	basic_pbxedit push_back(string_type const& _s)
	{
		if (!good())
		{
			return	*this;
		}

		auto		pos	= mPosition + mLength;
		string_type	retval(mData->substr(0, pos));
		retval += _s;
		retval += std::string_view(*mData).substr(pos);
		*mData	= retval;
		return	basic_pbxedit(mData, mPosition, mLength + _s.size());	
	}

protected:
	string_type*	mData		= nullptr;	//!< テキストデータ
	size_t			mPosition	= 0;		//!< ブロックの先頭位置
	size_t			mLength		= 0;		//!< ブロックの長さ
};


template<class ValueT, char Open, char Close>
class	basic_pbxkv
		: public basic_pbxedit<typename ValueT::string_type>
{
public:
	using	string_type	= typename ValueT::string_type;
	using	super_type	= basic_pbxedit<string_type>;
	using	value_type	= ValueT;
	using	super_type::super_type;

	basic_pbxkv(basic_pbxedit<string_type> const& _r)
		: super_type(_r)
	{}


	// () または {} で囲まれた部分を抽出する
	value_type value()
	{
		auto	bpos	= this->view().find_first_of(Open);
		if (bpos == std::string_view::npos)
		{
			return	{};
		}
		++bpos;
		auto	epos	= this->substr(bpos).view().find_last_of(Close);
		if (epos == std::string_view::npos)
		{
			return	{};
		}
		return	(value_type)this->substr(bpos, epos);
	}
protected:
};



template<class StringT>
class	basic_pbxarray
		: public basic_pbxedit<StringT>
{
public:
	using	string_type	= StringT;
	using	super_type	= basic_pbxedit<StringT>;
	using	super_type::super_type;

	basic_pbxarray(basic_pbxedit<StringT> const& _r)
		: super_type(_r)
	{}
};



template<class StringT>
class	basic_pbxdict
		: public basic_pbxedit<StringT>
{
public:
	using	string_type	= StringT;
	using	super_type	= basic_pbxedit<StringT>;
	using	super_type::super_type;

	basic_pbxdict(basic_pbxedit<StringT> const& _r)
		: super_type(_r)
	{}


	// 指定する名前を持つ配列を抽出する
	basic_pbxkv<basic_pbxarray<string_type>, '(', ')'> find_array(std::string const& _name)
	{
		std::cmatch			m;
		std::string			r	= format<std::string>("%s *(/\\*.*\\*/)* *= *\\(", _name.c_str());
		if (!std::regex_search(this->begin(), this->end(), m, std::regex(r)))
		{
			return	{};
		}

		basic_pbxdict		retval(this->mData, 0, 0);
		char const*	cur		= this->data() + m.position();
		char const*	beg		= cur;
		size_t		cc		= 0;
		for (;;)
		{
			if (*cur == '(')
			{
				++cc;
			}
			else if (*cur == ')')
			{
				--cc;
				if (cc == 0)
				{
					++cur;
					break;
				}
			}
			++cur;
		}
		retval.mPosition	= this->mPosition + m.position();
		retval.mLength		= cur - beg;
		return	retval;
	}


	//! "ID /* */ = { }" を抽出する
	basic_pbxkv<basic_pbxdict<string_type>, '{', '}'> find_dict(string_type const& _name)
	{
		std::cmatch		m;
		string_type		f	= format<string_type>("%s *(/\\*.*\\*/)* *= *\\{", _name.c_str());
		if (!std::regex_search(this->begin(), this->end(), m, std::regex(f)))
		{
			return	{};
		}

		basic_pbxdict<string_type>	retval(this->mData, 0, 0);
		char const*					cur		= this->data() + m.position();
		char const*					beg		= cur;
		size_t						cc		= 0;
		for (;;)
		{
			if (*cur == '{')
			{
				++cc;
			}
			else if (*cur == '}')
			{
				--cc;
				if (cc == 0)
				{
					++cur;
					break;
				}
			}
			++cur;
		}
		retval.mPosition	= this->mPosition + m.position();
		retval.mLength		= cur - beg;
		return	retval;
	}
};




template<class StringT>
class	basic_pbxsection
		: public basic_pbxdict<StringT>
{
public:
	using	string_type	= StringT;
	using	super_type	= basic_pbxdict<StringT>;
	using	super_type::super_type;

	basic_pbxsection(basic_pbxedit<StringT> const& _r)
		: super_type(_r)
	{}

	// セクションの内側を抽出する
	basic_pbxdict<string_type> inner()
	{
		std::cmatch		mb;
		std::cmatch		me;
		if (!std::regex_search(this->begin(), this->end(), mb, std::regex(R"(/\* Begin .+ section \*/)")))
		{
			return	{};
		}
		if (!std::regex_search(this->begin(), this->end(), me, std::regex(R"(/\* End .+ section \*/)")))
		{
			return	{};
		}
		return	this->substr(mb.position(), me.position() - mb.position());
	}
};



template<class StringT>
class	basic_objects
		: public basic_pbxdict<StringT>
{
public:
	using	string_type	= StringT;
	using	super_type	= basic_pbxdict<StringT>;
	using	super_type::super_type;

	basic_objects(basic_pbxedit<StringT> const& _r)
		: super_type(_r)
	{}


	// 指定するセクションを抽出する
	basic_pbxsection<string_type> find_section(string_type const& _section)
	{
		string_type	beginStr	= format<string_type>("/* Begin %s section */", _section.c_str());
		string_type	endStr		= format<string_type>("/* End %s section */", _section.c_str());
		auto	b	= this->view().find(beginStr);
		auto	e	= this->view().find(endStr);
		if (b == string_type::npos || e == string_type::npos)
		{
			return	{};
		}
		return	(basic_pbxsection<string_type>)this->substr(b, e - b + endStr.size());
	}
};



template<class StringT>
class	basic_pbxroot
		: public basic_pbxdict<StringT>
{
public:
	using	string_type	= StringT;
	using	super_type	= basic_pbxdict<StringT>;
	using	super_type::super_type;


	basic_objects<string_type> objects()
	{
		return	(basic_objects<string_type>)this->find_dict("objects").value();
	}

	string_type mainGroupCode()
	{
		std::cmatch	m;
		std::regex_search(this->begin(), this->end(), m, std::regex(R"(mainGroup *= *([0-9A-Za-z]+))"));
		return	m[1].str();
	}

	string_type rootObjectCode()
	{
		std::cmatch	m;
		std::regex_search(this->begin(), this->end(), m, std::regex(R"(rootObject *= *([0-9A-Za-z]+))"));
		return	m[1].str();
	}

	string_type generateFileId(std::string const& _hint)
	{
		uint8_t		buff[64];
		amtrs::crypto::sha256(buff, _hint.data(), _hint.size(), false);
		for (;;)
		{
			std::string	id((char*)buff, 24);
			if (this->mData->find(id) == string_type::npos)
			{
				return	id;
			}
			amtrs::crypto::sha256(buff, buff, sizeof(buff), false);
		}
	}

};


using	pbxedit	= basic_pbxedit<std::string>;
using	pbxroot	= basic_pbxroot<std::string>;

#endif

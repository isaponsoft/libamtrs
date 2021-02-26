/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__passion__token__hpp
#define	__passion__token__hpp
#include "source_location.hpp"
PASSION_NAMESPACE_BEGIN

class	Token
		: public source_location
{
public:
	using	location_type	= location;

	enum	Type
	{
		Error		= -1,
		UNKOWN		= 0,
		EOFTOKEN	= (1<<0),
		NUMBER		= (1<<1),
		STRING		= (1<<2),
		CODE		= (1<<3),
		COMMAND		= (1<<4),
		SPACE		= (1<<5),
		RETURN		= (1<<6),
		COMMENT		= (1<<7),
	};

	const location_type& location() const noexcept { return *this; }

	Token() = default;

	Token(Type _type, vmstring _token = {}, const source_location& _location = {})
		: source_location(_location)
		, mType(_type)
		, mNumber(0)
		, mString(std::move(_token))
	{}

	Token(Type _type, vmstring _token, int64_t _number , const source_location& _location = {})
		: source_location(_location)
		, mType(_type)
		, mNumber(_number)
		, mString(std::move(_token))
	{}

	Type type() const noexcept
	{
		return	mType;
	}

	bool is_identifer() const noexcept { return *this == Type::COMMAND; } 
	bool is_operator() const noexcept { return *this == Type::CODE; } 

	bool operator == (const Token& _r) const noexcept
	{
		if (_r.mType == EOFTOKEN && mType == EOFTOKEN)
		{
			return	true;
		}
		if (_r.mType != mType)
		{
			return	false;
		}
		return	mString == _r.mString;
	}

	bool operator != (const Token& _r) const noexcept
	{
		if (_r.mType == EOFTOKEN && mType == EOFTOKEN)
		{
			return	false;
		}
		if (_r.mType != mType)
		{
			return	true;
		}
		return	mString != _r.mString;
	}

	bool operator == (std::string_view _r) const noexcept	{ return mString == _r; 	}
	bool operator != (std::string_view _r) const noexcept	{ return mString != _r; 	}
	bool operator == (const char* _r) const					{ return mString == _r; 	}
	bool operator != (const char* _r) const					{ return mString != _r; 	}
	bool operator == (Type _r) const noexcept				{ return mType == _r;		}
	bool operator != (Type _r) const noexcept				{ return mType != _r;		}
	int64_t number() const noexcept							{ return mNumber;			}
	void neg() noexcept										{ mNumber = -mNumber;		}
	const vmstring& str() const noexcept					{ return mString;			}

	//!	エラー用に文字列を設定します
	void set_error_string(vmstring _msg)
	{
		mString	= std::move(_msg);
	}

	operator const vmstring& () const noexcept { return mString; }

	auto c_str() const noexcept { return mString.c_str(); }

private:
	Type			mType		= UNKOWN;
	int64_t			mNumber		= 0;
	vmstring		mString;
};


PASSION_NAMESPACE_END
#endif

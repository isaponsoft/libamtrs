/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__passion__token__hpp
#define	__libamtrs__passion__token__hpp
#include "string.hpp"
PASSION_NAMESPACE_BEGIN

template<class Traits>
class	basic_vmtoken : public Traits
{
public:
	using	traits_type		= Traits;
	using	token_type		= typename traits_type::token_type;
	using	view_type		= vmstring::view_type;

	basic_vmtoken() = default;
	basic_vmtoken(basic_vmtoken const&) = default;
	basic_vmtoken(basic_vmtoken&&) = default;
	basic_vmtoken& operator = (basic_vmtoken const&) = default;
	basic_vmtoken& operator = (basic_vmtoken&&) = default;

	basic_vmtoken(vmstring _filename, int32_t _line, vmstring::view_type _token)
		: mToken(_token)
		, mFilename(_filename)
		, mLine(_line)
	{}

	auto type() const noexcept { return mType; }
	auto string() const noexcept { return mToken; }
	auto filename() const noexcept { return mFilename; }
	auto line() const noexcept { return mLine; }
	bool empty() const noexcept { return string().empty(); }

protected:
	view_type		mToken;
	vmstring		mFilename;
	int32_t			mLine;
	token_type		mType;
};


struct	passion_token_traits
{
	enum	token_type
	{
		tt_none			= 0,
		tt_keyword		= 1,
		tt_string		= 2,
		tt_decimal		= 3,
		tt_opecode		= 4,
		tt_bracket		= 5,
		tt_real			= 6,
		tt_hex			= 7,
	};
};


using	vmtoken	= basic_vmtoken<passion_token_traits>;

PASSION_NAMESPACE_END
#endif

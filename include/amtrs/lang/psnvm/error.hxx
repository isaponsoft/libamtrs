/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__lang__psnvm__error__hxx
#define	__libamtrs__lang__psnvm__error__hxx
PSNVM_LANG_NAMESPACE_BEGIN

enum	errorcode
{
#define	PSNVMPARSER_ERRORCODE(_name, _code)			_name = (_code),
#define	PSNVMPARSER_WARNCODE(_name, _code)			_name = (_code | (1<<24)),
	#define	PSNVMPARSER_UNTERMERROR(_name, _code)		_name = (_code | (2<<24)),					// 文などが途中で終わっている。対話モードで入力状態を維持するかどうかの判別に使用できる。
	#define	PSNVM_IS_UNTERMERROR(_name, _code)			_name = (((_code>>24) & 0xff) == 2),		// 文などが途中で終わっている。対話モードで入力状態を維持するかどうかの判別に使用できる。
	#include "errorcodes.hxx"
	#undef	PSNVMPARSER_ERRORCODE
	#undef	PSNVMPARSER_WARNCODE
	#undef	PSNVMPARSER_UNTERMERROR
	#undef	PSNVM_IS_UNTERMERROR
	___max
};
#define	PSNVM_ERRORCODE(_code)			((int)(_code) & 0xffffff)


struct	ambitiousinfo;


struct	errorinfo
{
	struct	ambitiousinfo
	{
		text			location;
		shared_string	token;
	};

	text						location;
	errorcode					err;
	shared_string				token1;
	shared_string				token2;
	shared_string				token3;
	simplearray<ambitiousinfo>	ambitiouses;

	errorinfo(text _location = nullptr, errorcode _err = errorcode::ec_success, shared_string _t1 = nullptr, shared_string _t2 = nullptr, shared_string _t3 = nullptr)
		: location(_location)
		, err(_err)
		, token1(_t1), token2(_t2), token3(_t3)
	{}
	errorinfo(errorinfo const&) = default;
	errorinfo(errorinfo&&) = default;
};




// errorcode を文字列として取得します。例えば errorcode::ec_success は "ec_success" になります。
auto error_string(errorcode _e) -> text;


PSNVM_LANG_NAMESPACE_END
#endif

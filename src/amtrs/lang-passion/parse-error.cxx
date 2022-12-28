/* Copyright (c) 2022, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
PSNVM_LANG_NAMESPACE_BEGIN

auto error_string(errorcode _e) -> text
{
	switch (_e)
	{
		#define	PSNVMPARSER_ERRORCODE(_name, _code)			case _name : return text(#_name);
		#define	PSNVMPARSER_WARNCODE(_name, _code)			case _name : return text(#_name);
		#define	PSNVMPARSER_UNTERMERROR(_name, _code)		case _name : return text(#_name);
		#define	PSNVM_IS_UNTERMERROR(_name, _code)			case _name : return text(#_name);
		#include <amtrs/lang/psnvm/errorcodes.hxx>
		#undef	PSNVMPARSER_ERRORCODE
		#undef	PSNVMPARSER_WARNCODE
		#undef	PSNVMPARSER_UNTERMERROR
		#undef	PSNVM_IS_UNTERMERROR
		default:		return	text("");
	}
}


PSNVM_LANG_NAMESPACE_END

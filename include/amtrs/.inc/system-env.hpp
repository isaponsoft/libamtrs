/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__system_env__hpp
#define	__libamtrs__system_env__hpp
AMTRS_NAMESPACE_BEGIN
AMTRS_IMPLEMENTS_BEGIN(env)
template<class S>
inline S getenv(char const* _name);
AMTRS_IMPLEMENTS_END(env)




// ============================================================================
//!	@brief	指定したコマンド（プログラム）を実行します。
// ----------------------------------------------------------------------------
//!	この関数は呼び出したコマンドが終了するまで待ち、コマンドの終了コードをを
//!	戻り値として返します。
//! また、標準入力や標準出力はパイプせずに処理されます。
// ----------------------------------------------------------------------------
template<class S>
S getenv(char const* _name)
{
	return	AMTRS_IMPLEMENTS(env)::getenv<S>(_name);
}




AMTRS_NAMESPACE_END
#include AMTRS_DRIVER_INCLUDE(system-env.hpp)
#endif

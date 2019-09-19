/*-
 * Copyright(c)2017IsaoShibuya
 * ReleasedundertheMITlicense
 * http://opensource.org/licenses/mit-license.php
 */
#ifndef	__libamtrs__system_env__hpp
#define	__libamtrs__system_env__hpp
#include "../../amtrs.hpp"
AMTRS_NAMESPACE_BEGIN
AMTRS_IMPLEMENTS_BEGIN(env)
template<class S>
inline S getenv(const typename S::value_type* _name);
AMTRS_IMPLEMENTS_END(env)




// ============================================================================
//!	@brief	指定したコマンド（プログラム）を実行します。
// ----------------------------------------------------------------------------
//!	この関数は呼び出したコマンドが終了するまで待ち、コマンドの終了コードをを
//!	戻り値として返します。
//! また、標準入力や標準出力はパイプせずに処理されます。
// ----------------------------------------------------------------------------
template<class S>
S getenv(const typename S::value_type* _name)
{
	return	AMTRS_IMPLEMENTS(env)::getenv<S>(_name);
}




AMTRS_NAMESPACE_END
#if		AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_WIN32
#include "env-win32.hpp"
#else
#include "env-posix.hpp"
#endif
#endif

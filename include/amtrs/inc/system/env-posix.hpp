/*-
 * Copyright(c)2017IsaoShibuya
 * ReleasedundertheMITlicense
 * http://opensource.org/licenses/mit-license.php
 */
#ifndef	__libamtrs__system__env_posix__hpp
#define	__libamtrs__system__env_posix__hpp
#include <stdio.h>
#include <stdlib.h>
#include "env.hpp"
AMTRS_NAMESPACE_BEGIN
AMTRS_IMPLEMENTS_BEGIN(env)

template<class S>
inline S getenv(const typename S::value_type* _name)
{
	auto	e = ::getenv(_name);
	return	e ? S{e} : S{};
}


AMTRS_IMPLEMENTS_END(env)
AMTRS_NAMESPACE_END
#endif

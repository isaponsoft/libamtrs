/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__basic_type__def__hpp
#define	__libamtrs__basic_type__def__hpp
#ifndef	AMTRS_NAMESPACE
#define	AMTRS_NAMESPACE			amtrs
#endif

#define	AMTRS_NAMESPACE_BEGIN			namespace AMTRS_NAMESPACE {
#define	AMTRS_NAMESPACE_END				}
#define	AMTRS_STD_NAMESPACE_BEGIN		namespace std {
#define	AMTRS_STD_NAMESPACE_END			}

#define	AMTRS_IMPLEMENTS_BEGIN(_name)			namespace _name##_impl {
#define	AMTRS_IMPLEMENTS_END(_name)				}
#define	AMTRS_IMPLEMENTS(_name)					_name##_impl

#include "../../os/def.hpp"

#endif

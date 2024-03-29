﻿/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__base__namespace__hpp
#define	__libamtrs__base__namespace__hpp

/* namespace */
#ifndef	AMTRS_NAMESPACE
#define	AMTRS_NAMESPACE							amtrs
#endif

#define	AMTRS_NAMESPACE_BEGIN					namespace AMTRS_NAMESPACE {
#define	AMTRS_NAMESPACE_END						}
AMTRS_NAMESPACE_BEGIN
AMTRS_NAMESPACE_END

#define	AMTRS_STD_NAMESPACE_BEGIN				namespace std {
#define	AMTRS_STD_NAMESPACE_END					}




/* namespaces */

/* chrono */
#define	AMTRS_CHRONO_NAMESPACE					AMTRS_NAMESPACE::chrono
#define	AMTRS_CHRONO_NAMESPACE_BEGIN			AMTRS_NAMESPACE_BEGIN namespace chrono {
#define	AMTRS_CHRONO_NAMESPACE_END				} AMTRS_NAMESPACE_END
AMTRS_CHRONO_NAMESPACE_BEGIN
AMTRS_CHRONO_NAMESPACE_END

/* console */
#define	AMTRS_CONSOLE_NAMESPACE					AMTRS_NAMESPACE::console
#define	AMTRS_CONSOLE_NAMESPACE_BEGIN			AMTRS_NAMESPACE_BEGIN namespace console {
#define	AMTRS_CONSOLE_NAMESPACE_END				} AMTRS_NAMESPACE_END
AMTRS_CONSOLE_NAMESPACE_BEGIN
AMTRS_CONSOLE_NAMESPACE_END

/* crypto */
#define	AMTRS_CRYPTO_NAMESPACE					AMTRS_NAMESPACE::crypto
#define	AMTRS_CRYPTO_NAMESPACE_BEGIN			AMTRS_NAMESPACE_BEGIN namespace crypto {
#define	AMTRS_CRYPTO_NAMESPACE_END				} AMTRS_NAMESPACE_END
AMTRS_CRYPTO_NAMESPACE_BEGIN
AMTRS_CRYPTO_NAMESPACE_END

/* filesystem */
#define	AMTRS_FILESYSTEM_NAMESPACE				AMTRS_NAMESPACE::filesystem
#define	AMTRS_FILESYSTEM_NAMESPACE_BEGIN		AMTRS_NAMESPACE_BEGIN namespace filesystem {
#define	AMTRS_FILESYSTEM_NAMESPACE_END			} AMTRS_NAMESPACE_END
#define	AMTRS_FILESYSTEM_ZIP_NAMESPACE_BEGIN	AMTRS_FILESYSTEM_NAMESPACE_BEGIN namespace zip {
#define	AMTRS_FILESYSTEM_ZIP_NAMESPACE_END		} AMTRS_FILESYSTEM_NAMESPACE_END
AMTRS_FILESYSTEM_NAMESPACE_BEGIN
AMTRS_FILESYSTEM_NAMESPACE_END

/* g3d */
#define	AMTRS_G3D_NAMESPACE						AMTRS_NAMESPACE::g3d
#define	AMTRS_G3D_NAMESPACE_BEGIN				AMTRS_NAMESPACE_BEGIN namespace g3d {
#define	AMTRS_G3D_NAMESPACE_END					} AMTRS_NAMESPACE_END
AMTRS_G3D_NAMESPACE_BEGIN
AMTRS_G3D_NAMESPACE_END

/* graphics */
#define	AMTRS_GRAPHICS_NAMESPACE				AMTRS_NAMESPACE::graph
#define	AMTRS_GRAPHICS_NAMESPACE_BEGIN			AMTRS_NAMESPACE_BEGIN namespace graph {
#define	AMTRS_GRAPHICS_NAMESPACE_END			} AMTRS_NAMESPACE_END
AMTRS_GRAPHICS_NAMESPACE_BEGIN
AMTRS_GRAPHICS_NAMESPACE_END

/* io */
#define	AMTRS_IO_NAMESPACE						AMTRS_NAMESPACE::io
#define	AMTRS_IO_NAMESPACE_BEGIN				AMTRS_NAMESPACE_BEGIN namespace io {
#define	AMTRS_IO_NAMESPACE_END					} AMTRS_NAMESPACE_END
AMTRS_IO_NAMESPACE_BEGIN
AMTRS_IO_NAMESPACE_END

/* os */
#define	AMTRS_OS_NAMESPACE_BEGIN				AMTRS_NAMESPACE_BEGIN namespace os {
#define	AMTRS_OS_NAMESPACE_END					} AMTRS_NAMESPACE_END

/* string */
#define	AMTRS_STRING_NAMESPACE					AMTRS_NAMESPACE::strutil
#define	AMTRS_STRING_NAMESPACE_BEGIN			AMTRS_NAMESPACE_BEGIN namespace strutil {
#define	AMTRS_STRING_NAMESPACE_END				} AMTRS_NAMESPACE_END
AMTRS_STRING_NAMESPACE_BEGIN
AMTRS_STRING_NAMESPACE_END

/* sourcecode */
#define	AMTRS_SOURCECODE_NAMESPACE				AMTRS_NAMESPACE::sourcecode
#define	AMTRS_SOURCECODE_NAMESPACE_BEGIN		AMTRS_NAMESPACE_BEGIN namespace sourcecode {
#define	AMTRS_SOURCECODE_NAMESPACE_END			} AMTRS_NAMESPACE_END
AMTRS_SOURCECODE_NAMESPACE_BEGIN
AMTRS_SOURCECODE_NAMESPACE_END

/* system */
#define	AMTRS_SYSTEM_NAMESPACE					AMTRS_NAMESPACE::system
#define	AMTRS_SYSTEM_NAMESPACE_BEGIN			AMTRS_NAMESPACE_BEGIN namespace system {
#define	AMTRS_SYSTEM_NAMESPACE_END				} AMTRS_NAMESPACE_END
AMTRS_SYSTEM_NAMESPACE_BEGIN
AMTRS_SYSTEM_NAMESPACE_END

/* typeutil */
#define	AMTRS_TYPEUTIL_NAMESPACE				AMTRS_NAMESPACE::typeutil
#define	AMTRS_TYPEUTIL_NAMESPACE_BEGIN			AMTRS_NAMESPACE_BEGIN namespace typeutil {
#define	AMTRS_TYPEUTIL_NAMESPACE_END			} AMTRS_NAMESPACE_END
AMTRS_TYPEUTIL_NAMESPACE_BEGIN
AMTRS_TYPEUTIL_NAMESPACE_END


#define	AMTRS_IMPLEMENTS_BEGIN(_name)			namespace _name##_impl {
#define	AMTRS_IMPLEMENTS_END(_name)				}
#define	AMTRS_IMPLEMENTS(_name)					_name##_impl

#endif

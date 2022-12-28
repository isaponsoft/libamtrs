/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__lang__passion__namespace__hpp
#define	__libamtrs__lang__passion__namespace__hpp
#define	PSNVM_NAMESPACE					psnvm
#define	PSNVM_NAMESPACE_BEGIN			namespace psnvm {
#define	PSNVM_NAMESPACE_END				}
#define	PSNVM_PREPRO_NAMESPACE_BEGIN	PSNVM_NAMESPACE_BEGIN namespace pp {
#define	PSNVM_PREPRO_NAMESPACE_END		} PSNVM_NAMESPACE_END
#define	PSNVM_LANG_NAMESPACE_BEGIN		PSNVM_NAMESPACE_BEGIN namespace lang {
#define	PSNVM_LANG_NAMESPACE_END		} PSNVM_NAMESPACE_END
#define	PSNVM_UTILS_NAMESPACE_BEGIN		PSNVM_NAMESPACE_BEGIN namespace utils {
#define	PSNVM_UTILS_NAMESPACE_END		} PSNVM_NAMESPACE_END

#if		defined(PSNVM_EXPORT_DLL)
#define	PSNVM_API		AMTRS_API
#endif

#if		defined(PSNVM_IMPORT_DLL)
#define	PSNVM_API		AMTRS_API
#endif

#if		!defined(PSNVM_API)
#define	PSNVM_API
#endif

#if		__cplusplus >= 201500L
#define	psnvm_constexpr	amtrs_constexpr
#else
#define	psnvm_constexpr
#endif
#endif

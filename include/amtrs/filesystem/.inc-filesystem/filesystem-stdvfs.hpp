/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef __libamtrs__filesystem__stdvfs__hpp
#define __libamtrs__filesystem__stdvfs__hpp
AMTRS_FILESYSTEM_NAMESPACE_BEGIN


// ============================================================================
//! 標準ファイルシステムに対するインターフェース
// ----------------------------------------------------------------------------
//! std::filesystem を仮想化します。
// ----------------------------------------------------------------------------
class	stdvfs : public vfs
{
public:
	static ref<stdvfs> get_instance();
};


AMTRS_FILESYSTEM_NAMESPACE_END
#endif

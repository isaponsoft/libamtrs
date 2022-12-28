/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__billing__billing_manager_win32__hpp
#define	__libamtrs__billing__billing_manager_win32__hpp
#include <map>
#include <vector>
#include <amtrs/logging/@>
#include "def.hpp"

AMTRS_BILLING_NAMESPACE_BEGIN
class	billing_manager_win32
		: public billing_manager
{
public:
	friend	billing_manager;
};


// ========================================================================
//! 初期化を行う。
// ------------------------------------------------------------------------
inline billing_manager::billing_manager()
{
}


// ========================================================================
//! 課金システムに接続済みか調べる。
// ------------------------------------------------------------------------
inline bool billing_manager::connected() const noexcept
{
	return	false;
}


// ========================================================================
//! 課金システムへ接続する。
// ------------------------------------------------------------------------
//! 接続が終わるとコールバックを呼び出す。
// ------------------------------------------------------------------------
inline void billing_manager::connect(std::function<void(status _status)> _callback)
{
}


inline void billing_manager::purchase(std::function<void(status _status, purchase_result const* _details, size_t _detailsCount)> _callback, std::string_view _sku)
{
}


inline void billing_manager::loadSkuDetails(std::function<void(sku_detail const* _details, size_t _detailsCount)> _callback, implicit_virtual_iterator<std::string_view>& _first, implicit_virtual_iterator<std::string_view>& _last)
{
}


// ========================================================================
//! 課金システムとの接続を解除する。
// ------------------------------------------------------------------------
inline void billing_manager::disconnect()
{
}

AMTRS_BILLING_NAMESPACE_END
#endif

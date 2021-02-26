/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__billing__billing_manager__hpp
#define	__libamtrs__billing__billing_manager__hpp
#include <functional>
#include <string>
#include "../iterator/@"
#include "def.hpp"
#include "enum.hpp"
AMTRS_BILLING_NAMESPACE_BEGIN


struct	sku_detail
{
	std::string	identify;		//!< アイテムの識別ＩＤ
	std::string	price;			//!< プラットフォームで推奨する表示価格 ex) \123, 123 JPY, etc...
	std::string	priceCurrency;
	bool		rewarded;
};


class	billing_manager
{
public:
	enum class	status
	{
		success			= 0,		//!< 処理を終えた
		unkown_error	= 1,		//!< 不明なエラー
		network_error	= 2,		//!< ネットワーク接続が途切れた
		unsupported		= 3,		//!< 無効な操作
		limited			= 4,		//!< 購入制限がかかっている
	};

	enum class	purchase_status
	{
		purchased		= 0,		//!< 購入成功
		unkoen_error	= 1,		//!< 不明なエラー
		pending			= 2,		//!< 承認待ち／課金処理がペンディング中
		cancel			= 3,		//!< キャンセル
		disable			= 4,		//!< 無効になっている
		already			= 5,		//!< 購入済み
		not_owned		= 6,		//!< アイテムを所有していないので消費できない
		unspecified		= 7,		//!< 払い戻しが発生した
	};


	struct	purchase_result
	{
		purchase_status	state;
		std::string		identify;		//!< SKU.identify
		std::string		receipt;		//!< 購入時のレシートＩＤ(Androidなら Purchase.getPurchaseToken(), iOSならSKPaymentTransaction.transactionIdentifier)
	};



	static constexpr status		success			= status::success;
	static constexpr status		unkown_error	= status::unkown_error;

	billing_manager();
	billing_manager(const billing_manager&) = default;
	billing_manager(billing_manager&&) = default;
	billing_manager& operator = (const billing_manager&) = default;
	billing_manager& operator = (billing_manager&&) = default;

	// ========================================================================
	//! 課金システムに接続済みか調べる。
	// ------------------------------------------------------------------------
	bool connected() const noexcept;

	// ========================================================================
	//! 課金システムへ接続する。
	// ------------------------------------------------------------------------
	//! 接続が終わるとコールバックを呼び出す。
	// ------------------------------------------------------------------------
	void connect(std::function<void(status _status)> _callback = {});

	// ========================================================================
	//! 購入を開始する。
	// ------------------------------------------------------------------------
	//! 処理が終わるとコールバックを呼び出す。
	// ------------------------------------------------------------------------
	void purchase(std::function<void(status _status, purchase_result const* _details, size_t _detailsCount)> _callback, std::string_view _sku);


	// ========================================================================
	//! SKUの詳細を取得する
	// ------------------------------------------------------------------------
	//! 接続が終わるとコールバックを呼び出す。
	// ------------------------------------------------------------------------
	template<class Bucket>
	void loadSkuDetails(std::function<void(sku_detail const* _details, size_t _detailsCount)> _callback, Bucket _skuBucket)
	{
		auto	first	= make_implicit_virtual_iterator<std::string_view>(_skuBucket.begin());
		auto	last	= make_implicit_virtual_iterator<std::string_view>(_skuBucket.end());
		loadSkuDetails(std::move(_callback), first, last);
	}


	void loadSkuDetails(std::function<void(sku_detail const* _details, size_t _detailsCount)> _callback, implicit_virtual_iterator<std::string_view>& _first, implicit_virtual_iterator<std::string_view>& _last);


	// ========================================================================
	//! 課金システムとの接続を解除する。
	// ------------------------------------------------------------------------
	void disconnect();


protected:
	class	interface_base : public ref_object
	{};

	ref<interface_base>	mInterface;
};


AMTRS_BILLING_NAMESPACE_END
#include AMTRS_DRIVER_INCLUDE(billing-manager.hpp)
#endif

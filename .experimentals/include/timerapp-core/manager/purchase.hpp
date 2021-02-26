﻿/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libscheduler__purchase__purchase__hpp
#define	__libscheduler__purchase__purchase__hpp
#include <gframe/@>
namespace purchase
{

struct	purchase_product
{
	std::string	productId;		//!< プロダクトID（プラットフォーム用）
	std::string	identify;		//!< アプリ内識別ID
	std::string	price;			//!< 価格（表示用）
	std::string	title;			//!< タイトル（言語設定に従う）
	std::string	icon;			//!< アイコン
	std::string	comment;		//!< 詳細（言語設定に従う）
	bool		rewarded;		//!< 購入済みなら true
};


enum class	connection_status
{
	ok			= 0,
	fail		= 1,
};


enum class	purchase_status
{
	purchased	= 0,			//!< 購入した
	already		= 1,			//!< すでに購入済み
	pending		= 2,			//!< 購入が保留（ペアレントコントロールなど）
	unspecified	= 3,			//!< 払い戻しが行われた
	cancel		= 4,			//!< ユーザーによってキャンセルされた
	unproduct	= 5,			//!< 購入制限もしくはアイテムが公開停止もしくは非サポート
	failnetwork	= 6,			//!< 通信失敗
};


// ==================================================================
//! 課金処理システムの状態変更通知を受け取るためのリスナ
// ------------------------------------------------------------------
class	purchase_listener
{
public:
};


// ==================================================================
//! 課金処理システムを初期化します。
// ------------------------------------------------------------------
void initialize(std::string const& _script_filename);


// ==================================================================
//! 課金処理システムを開始します。
// ------------------------------------------------------------------
//! 通常はアプリケーション開始／再開時に呼び出すことが推奨です。
// ------------------------------------------------------------------
void resume(std::function<void(connection_status, purchase_product const*, size_t)> _callback);


// ==================================================================
//! 課金処理システムを終了します。
// ------------------------------------------------------------------
//! 通常はアプリケーション終了／一時停止時に呼び出すことが推奨です。
// ------------------------------------------------------------------
void stop();


// ==================================================================
//! 通信中かどうか調べます。
// ------------------------------------------------------------------
bool connecting();


// ==================================================================
//! 購入可能な商品の一覧を返します。
// ------------------------------------------------------------------
std::pair<purchase_product const*, size_t> get_products();


// ==================================================================
//! リスナを設定します。
// ------------------------------------------------------------------
void set_purchase_listener(purchase_listener* _listener);


// ==================================================================
//! 指定するプロダクトIDもしくは識別子のアイテムを返します。
// ------------------------------------------------------------------
purchase_product const* find_product(std::string const& _id);


// ==================================================================
//! 購入処理を行います。
// ------------------------------------------------------------------
void start_purchase(std::string const& _id, std::function<void(purchase_status)> _callback);


// ==================================================================
//! 購入済みの復元を行います。
// ------------------------------------------------------------------
void restore_purchase(std::function<void(std::string const*, size_t)> _callback);

}
#endif

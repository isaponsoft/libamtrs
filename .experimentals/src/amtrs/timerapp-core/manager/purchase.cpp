/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <thread>
#include <amtrs/billing/@>
#include <timerapp-core/manager/purchase.hpp>
using namespace amtrs;
namespace purchase {

using	bm_type	= amtrs::billing::billing_manager;

// ****************************************************************************
// local declare

struct	billingdata : ref_object
{
	bm_type								bm;
	std::thread							conn;
	bool								connecting	= false;
	bool								modified	= false;
	std::vector<purchase_product>		productsCache;
	std::vector<purchase_product>		products;	//!< スクリプトに書かれていたプロダクト
	std::vector<billing::sku_detail>	skus;		//!< プラットフォームから返されたプロダクト
};


// ****************************************************************************
// local function

static void load_product_script(billingdata* _dest, std::string const& _script_filename);


// ****************************************************************************
// local variable

static	ref<billingdata>	gBilling;




// ==================================================================
//! 課金処理システムを初期化します。
// ------------------------------------------------------------------
void initialize(std::string const& _script_filename)
{
	if (gBilling.empty())
	{
		gBilling	= new billingdata();
		load_product_script(gBilling, _script_filename);
	}
}


// ==================================================================
//! 課金処理システムを開始します。
// ------------------------------------------------------------------
void resume(std::function<void(connection_status, purchase_product const*, size_t)> _callback)
{
	gBilling->productsCache	= {};
	gBilling->skus			= {};

	gBilling->connecting	= true;
	gBilling->bm.connect([=](auto _status)
	{
		AMTRS_DEBUG_LOG("purchases connect status %d", (int)_status);

		if (_status != bm_type::success)
		{
			_callback(connection_status::fail, nullptr, 0);
			return;
		}

		std::vector<std::string>	ids;
		for (auto& prd : gBilling->products)
		{
			ids.push_back(prd.productId);
		}

		gBilling->bm.loadSkuDetails([=](billing::sku_detail const* _details, size_t _detailsCount)
		{
			for (auto& sku : amtrs::make_bucket(_details, _detailsCount))
			{
				AMTRS_DEBUG_LOG("identify: %s, price: %s, rewarded: %s", sku.identify.c_str(), sku.price.c_str(), sku.rewarded ? "YES" : "NO");
				gBilling->skus.push_back(sku);
			}
			gBilling->modified		= true;
			gBilling->connecting	= false;

			auto	prods	= get_products();
			_callback(connection_status::ok, prods.first, get_products().second);
		}, ids);
	});
}


// ==================================================================
//! 課金処理システムを終了します。
// ------------------------------------------------------------------
void stop()
{
	gBilling	= nullptr;
}


// ==================================================================
//! 通信中かどうか調べます。
// ------------------------------------------------------------------
bool connecting()
{
	return	gBilling.empty() ? false : gBilling->connecting;
}



// ==================================================================
//! 購入可能な商品の一覧を返します。
// ------------------------------------------------------------------
std::pair<purchase_product const*, size_t> get_products()
{
	if (gBilling->modified)
	{
		// skus と スクリプトの両方にあるものだけをリストアップする
		gBilling->modified		= false;
		gBilling->productsCache	= {};

		for (auto& sku : gBilling->skus)
		{
			for (auto& prd : gBilling->products)
			{
				if (prd.productId == sku.identify)
				{
					prd.price		= sku.price;
					prd.rewarded	= sku.rewarded;
					gBilling->productsCache.push_back(prd);
					break;
				}
			}
		}
	}
	return	{gBilling->productsCache.data(), gBilling->productsCache.size()};
}


// ==================================================================
//! 指定するプロダクトIDもしくは識別子のアイテムを返します。
// ------------------------------------------------------------------
purchase_product const* find_product(std::string const& _id)
{
	auto	it	= std::find_if(gBilling->productsCache.begin(), gBilling->productsCache.end(), [&](auto prod)
				{
					return	prod.productId == _id || prod.identify == _id;
				});
	return	it == gBilling->productsCache.end() ? nullptr : &(*it);
}



// ==================================================================
//! 購入処理を行います。
// ------------------------------------------------------------------
void start_purchase(std::string const& _id, std::function<void(purchase_status)> _callback)
{
	AMTRS_DEBUG_LOG("start_purchase");
	auto	prod	= find_product(_id);
	if (!prod)
	{
		AMTRS_DEBUG_LOG("Nothing product");
		_callback(purchase_status::unproduct);
		return;
	}

	AMTRS_DEBUG_LOG("purchase");
	gBilling->bm.purchase([=](auto _apistate, auto _top, auto _size)
	{
		AMTRS_DEBUG_LOG("purchase ok");
		purchase_status	msg	= purchase_status::unproduct;

		// 購入処理に関するエラーがあった場合の処理
		if (_apistate != bm_type::status::success)
		{
			msg	= _apistate == bm_type::status::network_error	? purchase_status::failnetwork
				: _apistate == bm_type::status::unsupported		? purchase_status::unproduct
				: _apistate == bm_type::status::limited			? purchase_status::unproduct
				:												  purchase_status::unproduct;
		}
		else
		{
			// 購入結果に対する処理
			for (decltype(_size) i = 0; i < _size; ++i)
			{
				if (_top[i].identify != prod->productId)
				{
					continue;
				}
				msg	= _top[i].state == bm_type::purchase_status::purchased		? purchase_status::purchased
					: _top[i].state == bm_type::purchase_status::pending		? purchase_status::pending
					: _top[i].state == bm_type::purchase_status::unspecified	? purchase_status::unspecified
					: _top[i].state == bm_type::purchase_status::cancel			? purchase_status::cancel
					: _top[i].state == bm_type::purchase_status::already		? purchase_status::already
					:															  purchase_status::unproduct;
			}
		}
		_callback(msg);
		return;

	}, prod->productId);
}

// ==================================================================
//! 購入済みの復元を行います。
// ------------------------------------------------------------------
void restore_purchase(std::function<void(std::string const*, size_t)> _callback)
{
	AMTRS_DEBUG_LOG("start connect");
	gBilling->bm.connect([_callback](auto _status)
	{
		AMTRS_DEBUG_LOG("connect billing");
		std::vector<std::string_view>	sku;
		for (auto& prod : gBilling->productsCache)
		{
			sku.push_back(prod.productId);
		}
		gBilling->bm.loadSkuDetails([_callback](billing::sku_detail const* _details, size_t _detailsCount)
		{
			bool						unlocked	= false;
			std::vector<std::string>	unlockList;
			for (auto& sku : make_bucket(_details, _detailsCount))
			{
				AMTRS_DEBUG_LOG("identify: %s, price: %s, rewarded: %s", sku.identify.c_str(), sku.price.c_str(), sku.rewarded ? "YES" : "NO");
				if (sku.rewarded)
				{
					auto	i	= const_cast<purchase_product*>(find_product(sku.identify));
					i->rewarded	= true;
					unlocked	= true;
					unlockList.push_back(i->productId);
				}
			}
			_callback(unlockList.data(), unlockList.size());
		}, sku);
	});
}



void load_product_script(billingdata* _dest, std::string const& _script_filename)
{
	_dest->products	= {};

	std::string	script		= amtrs::filesystem::file_get_contents<std::string>(_script_filename);
	int			linenumber	= 0;
	int			mode		= 0;
	for (auto l : split_chars(std::string_view(script), "\n"))
	{
		++linenumber;

		// 改行コードを取り除き、空行ならスキップする。
		std::string_view	line	= trim(l, "\r\n");
		if (line == "")
		{
			mode				= 0;
			continue;
		}
		// tabで始まらない場合は新しいストア情報
		if (line[0] != '\t')
		{
			_dest->products.push_back({});
			_dest->products.back().title	= amtrs::trim(line, " \t");
			mode				= 0;
			continue;
		}

		if (mode == 1)
		{
			if (!starts_with(line, std::string_view("product-")))
			{
				_dest->products.back().comment	+= "\n";
				_dest->products.back().comment	+= trim(line, " \t");
			}
			continue;
		}
		mode				= 0;

		// それ以外は先頭のコマンドを読み取る
		line	= trim(line, " \t");
		auto	pos	= line.find_first_of(':');
		if (pos == decltype(line)::npos)
		{
			AMTRS_DEBUG_LOG("Unkown command line %d.", linenumber);
			continue;
		}
		AMTRS_DEBUG_LOG("line: %s", std::string(line).c_str());

		auto	cmd	= line.substr(0, pos);
		auto	val	= trim(line.substr(pos + 1), " \t");
		if (cmd == "identify")
		{
			_dest->products.back().identify		= val;
			continue;
		}
		if (cmd == "icon")
		{
			_dest->products.back().icon			= val;
			continue;
		}
		if (cmd == "comment")
		{
			_dest->products.back().comment		= val;
			mode								= 1;
			continue;
		}
		if (starts_with(cmd, std::string_view("product-")))
		{
		#define	___NAME(_a, _b)	_a #_b
		#define	__NAME(_a, _b)	___NAME(_a, _b)
		#define	_NAME(_a, _b)	__NAME(_a, _b)
			if (cmd == _NAME("product-", AMTRS_CURRENT_PLATFORM_NAME))
			{
				_dest->products.back().productId		= val;
			}
		#undef	_NAME
		#undef	__NAME
		#undef	___NAME
			continue;
		}
		AMTRS_DEBUG_LOG("Unkown line %s", std::string(line).c_str());
	}
	_dest->modified		= true;
}




} // namespace purchase

/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__billing__billing_manager_ios__hpp
#define	__libamtrs__billing__billing_manager_ios__hpp
#import <StoreKit/StoreKit.h>
#include <map>
#include <vector>
#include <amtrs/logging/@>
#include "applekit/objcptr.hpp"

@interface	libamtrs_BillingController
			: UIViewController<SKProductsRequestDelegate, SKPaymentTransactionObserver>
{
	@public	void*	manager;
}

- (void)request:(SKRequest *)request didFailWithError:(NSError *)error;
- (void)paymentQueue:(SKPaymentQueue *)queue updatedTransactions:(NSArray *)transactions;

// 購入処理のコールバック
- (void)productsRequest:(SKProductsRequest *)request didReceiveResponse:(SKProductsResponse *)response;
@end


AMTRS_BILLING_NAMESPACE_BEGIN
class	billing_manager_ios
		: public billing_manager
{
public:

	class	interface
			: public billing_manager::interface_base
	{
	public:
		interface()
		{
			*get_instance()	= this;
			mController				= [[libamtrs_BillingController alloc] init];
			mController->manager	= this;
			[[SKPaymentQueue defaultQueue] addTransactionObserver:mController];
		}

		virtual ~interface()
		{
			AMTRS_DEBUG_LOG("~interface() delete");
			*get_instance()	= nullptr;
		}

		bool connected() const noexcept
		{
			return	mConnected;
		}

		void connect(std::function<void(status _status)> _callback)
		{
			AMTRS_DEBUG_LOG("billing_manager_ios::connect");
			mConnected			= false;
			mProducts.clear();
			mConnectCallback	= std::move(_callback);
			_restore();
		}

		void purchase(std::function<void(status _status, purchase_result const* _details, size_t _detailsCount)> _callback, std::string_view _sku)
		{
			mPurchaseCallback	= std::move(_callback);

			std::string			s				= std::string(_sku);
#if	0
			NSString*			pid				= [NSString
												stringWithCString:	s.c_str()
												encoding:			NSUTF8StringEncoding];
#endif
			// コールバックを登録する

			// 購入制限がかかっていれば何もせずに返す
			if ([SKPaymentQueue canMakePayments] == NO)
			{
				mPurchaseCallback(status::limited, nullptr, 0);
				return;
			}

			auto				prod			= mProducts.find(s);
			if (prod == mProducts.end())
			{
				AMTRS_DEBUG_LOG("Nothing product %s", s.c_str());
				mPurchaseCallback(status::unsupported, nullptr, 0);
			}
			else
			{
				AMTRS_DEBUG_LOG("SKPaymentQueue product %s", s.c_str());
				SKPayment*		payment = [SKPayment paymentWithProduct:prod->second];
				[[SKPaymentQueue defaultQueue] addPayment:payment];
			}
		}


		void loadSkuDetails(std::function<void(sku_detail const* _details, size_t _detailsCount)> _callback, implicit_virtual_iterator<std::string_view>& _first, implicit_virtual_iterator<std::string_view>& _last)
		{
			mDetailsCallback	= std::move(_callback);

			// まずはプロダクトＩＤの一覧を生成する
			NSMutableSet*		productIds	= [[NSMutableSet alloc] init];
			for (;_first != _last; ++_first)
			{
				auto				s				= *_first;
				NSString*			pid				= [[NSString alloc]
													initWithBytes:		(const void**)s.data()
													length:				(NSUInteger)s.size()
													encoding:			NSUTF8StringEncoding];
				[productIds addObject:pid];
			}

			// プロダクトの情報をい投げる
			SKProductsRequest*	productsRequest		= [[SKProductsRequest alloc] initWithProductIdentifiers:productIds];
			productsRequest.delegate	= mController;
			[productsRequest start];
		//	[productsRequest release];
		}


		void disconnect()
		{
			mConnected	= false;
		}


		void _restore()
		{
			AMTRS_DEBUG_LOG("billing_manager_ios::_restore");
			mRestoreProducts	= {};
			[[SKPaymentQueue defaultQueue] restoreCompletedTransactions];
		}

		void _restore_finished(status _status)
		{
			if (_status == status::success)
			{
				mConnected	= true;
			}
			mConnectCallback(_status);
		}


		std::vector<billing::sku_detail>															mSkus;
		std::vector<std::string>																	mRestoreProducts;			// restore() で取得したIDの一覧
		std::function<void(status _status)>															mConnectCallback;
		std::function<void(sku_detail const* _details, size_t _detailsCount)>						mDetailsCallback;
		std::function<void(status _status, purchase_result const* _details, size_t _detailsCount)>	mPurchaseCallback;
		bool																						mConnected	= false;
		libamtrs_BillingController*																	mController;
		std::map<std::string, objcptr<SKProduct>>													mProducts;
	};


	static interface** get_instance()
	{
		static interface*	sInterface	= nullptr;
		return	&sInterface;
	}


	friend	billing_manager;
};


// ========================================================================
//! 初期化を行う。
// ------------------------------------------------------------------------
inline extern billing_manager::billing_manager()
{
	if (!*billing_manager_ios::get_instance())
	{
		auto	thiz	= new billing_manager_ios::interface();
		mInterface	= thiz;
	}
	else
	{
		mInterface	= *billing_manager_ios::get_instance();
	}
}


// ========================================================================
//! 課金システムに接続済みか調べる。
// ------------------------------------------------------------------------
inline extern bool billing_manager::connected() const noexcept
{
	return	static_cast<billing_manager_ios::interface*>(mInterface.get())->connected();
}


// ========================================================================
//! 課金システムへ接続する。
// ------------------------------------------------------------------------
//! 接続が終わるとコールバックを呼び出す。
// ------------------------------------------------------------------------
inline extern void billing_manager::connect(std::function<void(status _status)> _callback)
{
	static_cast<billing_manager_ios::interface*>(mInterface.get())->connect(std::move(_callback));
}


inline extern void billing_manager::purchase(std::function<void(status _status, purchase_result const* _details, size_t _detailsCount)> _callback, std::string_view _sku)
{
	static_cast<billing_manager_ios::interface*>(mInterface.get())->purchase(std::move(_callback), _sku);
}


inline extern void billing_manager::loadSkuDetails(std::function<void(sku_detail const* _details, size_t _detailsCount)> _callback, implicit_virtual_iterator<std::string_view>& _first, implicit_virtual_iterator<std::string_view>& _last)
{
	static_cast<billing_manager_ios::interface*>(mInterface.get())->loadSkuDetails(std::move(_callback), _first, _last);
}


// ========================================================================
//! 課金システムとの接続を解除する。
// ------------------------------------------------------------------------
inline extern void billing_manager::disconnect()
{
	static_cast<billing_manager_ios::interface*>(mInterface.get())->disconnect();
}

AMTRS_BILLING_NAMESPACE_END
#endif

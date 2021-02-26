/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/logging/@>
#include <amtrs/billing/billing_manager.hpp>
#include <amtrs/.driver/ios/applekit/string.hpp>
using namespace AMTRS_NAMESPACE;




@implementation libamtrs_BillingController

- (void)
	productsRequest:	(SKProductsRequest *)request
	didReceiveResponse:	(SKProductsResponse *)response
{
	AMTRS_DEBUG_LOG("libamtrs_BillingController::productsRequest:didReceiveResponse");
	auto	thiz	= ((billing::billing_manager_ios::interface*)manager);

	NSArray<SKProduct*>*				skus	= response.products;
	std::vector<billing::sku_detail>	res;
	for (size_t i = 0; i < skus.count; ++i)
	{
		SKProduct*			sku			= [skus objectAtIndex:i];
		NSNumberFormatter*	numberFmt	= [[NSNumberFormatter alloc] init];
		[numberFmt setFormatterBehavior:NSNumberFormatterBehavior10_4];
        [numberFmt setNumberStyle:NSNumberFormatterCurrencyStyle];
        [numberFmt setLocale:sku.priceLocale];

		billing::sku_detail	t;
		t.identify	= std::to_string(sku.productIdentifier);
		t.price		= std::to_string([numberFmt stringFromNumber:sku.price]);
		if (std::find_if(thiz->mRestoreProducts.begin(), thiz->mRestoreProducts.end(), [&](auto& p) { return t.identify == p; }) != thiz->mRestoreProducts.end())
		{
			t.rewarded	= true;
		}
		else
		{
			t.rewarded	= false;
		}
		res.push_back(std::move(t));

		((billing::billing_manager_ios::interface*)manager)->mProducts.insert({std::to_string(sku.productIdentifier), objcptr<SKProduct>(sku)});

		AMTRS_DEBUG_LOG("%s, %s", t.identify.c_str(), t.price.c_str());
	}
#ifdef  AMTRS_DEBUG_LOG_USE
	for (auto& prod : ((billing::billing_manager_ios::interface*)manager)->mProducts)
	{
		AMTRS_DEBUG_LOG("[%s]", prod.first.c_str());
	}
#endif
    
	((billing::billing_manager_ios::interface*)manager)->mDetailsCallback(res.data(), res.size());
}

- (void)
	request:			(SKRequest *) _request
	didFailWithError:	(NSError *) _error
{
	AMTRS_DEBUG_LOG("libamtrs_BillingController::request:didFailWithError");
}

- (void)
	paymentQueue:			(SKPaymentQueue *) _queue
	updatedTransactions:	(NSArray<SKPaymentTransaction*>*) _transactions
{
	AMTRS_DEBUG_LOG("libamtrs_BillingController::paymentQueue:updatedTransactions");
	auto	thiz	= ((billing::billing_manager_ios::interface*)manager);

	std::vector<billing::billing_manager::purchase_result>	purchases;
	for (SKPaymentTransaction* t in _transactions)
	{
		std::string									dmsg;
		std::string									emsg;
		NSInteger									errorCode			= t.error ? t.error.code : 0;
		std::string									productIdentifier	= t.payment ? std::to_string(t.payment.productIdentifier) : std::string();
		std::string									transaction			= t.transactionIdentifier ? std::to_string(t.transactionIdentifier) : std::string();
		billing::billing_manager::purchase_status	state				= billing::billing_manager::purchase_status::unkoen_error;

		if (t.transactionState == SKPaymentTransactionState::SKPaymentTransactionStatePurchasing)
		{
			// 課金処理（ＯＳのダイアログの表示）を開始しただけなのでこのメッセージは無視する。
			AMTRS_DEBUG_LOG("%s state=SKPaymentTransactionState::SKPaymentTransactionStatePurchasing, error.code=%s", transaction.c_str(), emsg.c_str());
			continue;
		}



		switch (t.transactionState)
		{
			// 課金処理を継続中
			case SKPaymentTransactionState::SKPaymentTransactionStatePurchasing :
			{
				// pending
				dmsg	= "SKPaymentTransactionState::SKPaymentTransactionStatePurchasing";
				break;
			}

			// 課金完了
			case SKPaymentTransactionState::SKPaymentTransactionStatePurchased :
			{
				dmsg	= "SKPaymentTransactionState::SKPaymentTransactionStatePurchased";
				state	= billing::billing_manager::purchase_status::purchased;
				[[SKPaymentQueue defaultQueue] finishTransaction:t];
				break;
			}

			// 課金失敗
			case SKPaymentTransactionState::SKPaymentTransactionStateFailed :
			{
				// キャンセルになっている場合はトランザクションを閉じる
				[[SKPaymentQueue defaultQueue] finishTransaction:t];
				dmsg	= "SKPaymentTransactionState::SKPaymentTransactionStateFailed";
				[[SKPaymentQueue defaultQueue] finishTransaction:t];
				break;
			}

			// リストア
			case SKPaymentTransactionState::SKPaymentTransactionStateRestored :
			{
				// unspecified
				dmsg	= "SKPaymentTransactionState::SKPaymentTransactionStateRestored";
				state	= billing::billing_manager::purchase_status::purchased;
				[[SKPaymentQueue defaultQueue] finishTransaction:t];
				break;
			}

			// 承認待ち
			case SKPaymentTransactionState::SKPaymentTransactionStateDeferred :
			{
				// pending
				dmsg	= "SKPaymentTransactionState::SKPaymentTransactionStateDeferred";
				state	= billing::billing_manager::purchase_status::pending;
				break;
			}

			// 不明
			default:
			{
				// pending
				dmsg	= "SKPaymentTransactionState::  Unkown";
				[[SKPaymentQueue defaultQueue] finishTransaction:t];
				break;
			}
		}

		switch (errorCode)
		{
			case SKErrorUnknown :
				emsg	= "SKErrorUnknown";
				break;

			case SKErrorClientInvalid :
				emsg	= "SKErrorClientInvalid";
				break;

			case SKErrorPaymentCancelled :
				emsg	= "SKErrorPaymentCancelled";
				state	= billing::billing_manager::purchase_status::cancel;
				break;

			case SKErrorPaymentInvalid :
				emsg	= "SKErrorPaymentInvalid";
				break;

			case SKErrorPaymentNotAllowed :
				emsg	= "SKErrorPaymentNotAllowed";
				break;

			case SKErrorStoreProductNotAvailable :
				emsg	= "SKErrorStoreProductNotAvailable";
				break;

			case SKErrorCloudServicePermissionDenied :
				emsg	= "SKErrorCloudServicePermissionDenied";
				break;

			case SKErrorCloudServiceNetworkConnectionFailed :
				emsg	= "SKErrorCloudServiceNetworkConnectionFailed";
				break;

			case SKErrorCloudServiceRevoked :
				emsg	= "SKErrorCloudServiceRevoked";
				break;

			case SKErrorPrivacyAcknowledgementRequired :
				emsg	= "SKErrorPrivacyAcknowledgementRequired";
				break;

			case SKErrorUnauthorizedRequestData :
				emsg	= "SKErrorUnauthorizedRequestData";
				break;

			case SKErrorInvalidOfferIdentifier :
				emsg	= "SKErrorInvalidOfferIdentifier";
				break;

			case SKErrorInvalidOfferPrice :
				emsg	= "SKErrorInvalidOfferPrice";
				break;

			case SKErrorInvalidSignature :
				emsg	= "SKErrorInvalidSignature";
				break;

			case SKErrorMissingOfferParams :
				emsg	= "SKErrorMissingOfferParams";
				break;

			default:
				emsg	= "UNKOWN";
				break;
		}

		AMTRS_DEBUG_LOG("%s state=%s, error.code=%s", transaction.c_str(), dmsg.c_str(), emsg.c_str());
		billing::billing_manager::purchase_result	p;
		p.state		= state;
		p.identify	= std::move(productIdentifier);
		p.receipt	= std::move(transaction);
		if (t.transactionState == SKPaymentTransactionStateRestored)
		{
			thiz->mRestoreProducts.push_back(std::to_string(t.payment.productIdentifier));
		}
		purchases.push_back(std::move(p));
	}
	if (thiz->mPurchaseCallback && purchases.size() > 0)
	{
		thiz->mPurchaseCallback(billing::billing_manager::status::success, purchases.data(), purchases.size());
	}
}


//! リストア失敗
- (void)
	paymentQueue:										(SKPaymentQueue *)queue
	restoreCompletedTransactionsFailedWithError:		(NSError *)error
{
	auto	thiz	= ((billing::billing_manager_ios::interface*)manager);
	thiz->_restore_finished(billing::billing_manager_ios::status::unkown_error);
}

//! リストア処理が終了
- (void)
	paymentQueueRestoreCompletedTransactionsFinished:	(SKPaymentQueue *)queue 
{
	auto	thiz	= ((billing::billing_manager_ios::interface*)manager);
	thiz->_restore_finished(billing::billing_manager_ios::status::success);
}


@end

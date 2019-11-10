#include <amtrs/billing/billing_manager.hpp>
#include <amtrs/os/ios/string.hpp>
#include <amtrs/logging/@>
using namespace AMTRS_NAMESPACE;




@implementation libamtrs_BillingController

- (void)
	productsRequest:	(SKProductsRequest *)request
	didReceiveResponse:	(SKProductsResponse *)response
{
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
		res.push_back(std::move(t));

		((billing::billing_manager_ios::interface*)manager)->mProducts.insert({std::to_string(sku.productIdentifier), sku});

		AMTRS_TRACE_LOG("%s, %s", t.identify.c_str(), t.price.c_str());
	}

	for (auto& prod : ((billing::billing_manager_ios::interface*)manager)->mProducts)
	{
		AMTRS_TRACE_LOG("[%s]", prod.first.c_str());
	}
    
	((billing::billing_manager_ios::interface*)manager)->mDetailsCallback(res.data(), res.size());
}

- (void)
	request:			(SKRequest *) _request
	didFailWithError:	(NSError *) _error
{
	AMTRS_TRACE_LOG("libamtrs_BillingController::request:didFailWithError");
}

- (void)
	paymentQueue:			(SKPaymentQueue *) _queue
	updatedTransactions:	(NSArray<SKPaymentTransaction*>*) _transactions
{
	AMTRS_TRACE_LOG("libamtrs_BillingController::paymentQueue:updatedTransactions");

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
			AMTRS_TRACE_LOG("%s state=SKPaymentTransactionState::SKPaymentTransactionStatePurchasing, error.code=%s", transaction.c_str(), emsg.c_str());
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

		AMTRS_TRACE_LOG("%s state=%s, error.code=%s", transaction.c_str(), dmsg.c_str(), emsg.c_str());
		billing::billing_manager::purchase_result	p;
		p.state		= state;
		p.identify	= std::move(productIdentifier);
		p.receipt	= std::move(transaction);
		purchases.push_back(std::move(p));
	}
	if (purchases.size() > 0)
	{
		((billing::billing_manager_ios::interface*)manager)->mPurchaseCallback(billing::billing_manager::status::success, purchases.data(), purchases.size());
	}
}


@end

package jp.libamtrs.Billing;

import	java.util.ArrayList;
import	java.util.List;

import	com.android.billingclient.api.*;
import	com.android.billingclient.api.BillingClient;
import	com.android.billingclient.api.BillingClient.BillingResponseCode;
import	com.android.billingclient.api.BillingClient.SkuType;
import	com.android.billingclient.api.Purchase.PurchaseState;

public class	Billing
				implements com.android.billingclient.api.PurchasesUpdatedListener
{
	native private void onStartFinished(long _nativeHandle, int _responseCode); 
	native private void onSkuDetails(long _nativeHandle, List<SkuDetails> _details); 
	native private void onPurchasesUpdate(long _nativeHandle, int _responseCode, List<Purchase> _purchase); 


	public Billing(long _native, android.app.Activity _activity)
	{
		mNative			= _native;
		mActivity		= _activity;
		billingClient	= BillingClient.newBuilder(mActivity)
						.setListener(this)
						.enablePendingPurchases()
						.build();
	}


	public void startConnection()
	{
		if (!mConnecting)
		{
			billingClient.startConnection(new BillingClientStateListener()
			{
				@Override
				public void onBillingSetupFinished(BillingResult billingResult)
				{
					mConnecting	= billingResult.getResponseCode() == BillingResponseCode.OK;
					onStartFinished(mNative, billingResult.getResponseCode());
				}

				@Override
				public void onBillingServiceDisconnected()
				{
					android.util.Log.d("lamtrs", "onBillingServiceDisconnected");
					mConnecting	= false;
				}
			});
		}
	}


	public void loadSkuDetails(List<String> _itemIds)
	{
		SkuDetailsParams.Builder	params	= SkuDetailsParams.newBuilder();
		params.setSkusList(_itemIds).setType(SkuType.INAPP);
		billingClient.querySkuDetailsAsync(params.build(), new SkuDetailsResponseListener()
		{
			@Override
			public void onSkuDetailsResponse(BillingResult billingResult, List<SkuDetails> skuDetailsList)
			{
				mSkuDetailsList	= skuDetailsList;
				if (billingResult.getResponseCode() == BillingResponseCode.OK && skuDetailsList != null)
				{
					onSkuDetails(mNative, skuDetailsList);
				}
				else
				{
					android.util.Log.d("lamtrs", "onBillingSetupFinished is failure " + billingResult.getResponseCode());
				}
			}
		});
	}


	public boolean startPurchase(String _sku)
	{
		for (SkuDetails s : mSkuDetailsList)
		{
			if (_sku.equals(s.getSku().toString()))
			{
				BillingFlowParams	params	= BillingFlowParams.newBuilder()
											.setSkuDetails(s)
											.build();
				billingClient.launchBillingFlow(mActivity, params);
				return	true;
			}
		}
		return	false;
	}


	// 購入結果が更新された
	@Override
	public void onPurchasesUpdated(BillingResult billingResult, List<Purchase> purchases)
	{
		onPurchasesUpdate(mNative, billingResult.getResponseCode(), purchases);
	}

	private	boolean						mConnecting	= false;
	private BillingClient				billingClient;
	private	android.app.Activity		mActivity;
	private	long						mNative;
	private List<SkuDetails>			mSkuDetailsList;
}

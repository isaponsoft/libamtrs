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
	native private void onSkuDetails(long _nativeHandle, List<Info> _details); 
	native private void onPurchasesUpdate(long _nativeHandle, int _responseCode, List<Purchase> _purchase); 


	static public class	Info
	{
		SkuDetails		details;
		Purchase		purcase;
	}


	public Billing(long _native, android.app.Activity _activity)
	{
		mNative			= _native;
		mActivity		= _activity;
		mBillingClient	= BillingClient.newBuilder(mActivity)
						.setListener(this)
						.enablePendingPurchases()
						.build();
	}


	public void startConnection()
	{
		if (!mConnecting)
		{
			mBillingClient	= BillingClient.newBuilder(mActivity)
							.setListener(this)
							.enablePendingPurchases()
							.build();
			mBillingClient.startConnection(new BillingClientStateListener()
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


	public void endConnection()
	{
		if (mBillingClient != null)
		{
			mBillingClient.endConnection();
			mBillingClient	= null;
			mConnecting		= false;
		}
	}


	public void loadSkuDetails(List<String> _itemIds)
	{
		SkuDetailsParams.Builder	params	= SkuDetailsParams.newBuilder();
		params.setSkusList(_itemIds).setType(SkuType.INAPP);
		mBillingClient.querySkuDetailsAsync(params.build(), new SkuDetailsResponseListener()
		{
			@Override
			public void onSkuDetailsResponse(BillingResult billingResult, List<SkuDetails> skuDetailsList)
			{
				mSkuDetailsList	= new ArrayList<Info>();
				for (SkuDetails s : skuDetailsList)
				{
					Info	inf	= new Info();
					inf.details	= s;
					mSkuDetailsList.add(inf);
				}

				mBillingClient.queryPurchaseHistoryAsync


				Purchase.PurchasesResult	purchasesResult	= mBillingClient.queryPurchases(BillingClient.SkuType.INAPP);
				int							responseCode	= purchasesResult.getResponseCode();
				if (responseCode== BillingClient.BillingResponseCode.OK)
				{
					List<Purchase>	purchases	= purchasesResult.getPurchasesList();
					if (!purchases.isEmpty())
					{
						for (Purchase purchase : purchases)
						{
							String	sku	= purchase.getSku();
							for (Info inf : mSkuDetailsList)
							{
								if (inf.details.getSku() == sku)
								{
									inf.purcase	= purchase;
									break;
								}
							}
						}
					}
				}
				if (billingResult.getResponseCode() == BillingResponseCode.OK && skuDetailsList != null)
				{
					onSkuDetails(mNative, mSkuDetailsList);
				}
				else
				{
					android.util.Log.d("lamtrs", "onBillingSetupFinished is failure " + billingResult.getResponseCode());
				}
			}
		});
	}

/*
	public void loadSkuDetails(List<String> _itemIds)
	{
		SkuDetailsParams.Builder	params	= SkuDetailsParams.newBuilder();
		params.setSkusList(_itemIds).setType(SkuType.INAPP);
		mBillingClient.querySkuDetailsAsync(params.build(), new SkuDetailsResponseListener()
		{
			@Override
			public void onSkuDetailsResponse(BillingResult billingResult, List<SkuDetails> skuDetailsList)
			{
		android.util.Log.d("lamtrs", "onSkuDetailsResponse **********************");

				mSkuDetailsList	= new ArrayList<Info>();
				for (SkuDetails s : skuDetailsList)
				{
		android.util.Log.d("lamtrs", "SkuDetails  >> " + s.toString());

					Info	inf	= new Info();
					inf.details	= s;
					mSkuDetailsList.add(inf);
				}

				// 購入済み情報
				for (Purchase purchase : mBillingClient.queryPurchases(BillingClient.SkuType.INAPP).getPurchasesList())
				{
		android.util.Log.d("lamtrs", "Purchase  >> " + purchase.toString());
					for (Info inf : mSkuDetailsList)
					{
						if (inf.details.getSku() == purchase.getSku())
						{
							inf.purcase	= purchase;
							break;
						}
					}
				}

				// コールバックを呼び出す
				onSkuDetails(mNative, mSkuDetailsList);
			}
		});
	}


/*
	public void loadSkuDetails(List<String> _itemIds)
	{
		SkuDetailsParams.Builder	params	= SkuDetailsParams.newBuilder();
		params.setSkusList(_itemIds).setType(SkuType.INAPP);
		mBillingClient.querySkuDetailsAsync(params.build(), new SkuDetailsResponseListener()
		{
			@Override
			public void onSkuDetailsResponse(BillingResult billingResult, List<SkuDetails> skuDetailsList)
			{
				mSkuDetailsList	= new ArrayList<Info>();
				for (SkuDetails s : skuDetailsList)
				{
					Info	inf	= new Info();
					inf.details	= s;
					mSkuDetailsList.add(inf);
				}


				mBillingClient.queryPurchaseHistoryAsync(BillingClient.SkuType.INAPP, new PurchaseHistoryResponseListener() {
					@Override
					public void onPurchaseHistoryResponse(BillingResult billingResult, List<PurchaseHistoryRecord> purchasesList)
					{
android.util.Log.d("lamtrs", "onPurchaseHistoryResponse");
						Purchase.PurchasesResult	purchasesResult	= mBillingClient.queryPurchases(BillingClient.SkuType.INAPP);
						int							responseCode	= purchasesResult.getResponseCode();
						if (responseCode== BillingClient.BillingResponseCode.OK)
						{
android.util.Log.d("lamtrs", "BillingClient.BillingResponseCode.OK");
							List<Purchase>	purchases	= purchasesResult.getPurchasesList();
android.util.Log.d("lamtrs", "BillingClient.BillingResponseCode.OK" + purchases.size());
							if (!purchases.isEmpty())
							{
								for (Purchase purchase : purchases)
								{
									String	sku	= purchase.getSku();
									for (Info inf : mSkuDetailsList)
									{
										if (inf.details.getSku() == sku)
										{
											inf.purcase	= purchase;
											break;
										}
									}
								}
							}
						}
						onSkuDetails(mNative, mSkuDetailsList);
					}
				});
			}
		});
	}
*/


/*
	public void loadSkuDetails(List<String> _itemIds)
	{
		SkuDetailsParams.Builder	params	= SkuDetailsParams.newBuilder();
		params.setSkusList(_itemIds).setType(SkuType.INAPP);
		mBillingClient.querySkuDetailsAsync(params.build(), new SkuDetailsResponseListener()
		{
			@Override
			public void onSkuDetailsResponse(BillingResult billingResult, List<SkuDetails> skuDetailsList)
			{
				mSkuDetailsList	= new ArrayList<Info>();
				for (SkuDetails s : skuDetailsList)
				{
					Info	inf	= new Info();
					inf.details	= s;
					mSkuDetailsList.add(inf);
				}

				Purchase.PurchasesResult	purchasesResult	= mBillingClient.queryPurchases(BillingClient.SkuType.INAPP);
				int							responseCode	= purchasesResult.getResponseCode();
				if (responseCode== BillingClient.BillingResponseCode.OK)
				{
					List<Purchase>	purchases	= purchasesResult.getPurchasesList();
					if (!purchases.isEmpty())
					{
						for (Purchase purchase : purchases)
						{
							String	sku	= purchase.getSku();
							for (Info inf : mSkuDetailsList)
							{
								if (inf.details.getSku() == sku)
								{
									inf.purcase	= purchase;
									break;
								}
							}
						}
					}
				}
				if (billingResult.getResponseCode() == BillingResponseCode.OK && skuDetailsList != null)
				{
					onSkuDetails(mNative, mSkuDetailsList);
				}
				else
				{
					android.util.Log.d("lamtrs", "onBillingSetupFinished is failure " + billingResult.getResponseCode());
				}
			}
		});
	}
*/

	public boolean startPurchase(String _sku)
	{
		for (Info s : mSkuDetailsList)
		{
			if (_sku.equals(s.details.getSku().toString()))
			{
				BillingFlowParams	params	= BillingFlowParams.newBuilder()
											.setSkuDetails(s.details)
											.build();
				mBillingClient.launchBillingFlow(mActivity, params);
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
	private BillingClient				mBillingClient;
	private	android.app.Activity		mActivity;
	private	long						mNative;
	private List<Info>					mSkuDetailsList;
}

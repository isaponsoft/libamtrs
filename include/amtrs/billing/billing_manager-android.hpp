#ifndef	__libamtrs__billing__billing_manager_android__hpp
#define	__libamtrs__billing__billing_manager_android__hpp
#include <amtrs/java/jp/libamtrs/Billing.Billing.hpp>
#include <amtrs/java/jp/libamtrs/Billing.Billing.hpp>
#include <amtrs/java/com/android/billingclient/api/_.hpp>
#include <amtrs/java/java/util/ArrayList.hpp>
#include <amtrs/os/android/amtrs_activity.hpp>
#include "def.hpp"
AMTRS_BILLING_NAMESPACE_BEGIN
class	billing_manager_android
		: public billing_manager
{
private:
	using	Purchase					= java::classes::com::android::billingclient::api::Purchase;
	using	PurchasesUpdatedListener	= java::classes::com::android::billingclient::api::PurchasesUpdatedListener;
	using	SkuDetails					= java::classes::com::android::billingclient::api::SkuDetails;
	using	Billing						= java::classes::jp::libamtrs::Billing::Billing;
	using	ArrayList					= java::classes::java::util::ArrayList;
	using	List						= java::classes::java::util::List;
	using	String						= java::classes::java::lang::String;


	class	interface
			: public billing_manager::interface_base
	{
	public:
		interface()
		{
			*get_instance()	= this;
		}

		virtual ~interface()
		{
			*get_instance()	= nullptr;
		}

		bool connected() const noexcept
		{
			return	mConnected;
		}

		void connect(std::function<void(status _status)> _callback)
		{
			mConnected			= false;
			mConnectCallback	= std::move(_callback);
			java::jobj<Billing>(mBilling).startConnection();
		}

		void purchase(std::function<void(status _status, purchase_result const* _details, size_t _detailsCount)> _callback, std::string_view _sku)
		{
			mPurchaseCallback	= std::move(_callback);
			mLastPurchaseSku	= _sku;

			java::jobj<Billing>(mBilling).startPurchase(java::jobj<String>(java::to_jstring(_sku)));
		}


		void loadSkuDetails(std::function<void(sku_detail const* _details, size_t _detailsCount)> _callback, implicit_virtual_iterator<std::string_view>& _first, implicit_virtual_iterator<std::string_view>& _last)
		{
			auto	objList		= ArrayList::find().init();
			mDetailsCallback	= std::move(_callback);
			for (; _first != _last; ++_first)
			{
				auto	n	= java::to_jstring(*_first);
				objList.add(n.get());
			}
			java::jobj<Billing>(mBilling).loadSkuDetails(objList);
		}


		void disconnect()
		{
			mConnected	= false;
		}

	
		void onPurchasesUpdate(int _responseCode, java::jobj<List> _purchase)
		{
			if (mPurchaseCallback)
			{
				std::vector<purchase_result>	details;
				auto							rs	= status::success;
				if (!_purchase.empty())
				{
					int		s	= _purchase.size();
					for (int i = 0; i < s; ++i)
					{
						purchase_result	r;
						auto			p	= java::jobj<Purchase>(_purchase.get(i));
						r.state		= purchase_status::unkoen_error;
						switch (_responseCode)
						{
							case 0 :
							{
								switch (p.getPurchaseState())
								{
									case 0 :	r.state	= purchase_status::unspecified;		break;		// Purchase.PurchaseState.UNSPECIFIED_STATE
									case 1 :	r.state	= purchase_status::purchased;		break;		// Purchase.PurchaseState.PURCHASED
									case 2 :	r.state	= purchase_status::pending;			break;		// Purchase.PurchaseState.PENDING
									default:	break;
								}
								break;
							}
							case 1 :	r.state	= purchase_status::cancel;			break;
							case 2 :	rs		= status::network_error;			break;
							case 3 :	r.state	= purchase_status::disable;			break;
							case 4 :	r.state	= purchase_status::disable;			break;
							case 5 :	r.state	= purchase_status::disable;			break;
							case 6 :	rs		= status::unkown_error;				break;
							case 7 :	r.state	= purchase_status::already;			break;
							case 8 :	r.state	= purchase_status::not_owned;		break;
							default:	break;
						}
						r.identify	= std::to_string((jstring)p.getSku().get());
						details.push_back(std::move(r));
					}
				}
				else if (!mLastPurchaseSku.empty())
				{
					// 購入済みのアイテムの場合は null を渡すことがある。
					purchase_result	r;
					r.state		= purchase_status::unkoen_error;
					switch (_responseCode)
					{
						case 0 :	r.state	= purchase_status::purchased;		break;
						case 1 :	r.state	= purchase_status::cancel;			break;
						case 2 :	rs		= status::network_error;			break;
						case 3 :	r.state	= purchase_status::disable;			break;
						case 4 :	r.state	= purchase_status::disable;			break;
						case 5 :	r.state	= purchase_status::disable;			break;
						case 6 :	rs		= status::unkown_error;				break;
						case 7 :	r.state	= purchase_status::already;			break;
						case 8 :	r.state	= purchase_status::not_owned;		break;
						default:	break;
					}
					r.identify	= std::move(mLastPurchaseSku);
					details.push_back(std::move(r));
				}
				mPurchaseCallback(rs, details.data(), details.size());
				mLastPurchaseSku.clear();
			}
		}


		void onSkuDetails(java::jobj<List> _details)
		{
			std::vector<sku_detail>	details;
			int							sizeOf	= _details.size();
			for (int i = 0; i < sizeOf; ++i)
			{
				auto		o	= _details.get((jint)i);
				auto		s	= java::jobj<SkuDetails>(o);

				sku_detail	d;
				d.identify		= std::to_string((jstring)s.getSku().get());
				d.price			= std::to_string((jstring)s.getPrice().get());
				d.priceCurrency	= std::to_string((jstring)s.getPriceCurrencyCode().get());
				d.rewarded		= s.isRewarded();
				details.push_back(std::move(d));
			}
			if (mDetailsCallback)
			{
				mDetailsCallback(details.data(), details.size());
			}
		}

		void onStartFinished(int _responseCode)
		{
			status	st	= status::success;
			switch (_responseCode)
			{
				case 0 : st = status::success;			break;
				case 1 : st = status::network_error;	break;
				case 2 : st = status::network_error;	break;
				case 3 : st = status::unsupported;		break;
				case 4 : st = status::network_error;	break;
				case 5 : st = status::network_error;	break;
				case 6 : st = status::unkown_error;		break;
				case 7 : st = status::network_error;	break;
				case 8 : st = status::network_error;	break;
				default: st = status::unkown_error;		break;
			}
			if (mConnectCallback)
			{
				mConnectCallback(st);
			}
		}

		java::globalref<jobject>																	mBilling;
		std::function<void(status _status)>															mConnectCallback;
		std::function<void(sku_detail const* _details, size_t _detailsCount)>						mDetailsCallback;
		std::function<void(status _status, purchase_result const* _details, size_t _detailsCount)>	mPurchaseCallback;
		std::string																					mLastPurchaseSku;
		bool																						mConnected	= false;
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
	using	Billing	= java::classes::jp::libamtrs::Billing::Billing;

	if (!*billing_manager_android::get_instance())
	{
		auto	thiz	= new billing_manager_android::interface();
		mInterface	= thiz;
		AMTRS_JAVA_JNIMETHOD_REGISTER(Billing, billing_manager_android::interface,
			AMTRS_JAVA_MAKE_JNIMETHOD(onPurchasesUpdate),
			AMTRS_JAVA_MAKE_JNIMETHOD(onStartFinished),
			AMTRS_JAVA_MAKE_JNIMETHOD(onSkuDetails)
		);

		auto	clsBilling		= Billing::find();
		auto	objAct			= os::android::get_activity();
		auto	obj				= clsBilling.init(reinterpret_cast<jlong>(mInterface.get()), objAct);
		thiz->mBilling		= java::globalref<jobject>(obj.get());

	}
	else
	{
		mInterface	= *billing_manager_android::get_instance();
	}
}


// ========================================================================
//! 課金システムに接続済みか調べる。
// ------------------------------------------------------------------------
inline extern bool billing_manager::connected() const noexcept
{
	return	static_cast<billing_manager_android::interface*>(mInterface.get())->connected();
}


// ========================================================================
//! 課金システムへ接続する。
// ------------------------------------------------------------------------
//! 接続が終わるとコールバックを呼び出す。
// ------------------------------------------------------------------------
inline extern void billing_manager::connect(std::function<void(status _status)> _callback)
{
	static_cast<billing_manager_android::interface*>(mInterface.get())->connect(std::move(_callback));
}


inline extern void billing_manager::purchase(std::function<void(status _status, purchase_result const* _details, size_t _detailsCount)> _callback, std::string_view _sku)
{
	static_cast<billing_manager_android::interface*>(mInterface.get())->purchase(std::move(_callback), _sku);
}



inline extern void billing_manager::loadSkuDetails(std::function<void(sku_detail const* _details, size_t _detailsCount)> _callback, implicit_virtual_iterator<std::string_view>& _first, implicit_virtual_iterator<std::string_view>& _last)
{
	static_cast<billing_manager_android::interface*>(mInterface.get())->loadSkuDetails(std::move(_callback), _first, _last);
}



// ========================================================================
//! 課金システムとの接続を解除する。
// ------------------------------------------------------------------------
inline extern void billing_manager::disconnect()
{
	static_cast<billing_manager_android::interface*>(mInterface.get())->disconnect();
}





AMTRS_BILLING_NAMESPACE_END
#endif
/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/ads/banner.hpp>
#include <amtrs/.driver/android-api-android_app.hpp>
#include <amtrs/java/android/widget/RelativeLayout.hpp>
#include <amtrs/java/com/google/android/gms/ads/AdListener.hpp>
#include <amtrs/java/com/google/android/gms/ads/AdRequest.hpp>
#include <amtrs/java/com/google/android/gms/ads/AdSize.hpp>
#include <amtrs/java/com/google/android/gms/ads/AdView.hpp>
#include <amtrs/java/com/google/android/gms/ads/MobileAds.hpp>
AMTRS_ADS_NAMESPACE_BEGIN

using namespace	java::classes::com::google::android::gms::ads;


class	banner_admob
		: public banner
{
public:
	banner_admob(position_types _position)
		: mPosition(_position)
	{}

	bool init(google_admob_params const& _param)
	{
		using		RelativeLayout	= java::classes::android::widget::RelativeLayout;

		auto	id		= java::jobj<java::classes::java::lang::String>(java::to_jstring(_param.unit_id));
		auto	adview	= AdView::find().init(os::android::get_activity());
		mAdView	= adview.get();
		adview.setAdSize(AdSize::find().get_BANNER());
		adview.setAdUnitId(id);

		auto	lcls	= RelativeLayout::find();
		auto	lpcls	= RelativeLayout::LayoutParams::find();
		auto	lp		= RelativeLayout::LayoutParams::init(lpcls.get_MATCH_PARENT(), lpcls.get_WRAP_CONTENT());
		lp.addRule(mPosition == top
			?	lcls.get_ALIGN_PARENT_TOP()
			:	lcls.get_ALIGN_PARENT_BOTTOM()
		);
		adview.setLayoutParams(lp);

		adview.loadAd(AdRequest::Builder::find().init().build());
		os::android::get_activity().addView(adview);
		return	true;
	}

	virtual void set_visibility(bool _visible) override
	{
		java::jobj<java::classes::android::view::View>	v(mAdView);
		v.setVisibility(_visible
			?	java::classes::android::view::View::find().get_VISIBLE()
			:	java::classes::android::view::View::find().get_GONE()
		);
	}


	position_types				mPosition;
	java::globalref<jobject>	mAdView;
};



ref<banner> banner::create(providor const* _prov, position_types _position)
{
	if (_prov->type == providor::google_admob)
	{
		ref<banner_admob>	thiz	= new banner_admob(_position);
		thiz->init(*static_cast<google_admob_params const*>(_prov));
		return	thiz;
	}
	return	{};
}


AMTRS_ADS_NAMESPACE_END

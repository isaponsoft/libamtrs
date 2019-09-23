/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__google__firebase_ios__hpp
#define	__libamtrs__google__firebase_ios__hpp
// Library
//	firebase_admob
//	firebase_app
// Framework
//	AudioToolbox.framework
//	AdSupport.framework
//	GLKit.framework
//	GoogleMobileAds.framework	(https://developers.google.com/admob/ios/download)
//	CoreMedia.framework
//	CoreMotion.framework
//	CoreVideo.framework
//	CFNetwork.framework
//	MediaPlayer.framework
//	MessageUI.framework
//	MobileCoreServices.framework
//	Security.framework
//	StoreKit.framework
//	SystemConfiguration.framework

#include "firebase.hpp"
AMTRS_GOOGLE_FIREBASE_NAMESPACE_BEGIN

ref<app> app::create()
{
	ref<app>	thiz	= new app();
	return	thiz;
}

AMTRS_GOOGLE_FIREBASE_NAMESPACE_END
#endif

/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__google__firebase_admob__hpp
#define	__libamtrs__google__firebase_admob__hpp
#include "../window.hpp"
#include "firebase.hpp"
AMTRS_GOOGLE_FIREBASE_NAMESPACE_BEGIN

class	admob
		: public ref_object
{
public:
	//! 広告の要求ターゲット(firebase_admob_adrequest.hpp)
	struct	adrequest;

	//! バナー型広告(firebase_admob_banner.hpp)
	class	banner;


	//! test mode 用の APP ID
	static constexpr const char*	ADMOB_TEST_APP_ID	= "ca-app-pub-3940256099942544~3347511713";

	
	static inline ref<admob> create(const char* _app_id = ADMOB_TEST_APP_ID, app* _app = nullptr);

protected:
	void initialize()
	{
		fb::admob::Initialize(*mApp->get_context(), mAppID.c_str());
	}


	ref<app>	mApp;
	std::string	mAppID;
};


ref<admob> admob::create(const char* _app_id, app* _app)
{
	ref<admob>	thiz	= new admob();

	if (_app)
	{
		thiz->mApp		= _app;
	}
	else
	{
		thiz->mApp		= app::create();
	}
	thiz->mAppID	= _app_id;
	thiz->initialize();

	return	thiz;
}


AMTRS_GOOGLE_FIREBASE_NAMESPACE_END
#include "firebase_admob_adrequest.hpp"
#include "firebase_admob_banner.hpp"
#endif

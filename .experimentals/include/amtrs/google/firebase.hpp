/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__google__firebase__hpp
#define	__libamtrs__google__firebase__hpp

// ****************************************************************************
// Firebase C++ SDK に include path を通す必要があります。
// ----------------------------------------------------------------------------
// https://firebase.google.com/docs/cpp/setup
// ----------------------------------------------------------------------------
#include <firebase/app.h>
#include <firebase/admob.h>
#include <firebase/admob/types.h>

#include "def.hpp"
#define	AMTRS_GOOGLE_FIREBASE_NAMESPACE_BEGIN	AMTRS_GOOGLE_NAMESPACE_BEGIN namespace firebase {
#define	AMTRS_GOOGLE_FIREBASE_NAMESPACE_END		} AMTRS_GOOGLE_NAMESPACE_END

AMTRS_GOOGLE_FIREBASE_NAMESPACE_BEGIN

namespace	fb	= ::firebase;


class	app
		: public ref_object
{
public:
	using	value_type	= fb::App*;

	static inline ref<app> create();


	//! firebase::App* を返す。
	value_type get_context() const noexcept { return mApp; }
	

protected:
	app() = default;

	value_type		mApp	= nullptr;
};



AMTRS_GOOGLE_FIREBASE_NAMESPACE_END
#if		AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_ANDROID
	#include "firebase-android.hpp"
#endif

#if		AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_WIN32
	#include "firebase-win32.hpp"
#endif

#if		AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_IOS
	#include "firebase-ios.hpp"
#endif

#endif

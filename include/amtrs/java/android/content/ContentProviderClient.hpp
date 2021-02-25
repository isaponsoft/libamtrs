/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__android__content__ContentProviderClient__hpp
#define	__libamtrs__android__java_classes__android__content__ContentProviderClient__hpp
#include "../../java/lang/String.hpp"
#include "../../android/net/Uri.hpp"
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN
namespace android::accounts {
struct	Account;
}
namespace android::content {
struct	ContentValues;
}
namespace android::database {
struct	Cursor;
struct	ContentObserver;
}
namespace android::os {
struct	Bundle;
}
namespace android::content {


AMTRS_JAVA_DEFINE_CLASS(ContentProviderClient, java::lang::Object)
{
	using	ContentValues	= android::content::ContentValues;
	using	Cursor			= android::database::Cursor;
	using	String			= java::lang::String;
	using	Uri				= android::net::Uri;
	using	ContentObserver	= android::database::ContentObserver;


	AMTRS_JAVA_CLASS_SIGNATURE("android/content/ContentProviderClient");

	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;

		AMTRS_JAVA_DEFINE_METHOD(close
			, void()
		)

		AMTRS_JAVA_DEFINE_METHOD_RENAMED(
			delete_, "delete",
			jint(Uri uri, String where, String selectionArgs[])
		)

		AMTRS_JAVA_DEFINE_METHOD(insert
			, Uri(Uri url, ContentValues values)
		)

		AMTRS_JAVA_DEFINE_METHOD(notifyChange
			, void(Uri uri, ContentObserver observer)
			, void(Uri uri, ContentObserver observer, jint flags)
			, void(Uri uri, ContentObserver observer, jboolean syncToNetwork)
		)

		AMTRS_JAVA_DEFINE_METHOD(
			query,
			Cursor(Uri uri, String projection[], String selection, String selectionArgs[], String sortOrder)
		)

		AMTRS_JAVA_DEFINE_METHOD(requestSync
			, void(android::accounts::Account account, String authority, android::os::Bundle extras)
		)

		AMTRS_JAVA_DEFINE_METHOD(
			update,
			jint(Uri uri, ContentValues values, String where, String selectionArgs[])
		)

		AMTRS_JAVA_DEFINE_METHOD(release
			, void()
		)
	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif

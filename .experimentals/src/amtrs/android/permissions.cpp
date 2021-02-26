/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/java/android/content/pm/PackageManager.hpp>
#include <amtrs/java/androidx/core/app/ActivityCompat.hpp>
#include <amtrs/.driver/android-api-amtrs_activity.hpp>
#include <amtrs/.driver/android-api-permissions.hpp>
#include <amtrs/filesystem.hpp>
AMTRS_OS_ANDROID_NAMESPACE_BEGIN


void permission(std::initializer_list<java::jobj<java::classes::java::lang::String>> _perms, std::function<void(bool)> _callback)
{
	std::vector<jobject>	requests;
	for (auto perm : _perms)
	{
		requests.push_back(java::get_jnienv()->NewGlobalRef(perm.get()));
	}
	os::android::amtrs_activity::get_instance()->regiseter_looper_listener((void*)&permission, [=]()
	{
		os::android::amtrs_activity::get_instance()->unregiseter_looper_listener((void*)&permission);

		auto					activity	= os::android::get_activity();
		auto					clsCompat	= java::classes::androidx::core::app::ActivityCompat::find();
		jint					grantFlg	= java::classes::android::content::pm::PackageManager::find().get_PERMISSION_GRANTED();

		std::vector<jobject>	nogrants;
		for (auto perm : requests)
		{
			if (clsCompat.checkSelfPermission(activity, java::jobj<java::classes::java::lang::String>(perm)) != grantFlg)
			{
				nogrants.push_back(perm);
			}
		}

		// すべてが許可済みなら即コールバックを呼び出す
		if (nogrants.empty())
		{
			_callback(true);
			return;
		}

		// アクティビティにパーミッションの結果コールバックを登録する
		int		requestCode	= os::android::amtrs_activity::get_instance()->register_permissions_result([=](auto perms, auto grants)
		{
			bool	grant	= true;
			int		length	= java::get_jnienv()->GetArrayLength(grants.get());
			jint*	data	= java::get_jnienv()->GetIntArrayElements(grants.get(), nullptr);
			for (int i = 0; i < length; ++i)
			{
				AMTRS_DEBUG_LOG("Permission register_permissions_result: grant %d", (int)data[i]);
				if (data[i] != grantFlg)
				{
					grant	= false;
				}
			}

			java::get_jnienv()->ReleaseIntArrayElements(grants.get(), data, 0);
			_callback(grant);
		});
		auto			clsStr		= java::classes::java::lang::String::find();
		jobjectArray 	objAry		= java::get_jnienv()->NewObjectArray(
										(int)nogrants.size(),
										java::classes::java::lang::String::find().get(),
										nullptr
									);
		for (int i = 0; i < (int)nogrants.size(); ++i)
		{
			java::get_jnienv()->SetObjectArrayElement(objAry, i, nogrants[i]);
		}
		auto	sa	= java::jobj<java::classes::java::lang::String[]>(java::local_obj(objAry));
		clsCompat.requestPermissions(activity, sa, requestCode);

		// 解放
		for (auto perm : requests)
		{
			java::get_jnienv()->DeleteGlobalRef(perm);
		}
	});
}

#if	0


void permission(java::jobj<java::classes::java::lang::String> _permtype, std::function<void(bool)> _callback)
{
	jobject	perm	= java::get_jnienv()->NewGlobalRef(_permtype.get());
	os::android::amtrs_activity::get_instance()->regiseter_looper_listener(perm, [_perm = perm, _callback]()
	{
		os::android::amtrs_activity::get_instance()->unregiseter_looper_listener(_perm);

		auto	activity	= os::android::get_activity();
		auto	clsCompat	= java::classes::androidx::core::app::ActivityCompat::find();
		jint	grantFlg	= java::classes::android::content::pm::PackageManager::find().get_PERMISSION_GRANTED();
		auto	perm		= java::jobj<java::classes::java::lang::String>(_perm);

		// 許可があるか調べ、許可済みなら即コールバックを呼び出す
		if (clsCompat.checkSelfPermission(activity, perm) == grantFlg)
		{
			// すでに許可されているのですぐにコールバックを呼び出す
			_callback(true);
			return;
		}

		// アクティビティにパーミッションの結果コールバックを登録する
		int		requestCode	= os::android::amtrs_activity::get_instance()->register_permissions_result([=](auto perms, auto grants)
		{
			bool	grant	= false;
			int		length	= java::get_jnienv()->GetArrayLength(grants.get());
			jint*	data	= java::get_jnienv()->GetIntArrayElements(grants.get(), nullptr);
			for (int i = 0; i < length; ++i)
			{
				AMTRS_DEBUG_LOG("Permission register_permissions_result: grant %d", (int)data[i]);
				if (data[i] == grantFlg)
				{
					grant	= true;
				}
			}

			java::get_jnienv()->ReleaseIntArrayElements(grants.get(), data, 0);
			_callback(grant);
		});

		auto	clsStr		= java::classes::java::lang::String::find();
		jobject	objAry		= java::get_jnienv()->NewObjectArray(
								1,
								java::classes::java::lang::String::find().get(),
								perm.get()
							);
		auto	sa	= java::jobj<java::classes::java::lang::String[]>(java::local_obj(objAry));
		clsCompat.requestPermissions(activity, sa, requestCode);

		java::get_jnienv()->DeleteGlobalRef(_perm);
	});
}

#endif


AMTRS_OS_ANDROID_NAMESPACE_END
AMTRS_FILESYSTEM_NAMESPACE_BEGIN


void permission(std::function<void(bool)> _callback)
{
	os::android::permission({ java::classes::android::Manifest::permission::find().get_READ_EXTERNAL_STORAGE() }, _callback);
}



AMTRS_FILESYSTEM_NAMESPACE_END

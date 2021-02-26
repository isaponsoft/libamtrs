/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__notification__alarm__hpp
#define	__libamtrs__notification__alarm__hpp
#include "../amtrs.hpp"
#include "../chrono/@"
#include "../utility/singleton.hpp"
AMTRS_NAMESPACE_BEGIN

class	alarm_manager
		: public ref_object
{
public:
	struct	alarm
	{
		// 通知のグループを指定します。
		// Androidにおける通知チャンネルになります。iOSでは無視されます。
		std::string		groupId;
		std::string		groupTitle;

		datetime		time;
		int				id;
		std::string		title;
		std::string		message;
		bool			wakeup	= false;		//!< trueの時はアプリを起動します
	};


	// ************************************************************************
	//!	アラームを生成します。
	// ------------------------------------------------------------------------
	//!	_receiverClassName
	//!		iOS では常に無視される。
	//!		Android では AmtrsBroadcastReceiver を継承し、AndroidManifest で指定されたクラスを指定する。
	//!		クラス名は my.domain.MyReceiver のように JNI の基準に準ずる。
	//!		proguard を使用している場合はクラス -keep public class my.domain.MyReceiver を追加して下さい;
	static ref<alarm_manager> create(const std::string& _receiverClassName = {});


	// ************************************************************************
	//!	アラームを追加します。
	// ------------------------------------------------------------------------
	virtual void add_alarm(const alarm& _notificate) = 0;


	// ************************************************************************
	//!	アラームをすべてキャンセルします。
	// ------------------------------------------------------------------------
	virtual void cancel_all() = 0;
};


AMTRS_NAMESPACE_END
#include AMTRS_DRIVER_INCLUDE(notification-alarm.hpp)
#endif

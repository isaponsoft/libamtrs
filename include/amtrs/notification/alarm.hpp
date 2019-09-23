/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
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
		datetime		time;
		int				id;
		std::string		title;
		std::string		message;
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
#include AMTRS_MAKE_INCLUDE_PLATFORM(alarm)
#endif

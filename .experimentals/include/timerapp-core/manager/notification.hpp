/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__manager__notification__hpp
#define	__manager__notification__hpp


struct	alarm_notification_manager
{
	//! 予約した通知をクリアします。
	static void clean();


	//! 通知をシステムにセットします。
	static void setup();
};


#endif

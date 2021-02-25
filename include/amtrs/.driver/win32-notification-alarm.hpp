/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__notification__alarm__win32__hpp
#define	__libamtrs__notification__alarm__win32__hpp
AMTRS_NAMESPACE_BEGIN

class	alarm_manager_win32
		: public alarm_manager
{
public:
	virtual void add_alarm(const alarm& _notificate) override
	{
	}


	virtual void cancel_all() override
	{
	}



protected:
	std::string			mReceiverClassName;

	friend	alarm_manager;
};


inline ref<alarm_manager> alarm_manager::create(const std::string& _receiverClassName)
{
	ref<alarm_manager_win32>	thiz	= new alarm_manager_win32();
	return	thiz;
}

AMTRS_NAMESPACE_END
#endif

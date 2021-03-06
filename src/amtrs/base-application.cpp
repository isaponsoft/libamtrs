/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#if		AMTRS_APPLICATION_ENABLE
#include <thread>
#include <amtrs/amtrs.hpp>
#include <amtrs/application.hpp>
#include <amtrs/window.hpp>
#include AMTRS_DRIVER_INCLUDE(application.hpp)
AMTRS_NAMESPACE_BEGIN

int application::run(int _argc, char* _args[])
{
	try
	{
		this->on_created(_argc, _args);
		window::messageloop([this]()
		{
			this->on_update();
			return	true;
		});
	}
#if	AMTRS_ERROR_LOG_USE
	catch (std::exception& e)
	{
		AMTRS_ERROR_LOG("Error : %s", e.what());
	}
#endif
	catch (...)
	{
		AMTRS_ERROR_LOG("Error : unkown exception");
	}
	return	0;
}



AMTRS_NAMESPACE_END
#endif

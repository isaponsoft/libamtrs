/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__application__application_win32__hpp
#define	__libamtrs__application__application_win32__hpp
#include "../../os/win32/windows.hpp"
AMTRS_NAMESPACE_BEGIN


inline filesystem::path application::cache_dir() const
{
	return	".";
}


inline filesystem::path application::documents_dir() const
{
	return	{};
}



inline filesystem::path application::files_dir(bool _external) const
{
	return	".";
}


inline void application::foreground()
{
}


inline int application::run(int _argc, char* _args[])
{
	this->on_created(_argc, _args);

	MSG		msg;
	while (!this->is_quit())
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				this->on_event(amtrs::application::system_event::stop);
				return	0;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		this->on_update();
	}
	return	0;
}



AMTRS_NAMESPACE_END
#endif

/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/application.hpp>
AMTRS_NAMESPACE_BEGIN


std::string application::cache_dir() const
{
	return	".";
}


std::string application::documents_dir() const
{
	return	{};
}



std::string application::files_dir(bool _external) const
{
	return	".";
}


void application::foreground()
{
}


int application::run(int _argc, char* _args[])
{
	this->on_created(_argc, _args);

	while (!this->is_quit())
	{
		MSG		msg;
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
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

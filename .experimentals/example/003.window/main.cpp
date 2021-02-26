/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/amtrs.hpp>
#include <amtrs/application.hpp>
#include <amtrs/window.hpp>


namespace ss	= AMTRS_NAMESPACE;


int main(int, char**)
{
	auto	w	= ss::window::create({640, 480}, [](auto& sender, auto& e)
	{
		switch (e)
		{
			case ss::window::event_type::activate :
			{
				std::cout << "Change Activate " << (e.wm.activate ? "ON" : "OFF") << std::endl;
				break;
			}

			case ss::window::event_type::created :
			{
				std::cout << "Created" << std::endl;
				break;
			}

			case ss::window::event_type::destroyed :
			{
				std::cout << "Destroyed" << std::endl;
				break;
			}

			case ss::window::event_type::sizechanged :
			{
				std::cout << "Size Changed " << e.wm.sizechanged.width << "x" << e.wm.sizechanged.height << std::endl;
				break;
			}

			case ss::window::event_type::input :
			{
				break;
			}

			case ss::window::event_type::repaint :
			{
				break;
			}
		}
		return	false;
	});


	ss::window::messageloop([]()
	{
		return	true;
	});
	return	0;
}


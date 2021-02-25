/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__main__main_winrt__hpp
#define	__libamtrs__main__main_winrt__hpp
#include "../../amtrs.hpp"
#include "../../application.hpp"


#define	AMTRS_GUI_MAIN(_mainclass)	\
		int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)\
		{\
			using namespace amtrs;\
			ref<_mainclass>	app = new _mainclass();\
			int	result = app->run(0, nullptr);\
			app = nullptr;\
			return	result;\
		}

#endif

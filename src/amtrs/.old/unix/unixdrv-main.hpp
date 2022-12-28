/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__main__main_posix__hpp
#define	__libamtrs__main__main_posix__hpp
#include <amtrs/amtrs.hpp>
#include <amtrs/application/@>



#define	AMTRS_GUI_MAIN(_mainclass)	\
		int main(int _argc, char* _args[])\
		{\
			using namespace amtrs;\
			auto	app = _mainclass;\
			int	result = app->run(_argc, _args);\
			app = nullptr;\
			return	result;\
		}

		
#endif

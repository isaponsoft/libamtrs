/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__main__main_win32__hpp
#define	__libamtrs__main__main_win32__hpp
#include "../../amtrs.hpp"
#include "../../os/win32.hpp"
#include "../../application.hpp"

#define	AMTRS_MAIN(_mainf)	\
		int _mainf(int _argc, char* _args[]);\
		int main(int _argc, char* _args[])\
		{\
			return	_mainf(_argc, _args);\
		}

#define	AMTRS_GUI_MAIN(_mainclass)	\
		extern "C" int main(int _argc, char* _args[])\
		{\
			using namespace amtrs;\
			ref<_mainclass>	app = new _mainclass();\
			int	result = app->run(_argc, _args);\
			app = nullptr;\
			return	result;\
		}

#endif

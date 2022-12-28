/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__main__main_win32__hpp
#define	__libamtrs__main__main_win32__hpp
#include "../../application.hpp"
#include "win32-api-windows.hpp"



#ifdef	_CONSOLE
	#define	AMTRS_GUI_MAIN(_mainclass)	\
			extern "C" int main(int _argc, char* _args[])\
			{\
				using namespace amtrs;\
				auto	app = _mainclass;\
				int	result = app->run(_argc, _args);\
				app = nullptr;\
				return	result;\
			}

	#define	AMTRS_ENTRY_POINT(_mainf)	\
			int _mainf(int _argc, char* _args[]);\
			int main(int _argc, char* _args[])\
			{\
				return	_mainf(_argc, _args);\
			}
#else
	#define	AMTRS_GUI_MAIN(_mainclass)	\
			extern "C" int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)\
			{\
				using namespace amtrs;\
				auto	app = _mainclass;\
				int	result = app->run(0, nullptr);\
				app = nullptr;\
				return	result;\
			}

	#define	AMTRS_ENTRY_POINT(_mainf)	\
			int _mainf(int _argc, char* _args[]);\
			int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)\
			{\
				return	_mainf(0, nullptr);\
			}
#endif

		
#endif

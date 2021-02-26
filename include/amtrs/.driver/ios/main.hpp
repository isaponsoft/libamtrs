/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__main__main_ios__hpp
#define	__libamtrs__main__main_ios__hpp
#include <amtrs/application/@>
#include <amtrs/logging/@>


#define	AMTRS_GUI_MAIN(_mainclass)	\
		extern "C" int main(int _argc, char* _args[])\
		{\
			using namespace amtrs;\
			@autoreleasepool {\
				auto	app = _mainclass;\
				return	UIApplicationMain(_argc, _args, nil, NSStringFromClass([AppDelegate class]));\
			}\
		}

#endif

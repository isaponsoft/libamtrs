/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__main__main_ios__hpp
#define	__libamtrs__main__main_ios__hpp
#include "../../application.hpp"
#include "../../logging.hpp"


#define	AMTRS_GUI_MAIN(_mainclass)	\
		extern "C" int main(int _argc, char* _args[])\
		{\
			using namespace amtrs;\
			@autoreleasepool {\
				ref<_mainclass>	app = new _mainclass();\
				return	UIApplicationMain(_argc, _args, nil, NSStringFromClass([AppDelegate class]));\
			}\
		}

#endif

/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__gframe__application__hpp
#define	__gframe__application__hpp
#include "def.hpp"

// アプリケーションが実装すべき関数群です。

//! アプリケーションの作成時に呼ばれます。
void gframe_application_created();


//! アプリケーションのレジューム時に呼ばれます。
void gframe_application_resume();


//! アプリケーションのポーズ時に呼ばれます。
void gframe_application_pause();


//! アプリケーションの停止時に呼ばれます。
void gframe_application_stop();


#endif

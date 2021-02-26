/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <main.hpp>
#include <amtrs/application/@>


class	myappli
		: public amtrs::application
{
public:
};


#include <amtrs/main/@>
AMTRS_GUI_MAIN(new myappli())

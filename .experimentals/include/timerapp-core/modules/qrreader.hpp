/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__module__qrreader__hpp
#define	__module__qrreader__hpp
#include "../contents.hpp"
namespace QRreader {

class	main_node
		: public contents::contents_base
{
public:
};



struct	create_params
{
	// カメラのサイズ
	int		min_width	= 300;
	int		max_width	= 480;
};


amtrs::ref<main_node> create(create_params _params = create_params());

}
#endif

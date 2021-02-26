/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libscheduler__system__hpp
#define	__libscheduler__system__hpp
#include <gframe/@>
namespace dialog {


struct	dialog_params
{
	static constexpr unsigned int	use_color	= 1<<0;

	std::string					title;
	amtrs::ref<amtrs::node>		custom;
	std::string					text;
	std::vector<std::string>	buttons;
	int							back	= 0;

	unsigned int				flags	= 0;
	amtrs::rgba<float>			color;
};


void show(dialog_params const& _dp, std::function<void(int)> _callback);


void show(
	  std::string							_text
	, std::initializer_list<std::string>	_buttons
	, int									_back
	, std::function<void(int)> _callback
);


amtrs::ref<amtrs::node> initialize();

}

namespace timer {


//! タイマーを作成します。
//! ベースシステムで使用されるためアプリケーションから呼ぶ必要はありません
amtrs::ref<amtrs::node> initialize();


} // namespace timer



namespace wallpaper {


amtrs::ref<amtrs::node> initialize();


//! save::storage に設定されている壁紙で更新をかけます。
void update();


} // namespace wallpaper
#endif

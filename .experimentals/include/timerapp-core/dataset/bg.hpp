/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libscheduler__bg__hpp
#define	__libscheduler__bg__hpp
#include <map>
#include <string>
#include <vector>

// ****************************************************************************
//! 背景画像
// ----------------------------------------------------------------------------
struct	bg
{
	struct	item
	{
		std::string					title;		//!< タイトル
		std::string					filename;	//!< ファイル名
		std::string					productId;	//!< product identify
	};


	struct	group
	{
		std::string			name;
		std::vector<item>	items;

		auto begin() const noexcept { return items.begin(); }
		auto end() const noexcept { return items.end(); }
	};



	std::vector<group>	groups;

	auto begin() const noexcept { return groups.begin(); }
	auto end() const noexcept { return groups.end(); }

	static bool has_bg();

	static void initialize(std::string const& _configfile);

	static bg const* get_bg();


	static group const* find_group(std::string const& _group);

	//! 指定した名前もしくはファイル名の画像を検索する
	static item const* find_item(std::string const& _item);

	//! すべてのアイテムを列挙して返す
	static std::vector<item const*> enum_items();

	//! ファイル名に _prev を付与します
	static std::string prev_filename(std::string const& _filename);
};


#endif

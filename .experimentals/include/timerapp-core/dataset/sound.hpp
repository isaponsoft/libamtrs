/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libscheduler__sound__hpp
#define	__libscheduler__sound__hpp

struct	sound
{
	struct	item
	{
		std::string		title;
		std::string		filename;
	};

	struct	group
	{
		std::string			name;
		std::vector<item>	items;
		std::string			product;	//!< ストア（プロダクトID）

		auto begin() const noexcept { return items.begin(); }
		auto end() const noexcept { return items.end(); }
	};


	std::vector<group>	groups;

	auto begin() const noexcept { return groups.begin(); }
	auto end() const noexcept { return groups.end(); }

	static bool has_sound();

	static void initialize(std::string const& _configfile);

	static sound const* get_instance();


	static group const* find_group(std::string const& _group);

	//! 指定した名前もしくはファイル名の画像を検索する
	static item const* find_item(std::string const& _item);

	//! すべてのアイテムを列挙して返す
	static std::vector<std::pair<group const*, item const*>> enum_items();
};


#endif

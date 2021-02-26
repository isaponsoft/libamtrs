/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <timerapp-core/node.hpp>
#include <amtrs/chrono/time.hpp>
#include <amtrs/node/node.hpp>
#include <amtrs/node/fillwrap_node.hpp>
#include <amtrs/node/flexbox_node.hpp>
#include <amtrs/node/linerlayout_node.hpp>
#include <amtrs/node/scrollbox_node.hpp>

#include <timerapp-core/application.hpp>
#include <timerapp-core/contents.hpp>
#include <timerapp-core/system.hpp>
#include <timerapp-core/manager/purchase.hpp>

using namespace ::amtrs;
using namespace ::gframe;
using namespace ::ui;



class	shop_list_contents
		: public scrollbox_node<flexbox_node<contents::shop_list>>
{
public:
	using	super_type	= scrollbox_node<flexbox_node<contents::shop_list>>;

	virtual std::string title() override
	{
		return	u8"ショップ";
	}

	shop_list_contents(save::storage* _storage)
		: mStorage(_storage)
	{
		this->set_name("shop");

		start_connect();
	}

	void start_connect()
	{
		contents::loading(true);
		purchase::resume([this](auto _state, auto _prods, auto _size)
		{
			AMTRS_DEBUG_LOG("purchase::resume(%d)", (int)_state);
			contents::loading(false);
			if (_state == purchase::connection_status::ok)
			{
				this->schedule_once([this]()
				{
					refresh();
				});
			}
			else
			{
				this->schedule_once([this]()
				{
					dialog::dialog_params	dp;
					dp.title	= u8"商品";
					dp.text		= u8"課金サーバーとの通信に失敗しました。通信状態の良いところに移動して再度試してみてください。";
					dp.buttons.push_back(u8"OK");
					dialog::show(dp, [this](auto msg)
					{
						this->pop();
					});
				});
			}
		});
	}

	void refresh()
	{
		auto						prods	= purchase::get_products();
		auto						svdata	= save::storage::current();
		bool						saveup	= false;
		std::vector<std::string>	rewards;

		// まずは追加
		for (auto prod : make_bucket(prods.first, prods.second))
		{
			// 購入済みのアイテムがセーブリストにのっていなければ追加

			if (prod.rewarded && !svdata->purchase.is_purchased(prod.identify))
			{
				AMTRS_DEBUG_LOG("Restore item %s", prod.identify.c_str());
				svdata->purchase.eternals.push_back({prod.identify, save::timetype::now()});
				rewards.push_back(prod.title);
				saveup	= true;
			}

			this->async_schedule("shoplist", [=]()
			{
				auto	thumb		= load_texture(format<std::string>("shop/%s_icon.jpg", prod.productId.c_str()));
				auto	itemnode	= purchase::create_purchase_item(
											//purchase::create_params
											{
												prod,
												std::move(thumb)
											},
											// listener
											[=]() -> unsigned int
											{
												this->set_contents(contents::shop_detail::create(prod, save::storage::current()));
												return	0;
											}
									);
				this->add_item(itemnode.get(), use_scheduler());
			});
		}

		if (saveup)
		{
			dialog::dialog_params	dp;
			dp.title	= u8"商品";
			dp.text		= u8"以前に購入した商品の情報を復元しました。\n復元したアイテムは\n";
			for (auto& i : rewards)
			{
				dp.text	+= u8"「";
				dp.text	+= i;
				dp.text	+= u8"」\n";
			}
			dp.text	+= u8" です。";
			dp.buttons.push_back(u8"OK");
			dialog::show(dp, [](auto msg)
			{
			});
			save::storage::current()->save();
		}
	}

protected:
	ref<save::storage>					mStorage;
};



amtrs::ref<contents::contents_base> contents::shop_list::create(save::storage* _storage)
{
	if (!_storage)
	{
		_storage	= save::storage::current();
	}
	return	new shop_list_contents(_storage);
}

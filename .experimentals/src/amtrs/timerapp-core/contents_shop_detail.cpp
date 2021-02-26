/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <timerapp-core/node.hpp>

#include <amtrs/chrono/time.hpp>
#include <amtrs/node/node.hpp>
#include <amtrs/node/anchor_node.hpp>
#include <amtrs/node/circle_node.hpp>
#include <amtrs/node/clipping_node.hpp>
#include <amtrs/node/fill_node.hpp>
#include <amtrs/node/fillwrap_node.hpp>
#include <amtrs/node/flickable_paging_node.hpp>
#include <amtrs/node/flexbox_node.hpp>
#include <amtrs/node/scrollbox_node.hpp>
#include <amtrs/node/linerlayout_node.hpp>
#include <amtrs/node/string_node.hpp>

#include <timerapp-core/application.hpp>
#include <timerapp-core/contents.hpp>

using namespace ::amtrs;
using namespace ::gframe;


class	shop_images_preview_node
		: public children_verticalwrap_node<flexbox_node<node>>
{
public:
	using	super_type	= children_verticalwrap_node<flexbox_node<node>>;

	using	marker_type		= circle_node<node>;
	using	imageview_type	= flickable_paging_node<node, paging_node_traits<node, void>>;

	shop_images_preview_node(purchase::purchase_product const& _product, save::storage* _storage)
	{
		this->set_justify(justify_content_type::space_between);
		this->set_spacing({0, 30});

		mImageView	= new clipping_node<imageview_type>();
		this->add_item(mImageView);
		mImageView->set_scroll_axis(flickable_paging_node_base::axis_x);

		for (auto itm : bg::enum_items())
		{
			AMTRS_DEBUG_LOG("%s == %s", itm->productId.c_str(), _product.identify.c_str());
			if (itm->productId != _product.identify)
			{
				continue;
			}
			auto	s	= new sprite_node<node>(load_texture(bg::prev_filename(itm->filename)));
			s->set_scale({2, 2, 2});
			mImageView->push_page(s, {1,0,0});

			auto	m	= new button_node<marker_type>();
			m->set_trigger_size_expand({10, 50});
			m->set_trigger_callback([=](auto)
			{
				mImageView->set_active_page(static_cast<node*>(s));
			});

			mMaker.push_back(this->add_item(m));
		}

		for (auto m : mMaker)
		{
			m->set_color({0.7f, 0.7f, 0.7f, 1.0f});
			m->set_size({50, 50, 0});
		}
		mMaker[0]->set_color({0, 0, 0, 1.0f});


		auto	first	= mImageView->child(0);
		mImageView->set_size(first->scaled_size());

		mImageView->set_page_changed_listenr([this](auto pageindex)
		{
			for (auto m : mMaker)
			{
				m->set_color({0.7f, 0.7f, 0.7f, 1.0f});
			}
			mMaker[pageindex.x]->set_color({0, 0, 0, 1.0f});
		});
		this->set_width(mImageView->scaled_size().width);
	}


	imageview_type*				mImageView;
	std::vector<marker_type*>	mMaker;
};


class	containor_node
		: public horizontalwrap_node<children_verticalwrap_node<node>>
{
public:
	using	super_type	= horizontalwrap_node<children_verticalwrap_node<node>>;

	containor_node()
	{
		this->set_padding({30,30,0}, {30,30,0});
	}
};



class	shop_detail_contents
		: public fillwrap_node<scrollbox_node<flexbox_node<fill_node<contents::shop_detail>>>>
{
public:
	using	super_type	= fillwrap_node<scrollbox_node<flexbox_node<fill_node<contents::shop_detail>>>>;

	virtual std::string title() override
	{
		return	u8"商品詳細";
	}

	shop_detail_contents(purchase::purchase_product const& _product, save::storage* _storage)
		: mProduct(_product)
		, mStorage(_storage)
	{
		this->set_name("shop");
		this->set_color({1, 1, 1, 1});

		auto	title	= new anchor_node<string_node<node>>(get_font(font_type::ft_large));
		title->set_anchor({anchor_node_base::center, anchor_node_base::none, anchor_node_base::none});
		title->set_string(mProduct.title);
		title->set_pivot({0.5f, 0, 0});
		this->add_item(new containor_node())->add_child(title);

		auto	iv		= new anchor_node<shop_images_preview_node>(_product, _storage);
		iv->set_anchor({anchor_node_base::center, anchor_node_base::none, anchor_node_base::none});
		iv->set_pivot({0.5f, 0, 0});
		this->add_item(new containor_node())->add_child(iv);


		auto	price	= new string_node<node>(get_font(font_type::ft_big));
		price->set_string(mProduct.price);
		this->add_item(price);

		bool	rewa	= save::storage::current()->purchase.is_purchased(_product.identify);
		auto	btn		= new button_node<anchor_node<sprite_node<node>>>(load_texture("ui/btn_yes_l.png"));
		btn->set_anchor({anchor_node_base::center, anchor_node_base::none, anchor_node_base::none});
		btn->set_pivot({0.5f, 0, 0});
//		btn->set_anchor({anchor_node_base::center, anchor_node_base::none, anchor_node_base::center});
//		btn->set_pivot({0.5f, 0, 0});
		btn->set_trigger_callback([this, rewa](auto pos)
		{
			if (!rewa)
			{
				AMTRS_DEBUG_LOG("BUY");
				buy();
			}
		});

		auto	lbl	= new anchor_node<string_node<node>>(get_font(font_type::ft_button));
		lbl->set_anchor({anchor_node_base::center, anchor_node_base::center, anchor_node_base::none});
		lbl->set_string(rewa ? u8"購入済み" : u8"購入する");
		lbl->set_pivot({0.5f, 0.5f, 0});
		btn->add_child(lbl);
		this->add_item(new containor_node())->add_child(btn);
		mButtonLabel	= lbl;
	}


	// 購入開始
	void buy()
	{
		contents::loading(true);
		purchase::start_purchase(mProduct.productId, [this](auto _status)
		{
			AMTRS_DEBUG_LOG("Status %d", (int)_status);
			if (_status == purchase::purchase_status::purchased || _status == purchase::purchase_status::already)
			{
				if (!save::storage::current()->purchase.is_purchased(mProduct.identify))
				{
					this->schedule_once([this]()
					{
						save::storage::current()->purchase.eternals.push_back({mProduct.identify, save::timetype::now()});
						save::storage::current()->save();
						mButtonLabel->set_string(u8"購入済み");
					});
				}
			}
			contents::loading(false);
		});
	}

protected:
	purchase::purchase_product			mProduct;
	ref<save::storage>					mStorage;
	string_node<node>*					mButtonLabel;
};



amtrs::ref<contents::contents_base> contents::shop_detail::create(purchase::purchase_product const& _product, save::storage* _storage)
{
	if (!_storage)
	{
		_storage	= save::storage::current();
	}
	return	new shop_detail_contents(_product, _storage);
}

/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <opencv2/opencv.hpp>

#include <timerapp-core/modules/qrreader.hpp>
#include <amtrs/node/anchor_node.hpp>
#include <amtrs/node/qrcode_node.hpp>
#include <amtrs/node/fill_node.hpp>
#include <amtrs/node/fillwrap_node.hpp>
#include <amtrs/node/text_node.hpp>
#include <amtrs/net/web.hpp>
#include <amtrs/string/starts_with.hpp>

#include <timerapp-core/application.hpp>
#include <timerapp-core/contents.hpp>
#include <timerapp-core/system.hpp>

using namespace amtrs;



class	qrreader
		: public fillwrap_node<updatable_node<qrcode_node<QRreader::main_node>>>
{
public:
	using	super_type	= fillwrap_node<updatable_node<qrcode_node<QRreader::main_node>>>;

	qrreader(QRreader::create_params _params)
		: super_type(std::move(_params))
	{
		this->set_name("qrcode");
	}


	virtual std::string title() override
	{
		return	u8"QRコード";
	}


protected:
	virtual void on_qr_decoded(std::string& _text) override
	{
		super_type::on_qr_decoded(_text);
		this->pause();

		std::string	text	= std::move(_text);

		auto	box			= new horizontalwrap_node<children_verticalwrap_node<fill_node<node>>>();
		box->set_color({1,1,1,1});
		box->set_padding({50,50,0}, {50,50,0});
		box->set_pivot({0, 1, 0});
		mTextBox	= this->add_child(box);


		auto*	textnode	= new text_node<node>(::get_font());
		textnode->set_width_adjust(500);
		textnode->set_string(text);
		box->add_child(textnode);

		if (starts_with(text, "http://") || starts_with(text, "https://"))
		{
			mMenuBox	= this->add_child(create_bottom_menu({ u8"ブラウザ", u8"無視" }, 1, [this, url = std::move(text)](auto sender, int _id)
			{
				mMenuBox	= nullptr;
				mTextBox->remove_from_parent();
				sender->remove_from_parent();
				this->restart();
				if (_id == 0)
				{
					web::jump_to(url);
				}
			}));
		}
		update_position();
	}

	void on_size_change(typename super_type::size_type& _size) override
	{
		super_type::on_size_change(_size);
		update_position();
	}

	void on_children_size_change(typename super_type::transform_component* _c) override
	{
		super_type::on_children_size_change(_c);
		update_position();
	}

	void update_position()
	{
		if (mTextBox && mMenuBox)
		{
			mTextBox->set_position_y(this->children_area().height - mMenuBox->size().height);
		}
	}

	node*	mMenuBox	= nullptr;
	node*	mTextBox	= nullptr;
};


amtrs::ref<QRreader::main_node> QRreader::create(create_params _params)
{
	using	type	= qrreader;
	ref<type>	r	= new type(std::move(_params));
	return	r;
}

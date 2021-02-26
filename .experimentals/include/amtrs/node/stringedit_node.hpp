/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__stringedit_node__hpp
#define	__libamtrs__node__stringedit_node__hpp
#include "button_node.hpp"
#include "string_node.hpp"
AMTRS_NAMESPACE_BEGIN

template<class SuperT, class StringNode = string_node<typename SuperT::node_type>>
class	stringedit_node
		: public button_node<SuperT>
{
public:
	using	super_type	= button_node<SuperT>;

	template<class... Args>
	stringedit_node(Args&&... _args)
	{
		mString	= new StringNode(std::forward<Args>(_args)...);
		mString->set_pivot({0, 0.5f, 0});
		this->add_child(mString);
	}

	template<class... Args> void set_font(Args&&... _args)
	{
		mString->set_font(std::forward<Args>(_args)...);
	}

	template<class... Args> void set_string(Args&&... _args)
	{
		mString->set_string(std::forward<Args>(_args)...);
	}


	std::string const& get_string() const noexcept
	{
		return	mString->get_string();
	}

	void set_input_enable(bool _enable)
	{
		mInputEnable	= _enable;
	}


protected:
	virtual void on_text_changing(std::string& _text)
	{
	}


	//! true を返した場合入力済みのテキストをけっていこうとする。
	virtual bool on_text_changed(std::string const& _text)
	{
		return	true;
	}


	virtual void on_button_trigger(vec3<float> _pos) override
	{
		super_type::on_button_trigger(_pos);
		if (mInputEnable)
		{
			mTitleInput	= textinput_dialog::create();
			mTitleInput->set_string(mString->get_string());
			mTitleInput->set_return_callback([this]()
			{
				auto	txt	= mTitleInput->text();
				if (on_text_changed(txt))
				{
					mString->set_string(txt);
				}
				mTitleInput->close();
				mTitleInput	= nullptr;
			});
		}
	}


	virtual void on_size_change(typename super_type::size_type& _size) override
	{
		super_type::on_size_change(_size);
		mString->set_position({0, _size.height/2, 0});
	}

	ref<textinput_dialog>		mTitleInput;
	StringNode*					mString			= nullptr;
	bool						mInputEnable	= true;
};



AMTRS_NAMESPACE_END
#endif

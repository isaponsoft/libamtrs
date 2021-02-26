/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__label_node__hpp
#define	__libamtrs__node__label_node__hpp
#include "node.hpp"
#include "string_node.hpp"
#include "updatable_node.hpp"
#include "ui_utils.hpp"
AMTRS_NAMESPACE_BEGIN



template<class NodeT, class StringNodeT = string_node<typename NodeT::node_type>>
class	label_node
		: public NodeT
{
	using	_base_type	= NodeT;
	using	string_node	= StringNodeT;
public:
	enum class horizontal_align
	{
		left,
		center,
		right,
	};

	enum class vertical_align
	{
		top,
		middle,
		bottom,
	};

	static constexpr horizontal_align	left	= horizontal_align::left;
	static constexpr horizontal_align	center	= horizontal_align::center;
	static constexpr horizontal_align	right	= horizontal_align::right;
	static constexpr vertical_align		top		= vertical_align::top;
	static constexpr vertical_align		middle	= vertical_align::middle;
	static constexpr vertical_align		bottom	= vertical_align::bottom;

	using	_base_type::_base_type;

	label_node()
	{
		initialize();
	}

	label_node(font* _font)
	{
		initialize();
		mStringNode->set_font(_font);
	}

	label_node(std::string_view _text)
	{
		initialize();
		set_text(_text);
	}

	void set_text(std::string_view _text)
	{
		mStringNode->set_text(_text);
		string_node_reposition();
	}

	void set_font(font* _font)
	{
		mStringNode->set_font(_font);
	}

	void set_halign(horizontal_align _a)
	{
		mHAlign	= _a;
		string_node_reposition();
	}

	void set_valign(vertical_align _a)
	{
		mVAlign	= _a;
		string_node_reposition();
	}

	void update()
	{
		string_node_reposition();
	}

protected:
	virtual void on_size_change(typename _base_type::size_type& _size) override
	{
		_base_type::on_size_change(_size);
		if (mStringNode)
		{
			string_node_reposition();
		}
	}


private:
	void initialize()
	{
		mStringNode	= new string_node();
		this->add_child(mStringNode);
	}

	// 文字列ノードを開放する。
	void string_node_release()
	{
		if (mStringNode)
		{
			mStringNode->remove_from_parent();
			mStringNode	= nullptr;
		}
	}

	// 文字列ノードの位置を調整する
	void string_node_reposition()
	{
		vec3<float>	pvt	= {0, 0, 0};
		vec3<float>	pos	= {0, 0, 0};
		if (mHAlign == center)
		{
			pvt.x	= 0.5f;
			pos.x	= this->size().width / 2;
		}
		else if (mHAlign == right)
		{
			pvt.x	= 1.0f;
			pos.x	= this->size().width;
		}

		if (mVAlign == middle)
		{
			pvt.y	= 0.5f;
			pos.y	= this->size().height / 2;
		}
		else if (mVAlign == bottom)
		{
			pvt.y	= 1.0f;
			pos.y	= this->size().height;
		}
		mStringNode->set_pivot(pvt);
		mStringNode->set_position(pos);
	}

	string_node*		mStringNode	= nullptr;
	horizontal_align	mHAlign		= left;
	vertical_align		mVAlign		= middle;
};


AMTRS_NAMESPACE_END
#endif

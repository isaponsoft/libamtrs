/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__stringable_node__hpp
#define	__libamtrs__node__stringable_node__hpp
#include "../graphics/font.hpp"
#include "node.hpp"
AMTRS_NAMESPACE_BEGIN

template<class StringT>
struct	strintable_traits
{
	using	string_type	= StringT;
	using	font_type	= ref<font>;
};


AMTRS_IMPLEMENTS_BEGIN(stringable_node)
template<class NodeT>
struct stringable_node_valid
{};

template<class SuperT, class TraitsT>
class stringable_node_impl
		: public SuperT
		, public stringable_node_valid<typename SuperT::node_type>
{
public:
	using	super_type		= SuperT;
	using	traits_type		= TraitsT;
	using	string_type		= typename traits_type::string_type;
	using	font_type		= typename traits_type::font_type;
	using	node_type		= typename super_type::node_type;
	using	position_type	= typename super_type::position_type;
	using	box_type		= typename super_type::box_type;
	using	on_child_event	= typename super_type::on_child_event;

	stringable_node_impl()
	{}

	template<class... Args>
	stringable_node_impl(font_type _font, Args&&... _args)
		: super_type(std::forward<Args>(_args)...)
		, mFont(std::move(_font))
	{}

	template<class... Args>
	stringable_node_impl(string_type _string, font_type _font, Args&&... _args)
		: super_type(std::forward<Args>(_args)...)
		, mString(std::move(_string))
		, mFont(std::move(_font))
	{
		update_schedule();
	}


	template<class T>
	font_type const set_font(T const& _font)
	{
		if (mFont != _font)
		{
			_set_font(font_type(_font));
		}
		return	get_font();
	}

	font_type const set_font(font_type _font)
	{
		if (mFont != _font)
		{
			_set_font(std::move(_font));
		}
		return	get_font();
	}

	string_type const set_string(string_type _text)
	{
		if (mString != _text)
		{
			_set_text(std::move(_text));
		}
		return	get_string();
	}

	void string_update()
	{
		on_stringable_state();
	}


	font_type const& get_font() const noexcept { return mFont; }
	string_type const& get_string() const noexcept { return mString; }

protected:
	virtual void on_stringable_state() {}

private:
	void update_schedule()
	{
		if (!mChangeEvent)
		{
			mChangeEvent	= true;
			this->schedule_once([this](float)
			{
				mChangeEvent	= false;
				on_stringable_state();
			});
		}
	}

	void _set_font(font_type _font)
	{
		mFont			= std::move(_font);
		update_schedule();
	}

	void _set_text(string_type _string)
	{
		mString			= std::move(_string);
		update_schedule();
	}

	string_type	mString;
	font_type	mFont;
	bool		mChangeEvent	= false;
};
AMTRS_IMPLEMENTS_END(stringable_node)

template<class NodeT>
struct	is_stringable_node
		:	std::conditional<
				std::is_base_of<AMTRS_IMPLEMENTS(stringable_node)::stringable_node_valid<typename NodeT::node_type>, NodeT>::value,
				std::true_type,
				std::false_type
			>::type
{};

// ****************************************************************************
//! 文字列を持つノード
// ----------------------------------------------------------------------------
//! string_node, text_node などフォントと文字列を持つノードの共通機能です
// ----------------------------------------------------------------------------
template<class SuperT, class StringT = std::string>
using	stringable_node	=
			typename std::conditional<
				std::is_base_of<AMTRS_IMPLEMENTS(stringable_node)::stringable_node_valid<typename SuperT::node_type>, SuperT>::value,
				SuperT,															
				AMTRS_IMPLEMENTS(stringable_node)::stringable_node_impl<SuperT, strintable_traits<StringT>>
			>::type;



AMTRS_NAMESPACE_END
#endif

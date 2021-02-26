/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__updatable_node__hpp
#define	__libamtrs__node__updatable_node__hpp
#include "node.hpp"
AMTRS_NAMESPACE_BEGIN
AMTRS_IMPLEMENTS_BEGIN(updatable_node)
template<class NodeT> struct updatable_node_valid
{
	NodeT* v	= nullptr;
	bool operator == (const updatable_node_valid& _r) const noexcept { return v == _r.v; }
	bool operator != (const updatable_node_valid& _r) const noexcept { return v != _r.v; }
};
template<class NodeT> class updatable_node_enter
		: public NodeT
		, public updatable_node_valid<typename NodeT::node_type>
{
	using	super_type	= NodeT;
	using	_valid_type	= updatable_node_valid<typename NodeT::node_type>;
public:
	using	super_type::super_type;

protected:
	virtual void on_update(float _delta)
	{
	}

	virtual void on_enter() override
	{
		super_type::on_enter();
		this->schedule([this](float _delta)
		{
			on_update(_delta);
		});
	}

	virtual void on_exit() override
	{
		super_type::on_exit();
		this->unschedule_all();
	}
};
template<class NodeT> struct	updatable_node_base
{
	using	type	= typename std::conditional<
							// 既に on_update() を持っているかどうかを updatable_node_valid<NodeT::node_type> の有無で調べます。
							std::is_base_of<updatable_node_valid<typename NodeT::node_type>, NodeT>::value,
							NodeT,															// すでに on_update() があるので何もしないで継承
							AMTRS_IMPLEMENTS(updatable_node)::updatable_node_enter<NodeT>	// on_update() が無いので on_update() を追加する。
						>::type;
};

AMTRS_IMPLEMENTS_END(updatable_node)



// ****************************************************************************
//!	クラスに on_update() メソッドを持たせることができます。
// ----------------------------------------------------------------------------
//!	on_update() はノードがルートノードから辿れる時に自動で呼ばれるようになります。
//!	
//!	example)
//!		class	my_class : public updatable_node<node>
//!		{
//!			using	super_type	= updatable_node<node>;
//!		public:
//!			virtual void on_update(float _delta) override
//!			{
//!				super_type::on_update(_delta);
//!			}
//!		};
// ----------------------------------------------------------------------------
template<class NodeT>
class	updatable_node
		: public	AMTRS_IMPLEMENTS(updatable_node)::updatable_node_base<NodeT>::type
{
public:
	using	super_type	= typename AMTRS_IMPLEMENTS(updatable_node)::updatable_node_base<NodeT>::type;
	using	super_type::super_type;
};



AMTRS_NAMESPACE_END
#endif

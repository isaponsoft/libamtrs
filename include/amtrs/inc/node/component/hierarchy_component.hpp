/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__component__hierarchy_componenet__hpp
#define	__libamtrs__component__hierarchy_componenet__hpp
#include <algorithm>
#include <list>
AMTRS_NAMESPACE_BEGIN namespace component {

// ==================================================================
//! component_object に対して親子関係を持たせます。
// ------------------------------------------------------------------
template<class NodeT>
class	hierarchy_component
{
	using	_base_type	= NodeT;
public:
	using	node_type	= NodeT;

	static constexpr int	first_order	= std::numeric_limits<int>::min();
	static constexpr int	last_order	= std::numeric_limits<int>::max();

	~hierarchy_component();

	// ==========================================================
	//! 親を取得します。
	// ----------------------------------------------------------
	//! 親があるかどうかは is_child() で調べることができます。
	// ----------------------------------------------------------
	NodeT* parent() const noexcept { return mParent; }


	// ==========================================================
	//! 指定するノードが自身の子か調べます。
	// ----------------------------------------------------------
	//! 子であるなら true を返します。
	// ----------------------------------------------------------
	bool is_child(node_type* _child) const noexcept { return (_child && _child->parent() == this) ? true : false; }


	// ==========================================================
	//! 子を追加します。
	// ----------------------------------------------------------
	//! 子が追加されたとき、NodeT::on_add_child(hierarchy, _child) を
	//! 呼び出します。
	// ----------------------------------------------------------
	auto add_child(NodeT* _key) -> NodeT*;

	// ==========================================================
	//! 子を指定した位置に追加します。
	// ----------------------------------------------------------
	//! 子が追加されたとき、NodeT::on_add_child(hierarchy, _child) を
	//! 呼び出します。
	// ----------------------------------------------------------
	auto insert_child(NodeT* _pos, NodeT* _key) -> NodeT*;

	// ==========================================================
	//! 子を削除します。
	// ----------------------------------------------------------
	//! オブジェクトとKeyTを比較し一致するオブジェクトを削除します。
	//! on_remove_child(), on_removed_child(), on_lost_parent()
	//! が呼ばれます。
	// ----------------------------------------------------------
	bool remove_child(NodeT* _key);

	// ==========================================================
	//! 自身を親から削除します。
	// ----------------------------------------------------------
	//! this->parent()->remove_child(this) と同じです。
	// ----------------------------------------------------------
	bool remove_from_parent();

	// ==========================================================
	//! 子に対してコールバックを呼び出します。
	// ----------------------------------------------------------
	template<class FuncT> void visit_children(FuncT&& _func, bool _enableOnly = true);
	template<class FuncT> void visit_children(FuncT&& _func, bool _enableOnly = true) const;

	// ==========================================================
	//! 子に対してコールバックを呼び出します。
	// ----------------------------------------------------------
	template<class FuncT> void visit_children_r(FuncT&& _func);
	template<class FuncT> void visit_children_r(FuncT&& _func) const;


	// ==========================================================
	//! 子の数を取得します。
	// ----------------------------------------------------------
	std::size_t children_count() const noexcept { return c.size(); }


	// ==========================================================
	//! 低速ですが子をインデックスで取得します。
	// ----------------------------------------------------------
	NodeT* child(std::size_t _index) const noexcept;


	// ==========================================================
	//! ノードの優先度を取得します。値が小さいほど先に処理されます。
	// ----------------------------------------------------------
	int priority() const noexcept { return mPriority; }

	// ==========================================================
	//! ノードの優先度を変更します。値が小さいほど先に処理されます。
	// ----------------------------------------------------------
	void set_priority(int _priotity) noexcept
	{
		mPriority	= _priotity;
		if (mParent)
		{
			mParent->mSort	= true;
		}
	}


	node_type* top()
	{
		node_type*	t = parent();
		if (!t)
		{
			return	static_cast<node_type*>(this);
		}
		node_type*	p	= t->parent();
		while (p)
		{
			t	= p;
			p	= p->parent();
		}
		return	t;
	}

	bool is_enable() const noexcept { return mEnable; }
	void set_enable(bool _enable) noexcept { mEnable = _enable; }


protected:
	enum class on_child_event
	{
		add,
		remove
	};

	// ==========================================================
	//! 親ノードが変更されたときに呼ばれます。
	// ----------------------------------------------------------
	//! 親を失った場合は nullptr が渡されます。	
	// ----------------------------------------------------------
	virtual void on_parent(node_type* _parent)
	{}

	// ==========================================================
	//! 子ノードが変更されたときに呼ばれます。
	// ----------------------------------------------------------
	//! 引数には変更のあった子ノードが渡されます。
	// ----------------------------------------------------------
	virtual void on_child(node_type* _child, on_child_event _event)
	{}


	bool is_enable(NodeT* _obj) const noexcept
	{
		return	_obj->is_enable() && _obj->mParent == this;
	}

private:
	struct	holder_type
	{
		holder_type() = default;
		holder_type(const holder_type&) = default;
		holder_type(holder_type&&) = default;
		holder_type(NodeT* _node)
			: node(_node)
#if	DEBUG
			, name(typeid(*_node).name())
#endif
		{}


		ref<NodeT>	node;
#if	DEBUG
		std::string	name;		// DEBUG
#endif
	};

	bool is_enable(const holder_type& _child) const noexcept
	{
		return (!_child.node.empty()) && is_enable(_child.node);
	}


	void schedule_compaction()
	{
		if (!mListCompaction)
		{
			mListCompaction	= true;
			static_cast<NodeT*>(this)->schedule_once([this](auto)
			{
				mListCompaction	= false;
				c.remove_if([this](auto child) { return (!child.node.empty()) && (child.node->mParent != this); });
				c.sort([](auto& a, auto& b) { return a.node->mPriority < b.node->mPriority; });
			});
		}
	}


	using	element			= holder_type;
	      NodeT* owner()       noexcept { return static_cast<      NodeT*>(this); }
	const NodeT* owner() const noexcept { return static_cast<const NodeT*>(this); }


	// ポインタとオブジェクトからそれぞれ、オブジェクトの参照に型変換するヘルパ。
	template<class Pointer>
	static NodeT& get(Pointer& _obj) noexcept { return *_obj; }
	static NodeT& get(NodeT&  _obj) noexcept { return  _obj; }
	NodeT*							mParent			= nullptr;
	mutable bool					mSort			= false;
	mutable bool					mListCompaction	= false;
	bool							mEnable			= true;
	int								mPriority		= 0;
	mutable std::list<element>		c;					// children. ISO/IEC 14882:2003の[23.2.3.1]
};


template<class NodeT>
hierarchy_component<NodeT>::~hierarchy_component()
{
#if	0
	visit_children([this](auto* _child) -> bool
	{
		remove_child(_child);
		return	false;
	}, false);
#endif
}



template<class NodeT>
auto hierarchy_component<NodeT>::add_child(NodeT* _child) -> NodeT*
{
	hierarchy_component*	chld		= static_cast<hierarchy_component*>(_child);
	c.emplace(c.end(), element(_child));
	chld->mParent	= owner();
	chld->on_parent(owner());
	on_child(_child, on_child_event::add);
	schedule_compaction();
	return	_child;
}


template<class NodeT>
auto hierarchy_component<NodeT>::insert_child(NodeT* _pos, NodeT* _child) -> NodeT*
{
	hierarchy_component*	chld		= static_cast<hierarchy_component*>(_child);
	c.insert(std::find(c.begin(), c.end(), _pos), element(_child));
	chld->mParent	= owner();
	chld->on_parent(owner());
	on_child(_child, on_child_event::add);
	schedule_compaction();
	return	_child;
}


template<class NodeT>
bool hierarchy_component<NodeT>::remove_child(NodeT* _child)
{
	for (auto& child : c)
	{
		if ((child.node == _child) && (child.node->mParent == this))
		{
			ref<NodeT>				obj(child.node);
			hierarchy_component*	c(static_cast<hierarchy_component*>(child.node));
			schedule_compaction();
			on_child(child.node, on_child_event::remove);
			c->on_parent(nullptr);
			c->mParent	= nullptr;
			return	true;
		}
	}
	return	false;
}


template<class NodeT>
bool hierarchy_component<NodeT>::remove_from_parent()
{
	if (!parent())
	{
		return	false;
	}
	return	parent()->remove_child(static_cast<NodeT*>(this));
}



template<class NodeT>
NodeT* hierarchy_component<NodeT>::child(std::size_t _index) const noexcept
{
	auto	it	= c.begin();
	while (_index > 0 && it != c.end())
	{
		--_index;
		++it;
	}
	return	it->get();
}


template<class NodeT>
template<class FuncT>
void hierarchy_component<NodeT>::visit_children(FuncT&& _func, bool _enableOnly)
{
	for (auto child : c)
	{
		if (is_enable(child) || !_enableOnly)
		{
			if (_func(&get(*child.node))) { break; }
		}
	}
}


template<class NodeT>
template<class FuncT>
void hierarchy_component<NodeT>::visit_children(FuncT&& _func, bool _enableOnly) const
{
	for (auto child : c)
	{
		if (is_enable(child) || !_enableOnly)
		{
			if (_func(&get(*child.node))) { break; }
		}
	}
}


template<class NodeT>
template<class FuncT>
void hierarchy_component<NodeT>::visit_children_r(FuncT&& _func)
{
	for (auto it = c.rbegin(); it != c.rend(); ++it)
	{
		if (is_enable(*it))
		{
			if (_func(&get(it->node))) { break; }
		}
	}
}


template<class NodeT>
template<class FuncT>
void hierarchy_component<NodeT>::visit_children_r(FuncT&& _func) const
{
	for (auto it = c.rbegin(); it != c.rend(); ++it)
	{
		if (is_enable(*it))
		{
			if (_func(&get(it->node))) { break; }
		}
	}
}


} AMTRS_NAMESPACE_END
#endif

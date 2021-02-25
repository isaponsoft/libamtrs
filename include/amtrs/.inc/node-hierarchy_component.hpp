/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__component__hierarchy_componenet__hpp
#define	__libamtrs__component__hierarchy_componenet__hpp
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
	//! insert_child(nullptr, _child); と同等です。
	// ----------------------------------------------------------
	template<class Child> Child* add_child(Child* _child) { static_assert(std::is_convertible<Child*, node_type*>::value); add_child(ref<NodeT>(_child)); return _child; }

	template<class Child, class Option> Child* add_child(Child* _child, Option _option);

	// ==========================================================
	//! 子を追加します。
	// ----------------------------------------------------------
	//! insert_child(nullptr, _child); と同等です。
	// ----------------------------------------------------------
	template<class Child> Child* add_child(ref<Child> _child) { insert_child(nullptr, _child); return _child; }

	// ==========================================================
	//! 子を入れ替えます。
	// ----------------------------------------------------------
	//! 必要に応じて remove_from_parent(), inser_child() を
	//! 呼び出します。ただし、_n1, _n2 のどちらも子ノードとして
	//! 存在しない場合は何もせず false を返します。
	// ----------------------------------------------------------
	bool swap_child(NodeT* _n1, NodeT* _n2);

	// ==========================================================
	//! 指定するノードを自身の位置と入れ替えます。
	// ----------------------------------------------------------
	//! ただし、自身が親ノードを持っていない場合は何もしません。
	// ----------------------------------------------------------
	void swap_from_parent(NodeT* _new);


	// ==========================================================
	//! 子を指定した位置に追加します。
	// ----------------------------------------------------------
	//! _pos で指定したノードが見つからない場合は最後尾に追加します。
	// ----------------------------------------------------------
	virtual auto insert_child(NodeT* _pos, ref<NodeT> _child) -> NodeT*;

	// ==========================================================
	//! 子を削除します。
	// ----------------------------------------------------------
	//! オブジェクトとKeyTを比較し一致するオブジェクトを削除します。
	// ----------------------------------------------------------
	virtual bool remove_child(NodeT* _child);

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
	template<class FuncT> void visit_children_r(FuncT&& _func, bool _enableOnly = true);
	template<class FuncT> void visit_children_r(FuncT&& _func, bool _enableOnly = true) const;


	// ==========================================================
	//! 子の数を取得します。
	// ----------------------------------------------------------
	std::size_t children_count() const noexcept { return mChildren.size(); }


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
		if (mPriority == _priotity)
		{
			return;
		}
		mPriority	= _priotity;
		if (mParent)
		{
			mParent->mSort				= true;
			mParent->mListCompaction	= true;
			mParent->schedule_compaction();
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

	void set_enable(bool _enable)
	{
		if (mEnable != _enable)
		{
			mEnable = _enable;
			if (!mScheduling)
			{
				mScheduling	= true;
				static_cast<NodeT*>(this)->schedule_once([this]()
				{
					mScheduling	= false;
					on_change_enable(mEnable);
				});
			}
		}
	}


protected:
	enum class on_child_event
	{
		add,
		remove
	};

	// ==========================================================
	//! 子ノードの順序が入れ替わった際に呼ばれる
	// ----------------------------------------------------------
	virtual void on_change_children_order()
	{}


	// ==========================================================
	//! 子ノードが変更されたときに呼ばれます。
	// ----------------------------------------------------------
	//! 引数には変更のあった子ノードが渡されます。
	// ----------------------------------------------------------
	virtual void on_child(node_type* _child, on_child_event _event)
	{}


	// ==========================================================
	//! 有効状態が変化したときに呼ばれます。
	// ----------------------------------------------------------
	virtual void on_change_enable(bool _enable)
	{
		static_cast<NodeT*>(this)->visit_children([_enable](auto* c) -> bool
		{
			if (c->mEnable != _enable)
			{
				c->on_change_enable(_enable);
			}
			return	false;
		});
	}



	bool is_enable(NodeT* _obj) const noexcept
	{
		return	_obj->is_enable() && _obj->mParent == this;
	}


	void schedule_compaction()
	{
		if (!mListCompaction)
		{
			mListCompaction	= true;
			static_cast<NodeT*>(this)->schedule_once([this](auto)
			{
				mListCompaction	= false;
				mChildren.remove_if([](auto c) { return c.remove; });
				mChildren.sort([](auto& a, auto& b) { return a.node->mPriority < b.node->mPriority; });
				on_change_children_order();
			});
		}
	}

private:
	struct	holder_type
	{
		holder_type() = default;
		holder_type(const holder_type&) = default;
		holder_type(holder_type&&) = default;
		holder_type(NodeT* _node)
			: node(_node)
		{}
		void swap(holder_type& _b) { std::swap(node, _b.node); }
		bool operator == (NodeT* _node) const noexcept { return node == _node; }
		bool operator != (NodeT* _node) const noexcept { return !(*this == _node); }
		ref<NodeT>	node;
		bool		remove	= false;
	};

	bool is_enable(const holder_type& _child) const noexcept
	{
		return (!_child.node.empty()) && is_enable(_child.node);
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
	mutable bool					mScheduling		= false;
	bool							mEnable			= true;
	int								mPriority		= 0;
	mutable std::list<element>		mChildren;
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
template<class Child, class Option>
auto hierarchy_component<NodeT>::add_child(Child* _child, Option _option) -> Child*
{
	static_assert(std::is_convertible<Child*, node_type*>::value);
	if constexpr (std::is_same<Option, typename NodeT::use_scheduler>::value)
	{
		node_type*		thiz(static_cast<node_type*>(this));
		ref<node_type>	cld(_child);
		thiz->schedule_once([=]() { thiz->add_child(cld); });
		return	_child;
	}
}


template<class NodeT>
bool hierarchy_component<NodeT>::swap_child(NodeT* _n1, NodeT* _n2)
{
	// どちらも子ノードでない場合はエラー
	if (_n1->parent() != this && _n2->parent() != this)
	{
		return	false;
	}

	// どちらも子ノードの場合は位置を入れ替える
	if ((_n1->parent() = this) && (_n2->parent() = this))
	{
		std::swap(*mChildren.find(_n1), *mChildren.find(_n2));
		return	true;
	}

	// 子ノードを入れ替える
	ref<NodeT>	tc;
	ref<NodeT>	oc;
	ref<NodeT>	op;
	if (_n1->parent() == this)
	{
		tc	= _n1;
		oc	= _n2;
	}
	else
	{
		tc	= _n2;
		oc	= _n1;
	}
	op	= oc->parent();

	tc->remove_from_parent();
	oc->remove_from_parent();
	this->add_child(oc);
	if (op)
	{
		op->add_child(tc);
	}
	return	true;
}


template<class NodeT>
void hierarchy_component<NodeT>::swap_from_parent(NodeT* _new)
{
	if (this->parent())
	{
		this->parent()->swap_child(this, _new);
	}
}


template<class NodeT>
auto hierarchy_component<NodeT>::insert_child(NodeT* _pos, ref<NodeT> _child) -> NodeT*
{
	NodeT*					c		= _child.get();
	hierarchy_component*	chld	= static_cast<hierarchy_component*>(c);
	mChildren.insert(std::find(mChildren.begin(), mChildren.end(), _pos), element(std::move(_child)));
	chld->mParent	= owner();
	on_child(c, on_child_event::add);
	schedule_compaction();
	return	c;
}


template<class NodeT>
bool hierarchy_component<NodeT>::remove_child(NodeT* _child)
{
	for (auto& child : mChildren)
	{
		if ((child.node == _child) && (!child.remove))
		{
			child.remove	= true;
			hierarchy_component*	chc(static_cast<hierarchy_component*>(_child));
			schedule_compaction();
			on_child(child.node, on_child_event::remove);
			chc->mParent	= nullptr;
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
	auto	it	= mChildren.begin();
	while (_index > 0 && it != mChildren.end())
	{
		--_index;
		++it;
	}
	return	it->node.get();
}


template<class NodeT>
template<class FuncT>
void hierarchy_component<NodeT>::visit_children(FuncT&& _func, bool _enableOnly)
{
	for (auto child : mChildren)
	{
		if (child.node->mParent == this && (child.node->is_enable() || !_enableOnly))
		{
			if (_func(&get(*child.node))) { break; }
		}
	}
}


template<class NodeT>
template<class FuncT>
void hierarchy_component<NodeT>::visit_children(FuncT&& _func, bool _enableOnly) const
{
	for (auto child : mChildren)
	{
		if (child.node->mParent == this && (child.node->is_enable() || !_enableOnly))
		{
			if (_func(&get(*child.node))) { break; }
		}
	}
}


template<class NodeT>
template<class FuncT>
void hierarchy_component<NodeT>::visit_children_r(FuncT&& _func, bool _enableOnly)
{
	for (auto it = mChildren.rbegin(); it != mChildren.rend(); ++it)
	{
		if ((*it).node->mParent == this && ((*it).node->is_enable() || !_enableOnly))
		{
			if (_func(&get((*it).node))) { break; }
		}
	}
}


template<class NodeT>
template<class FuncT>
void hierarchy_component<NodeT>::visit_children_r(FuncT&& _func, bool _enableOnly) const
{
	for (auto it = mChildren.rbegin(); it != mChildren.rend(); ++it)
	{
		if ((*it).node->mParent == this && ((*it).node->is_enable() || !_enableOnly))
		{
			if (_func(&get((*it).node))) { break; }
		}
	}
}


} AMTRS_NAMESPACE_END
#endif

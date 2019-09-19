#ifndef	__libamtrs__node__pager_horizontal_node__hpp
#define	__libamtrs__node__pager_horizontal_node__hpp
#include "node.hpp"
AMTRS_NAMESPACE_BEGIN



// ****************************************************************************
//! ページャ（水平）
// ----------------------------------------------------------------------------
//! ページャは（スクロール中を除き）ひとつ子ノードを表示します。
// ----------------------------------------------------------------------------
template<class NodeT>
class	pager_horizontal_node
		: public children_transform_node<NodeT>
{
	using	_base_type	= children_transform_node<NodeT>;
public:
	using	node_type	= typename _base_type::node_type;
	using	index_type	= std::size_t;

	static constexpr index_type	nopage	= static_cast<index_type>(-1);

	// ========================================================================
	//! 持っているページの数を返します。
	// ------------------------------------------------------------------------
	std::size_t count() const noexcept { return _base_type::children_count(); }

	// ========================================================================
	//! ページを持っていない場合は true を返します。
	// ------------------------------------------------------------------------
	bool empty() const noexcept { return count() == 0; }

	// ========================================================================
	//! 最初のページを表示している場合は true を返します。
	// ------------------------------------------------------------------------
	//! page() == 0 と同じです。
	// ------------------------------------------------------------------------
	bool is_first() const noexcept { return page() == 0; }

	// ========================================================================
	//! 最後のページを表示している場合は true を返します。
	// ------------------------------------------------------------------------
	//! page() == (count()-1) と同じです。
	// ------------------------------------------------------------------------
	bool is_last() const noexcept { return page() == (count()-1); }

	
	index_type page() const noexcept { return mPage; }

	bool set_page(index_type _page)
	{
		if (_set_page(_page))
		{
			return	true;
		}
		return	false;
	}

	template<class Callback>
	bool set_page(index_type _page, Callback&& _callback)
	{
		auto*	act	= _set_page(_page);
		if (act)
		{
			act->set_finish_callback(std::forward<Callback>(_callback));
			return	true;
		}
		return	false;
	}

	bool back()
	{
		return	set_page(page()-1);
	}

	template<class Callback>
	bool back(Callback&& _callback)
	{
		return	set_page(page()-1, std::forward<Callback>(_callback));
	}

	bool next()
	{
		return	set_page(page()+1);
	}

	template<class Callback>
	bool next(Callback&& _callback)
	{
		return	set_page(page()+1, std::forward<Callback>(_callback));
	}


protected:

	virtual void on_child(node_type* _node, typename _base_type::on_child_event _event) override
	{
		_base_type::on_child(_node, _event);
		reposition();
	}

	virtual void on_coordinate_modify() noexcept override
	{
		_base_type::on_coordinate_modify();
		const_cast<pager_horizontal_node*>(this)->reposition();
	}


private:
	void reposition()
	{
		float		pos		= 0;
		index_type	page	= 0;
		index_type	current	= mPage == nopage ? 0 : mPage;
		this->visit_children([this, &pos, &page, current](auto c) -> bool
		{
			c->set_pivot({0, 0, 0});
			c->set_position(vec3<float>{pos, 0, 0});
			if (current != page)
			{
				c->set_scale({0,1,1});
			}
			pos += this->size().width;
			++page;
			return	false;
		});
	}

	static constexpr float	change_duration	= 0.12f;


	typename _base_type::action_type* _set_page(index_type _page)
	{
		typename _base_type::action_type*	action	= nullptr;
		if (mPage != _page && _page < _base_type::children_count())
		{
			auto	pageSize	= this->size().width;
			if (mPage < _base_type::children_count())
			{
				this->child(mPage)->run_action(
					new scale_to_action({0, 1, 1}, change_duration)
				);
			}

			this->child(_page)->set_scale({1,1,1});
			action	= new sequence_action({
				new move_by_action(this->children_transform(), {-float(pageSize * _page), 0, 0}, change_duration)
			});
			this->run_action(action);

			mPage	= _page;
		}
		return	action;
	}

private:
	index_type	mPage			= nopage;
};


AMTRS_NAMESPACE_END
#endif

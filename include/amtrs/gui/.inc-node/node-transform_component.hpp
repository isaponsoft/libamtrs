/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__component__transform_component__hpp
#define	__libamtrs__component__transform_component__hpp
AMTRS_NAMESPACE_BEGIN namespace component {

// ==================================================================
//! component_object に対して３Ｄ座標系を持たせます。
// ------------------------------------------------------------------
//! オブジェクトが hierarchy component を持っている必要があります。
// ------------------------------------------------------------------
template<class NodeT, class T, class Traits>
struct	transform_component
		: public transform<T>
{
	using	this_type		= transform_component<NodeT, T, Traits>;
public:
	using	node_type		= NodeT;

	using	transform_type	= transform<T>;
	using	value_type		= typename transform_type::value_type;
	using	position_type	= typename transform_type::position_type;
	using	scale_type		= typename transform_type::scale_type;
	using	rotate_type		= typename transform_type::rotate_type;
	using	matrix_type		= typename transform_type::matrix_type;
	using	size_type		= typename transform_type::size_type;
	using	box_type		= typename transform_type::box_type;
	using	bounds_type		= typename transform_type::box_type;


	node_type* set_position(position_type _pos)
	{
		transform_type::set_position(_pos);
		return	static_cast<node_type*>(this);
	}


	node_type* set_scale(scale_type _scale)
	{
		if (transform_type::set_scale(_scale) && mParentTransform)
		{
			mParentTransform->on_children_size_change(this);
		}
		return	static_cast<node_type*>(this);
	}


	node_type* set_size(size_type _size)
	{
		if (transform_type::set_size(_size) && mParentTransform)
		{
			mParentTransform->on_children_size_change(this);
		}
		return	static_cast<node_type*>(this);
	}

	node_type* set_width(value_type _size)
	{
		if (transform_type::set_width(_size) && mParentTransform)
		{
			mParentTransform->on_children_size_change(this);
		}
		return	static_cast<node_type*>(this);
	}

	node_type* set_height(value_type _size)
	{
		if (transform_type::set_height(_size) && mParentTransform)
		{
			mParentTransform->on_children_size_change(this);
		}
		return	static_cast<node_type*>(this);
	}

	node_type* set_pivot(position_type _pos)
	{
		transform_type::set_pivot(_pos);
		return	static_cast<node_type*>(this);
	}

	node_type* set_rotate(rotate_type _rotate) noexcept
	{
		transform_type::set_rotate(_rotate);
		return	static_cast<node_type*>(this);
	}


	bounds_type bounds() const noexcept
	{
		return	bounds_type(
					this->position() - (position_type)(this->size() * this->pivot()),
					this->size()
				);
	}


	template<class FirstT, class LastT>
	box_type bounds(FirstT _first, LastT _last) const noexcept
	{
		if (_first == _last)
		{
			return	{{0,0,0}, {0,0,0}};
		}

		box_type	bounds	= {(*_first)->position(), (*_first)->size()};
		for (auto it = _first; it != _last; ++it)
		{
			auto		br	= box_type((*it)->position() - (position_type((*it)->size()) * (*it)->pivot()), (*it)->size() * (*it)->scale());
			if (bounds.left() > br.left())
			{
				bounds.x	= br.x;
			}
			if (bounds.top() > br.top())
			{
				bounds.y	= br.y;
			}
			if (bounds.front() > br.front())
			{
				bounds.z	= br.z;
			}
			if (bounds.left() > br.left())
			{
				bounds.x	= br.x;
			}
			if (bounds.right() < br.right())
			{
				bounds.width	=  br.right() - bounds.left();
			}
			if (bounds.bottom() < br.bottom())
			{
				bounds.height	=  br.bottom() - bounds.top();
			}
			if (bounds.back() < br.back())
			{
				bounds.depth	=  br.back() - bounds.front();
			}
		}
		return	bounds;
	}


	// ========================================================================
	//! 子ノードを覆うバウンディングボックスを計算します。
	// ------------------------------------------------------------------------
	box_type children_bounds() const noexcept
	{
		auto*	node	= static_cast<node_type const*>(this);
		if (node->children_count() == 0)
		{
			return	{{0,0,0}, {0,0,0}};
		}


		box_type	bounds	= {node->child(0)->position(), node->child(0)->size()};
		node->visit_children([&](auto c) -> bool
		{
			auto		br	= box_type(c->position() - (position_type(c->size()) * c->pivot()), c->size() * c->scale());
			if (bounds.left() > br.left())
			{
				bounds.x	= br.x;
			}
			if (bounds.top() > br.top())
			{
				bounds.y	= br.y;
			}
			if (bounds.front() > br.front())
			{
				bounds.z	= br.z;
			}
			if (bounds.left() > br.left())
			{
				bounds.x	= br.x;
			}
			if (bounds.right() < br.right())
			{
				bounds.width	=  br.right() - bounds.left();
			}
			if (bounds.bottom() < br.bottom())
			{
				bounds.height	=  br.bottom() - bounds.top();
			}
			if (bounds.back() < br.back())
			{
				bounds.depth	=  br.back() - bounds.front();
			}
			return	false;
		}, false);
		return	bounds;
	}




	// ========================================================================
	//! Tをベースに持つ子ノードを覆うバウンディングボックスを計算します。
	// ------------------------------------------------------------------------
	template<class DerivedT>
	box_type children_bounds() const noexcept
	{
		auto*	node	= static_cast<node_type const*>(this);
		if (node->children_count() == 0)
		{
			return	{{0,0,0}, {0,0,0}};
		}


		box_type	bounds	= {node->child(0)->position(), node->child(0)->size()};
		node->visit_children([&](auto c) -> bool
		{
			if (!dynamic_cast<DerivedT const*>(c))
			{
				return	false;
			}

			auto		br	= box_type(c->position() - (position_type(c->size()) * c->pivot()), c->size() * c->scale());
			if (bounds.left() > br.left())
			{
				bounds.x	= br.x;
			}
			if (bounds.top() > br.top())
			{
				bounds.y	= br.y;
			}
			if (bounds.front() > br.front())
			{
				bounds.z	= br.z;
			}
			if (bounds.left() > br.left())
			{
				bounds.x	= br.x;
			}
			if (bounds.right() < br.right())
			{
				bounds.width	=  br.right() - bounds.left();
			}
			if (bounds.bottom() < br.bottom())
			{
				bounds.height	=  br.bottom() - bounds.top();
			}
			if (bounds.back() < br.back())
			{
				bounds.depth	=  br.back() - bounds.front();
			}
			return	false;
		}, false);
		return	bounds;
	}

	// ========================================================================
	//! 子ノードを覆うサイズに自信のサイズを設定します。
	// ------------------------------------------------------------------------
	node_type* set_size_by_children()
	{
		this->set_size(this->children_bounds().size());
		return	static_cast<NodeT*>(this);
	}


	using	transform_type::pivot;
	using	transform_type::position;
	using	transform_type::scale;
	using	transform_type::size;
	using	transform_type::on_size_change;

	// ==============================================================
	//! 子ノードを格納することができるローカルエリアのサイズを返す。
	// --------------------------------------------------------------
	virtual box_type children_area() const { return box_type({0,0,0}, this->size()); }

	// ==============================================================
	//! 子座標系を持っている場合は子座標系を返します。
	// --------------------------------------------------------------
	//! 子ノードだけを変換（スクロールさせたり）するのに使用できます。
	//! nullptrの場合は子座標を持ちません。
	// --------------------------------------------------------------
	virtual matrix_type children_matrix() const
	{
		static constexpr matrix_type	one(1,0,0,0,  0,1,0,0,  0,0,1,0,  0,0,0,1);
		return	one;
	}

	// ==============================================================
	//! 自身のマトリックスを返します。
	// --------------------------------------------------------------
	matrix_type const& local_matrix() const
	{
		return	transform_type::to_matrix();
	}

	// ==============================================================
	//! ワールド座標系を取得します。
	// --------------------------------------------------------------
	const matrix_type& world_matrix() const
	{
		if (mWorldModified)
		{
			mWorldModified	= false;
			if (mParentTransform)
			{
				mWorldMatirx = local_matrix() * mParentTransform->children_matrix() * mParentTransform->world_matrix();
			}
			else
			{
				mWorldMatirx = local_matrix();
			}
		}
		return	mWorldMatirx;
	}

	virtual void on_size_change(size_type& _size) override
	{
		transform_type::on_size_change(_size);
		if (mParentTransform)
		{
			mParentTransform->on_children_size_change(this);
		}
	}


	virtual void on_coordinate_modify() override
	{
		transform_type::on_coordinate_modify();
		if (mParentTransform)
		{
			mParentTransform->on_children_transform_change(this);
		}
		mWorldModified	= true;
		static_cast<NodeT*>(this)->visit_children([](auto* c) -> bool
		{
			static_cast<this_type*>(c)->on_coordinate_modify();
			return	false;
		}, false);
		static_cast<NodeT*>(this)->rerender_request();
	}

protected:
	transform_component(){}

	void set_parent_transform(transform_component* _parent)
	{
		mParentTransform	= _parent;
		on_coordinate_modify();
	}


	virtual void on_children_transform_change(transform_component*)
	{}

	virtual void on_children_size_change(transform_component*)
	{}



private:
	mutable bool			mWorldModified		= true;
	mutable matrix_type		mWorldMatirx;
	transform_component*	mParentTransform	= nullptr;
};

} AMTRS_NAMESPACE_END
#endif

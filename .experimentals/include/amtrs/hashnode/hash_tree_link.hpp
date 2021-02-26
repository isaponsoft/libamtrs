/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__hashnode__hash_node_tree_link__hpp
#define	__libamtrs__hashnode__hash_node_tree_link__hpp
#include "../hash/hash_stream.hpp"
#include "def.hpp"
#include "hash_node_traits.hpp"
AMTRS_NAMESPACE_BEGIN





template<class T, class Tree, class Incrementor>
class	__hash_tree_list_iterator;



//! リンクリストを持ち、イテレータが使用可能な hash_tree
template<class T, class Traits = hash_node_tree_traits<T>>
class	hash_tree_link
{
	using	__value_type	= typename Traits::value_type;

	struct	__link
	{
		__link*			prev	= nullptr;
		__link*			next	= nullptr;
	};

	struct	__item : __link
	{
		template<class S>
		__item(S _s) : val(_s)
		{}
		__value_type	val;
	};

	struct	__traits
	{
		static constexpr int	node_size	= Traits::node_size;
		using	value_type		= __item;
		using	key_type		= typename Traits::key_type;
		using	mapped_type		= typename Traits::mapped_type;
		static key_type const& key(__item const& _v) noexcept
		{
			return	Traits::key(_v.val);
		}
	};

	struct	__inc
	{
		template<class Item> auto* inc(Item* _v) const noexcept { return _v->next; }
		template<class Item> auto* dec(Item* _v) const noexcept { return _v->prev; }
	};

	struct	__dec
	{
		template<class Item> auto* inc(Item* _v) const noexcept { return _v->prev; }
		template<class Item> auto* dec(Item* _v) const noexcept { return _v->next; }
	};

	using	__tree_base	= __hash_tree_base<T, __traits>;

public:
	using	value_type				= typename Traits::value_type;
	using	size_type				= size_t;
	using	iterator				= __hash_tree_list_iterator<value_type, hash_tree_link, __inc>;
	using	const_iterator			= __hash_tree_list_iterator<typename std::add_const<value_type>::type, hash_tree_link const, __inc>;
	using	reverse_iterator		= __hash_tree_list_iterator<value_type, hash_tree_link, __dec>;
	using	const_reverse_iterator	= __hash_tree_list_iterator<typename std::add_const<value_type>::type, hash_tree_link const, __dec>;

	hash_tree_link() = default;

	hash_tree_link(const hash_tree_link& _r)
		: hash_tree_link(_r.cbegin(), _r.cend())
	{}

	hash_tree_link(hash_tree_link&& _r)
		: mBase(std::move(_r.mBase))
		, mTerm(_r.mTerm)
	{
		if (mTerm.next)
		{
			mTerm.next->prev	= &mTerm;
		}
		if (mTerm.prev)
		{
			mTerm.prev->next	= &mTerm;
		}
		_r.mTerm.prev	= nullptr;
		_r.mTerm.next	= nullptr;
	}

	template<class FirstIt, class LastIt>
	hash_tree_link(FirstIt _first, LastIt _last)
	{
		for (; _first != _last; ++_first)
		{
			add(*_first);
		}
	}

	~hash_tree_link()
	{
		clear();
	}

	void clear()
	{
		while (mBase.root())
		{
			del(__traits::key(mBase.root()->value));
		}
	}

	size_type size() const noexcept { return mBase.size(); }
	bool empty() const noexcept { return mBase.empty(); }

	template<class V>
	bool add(V&& _val)
	{
		__item*	target	= mBase.add(__item(_val));
		if (target)
		{
			// front() がないなら front() を更新
			if (!mTerm.next)
			{
				mTerm.next			= target;
				target->next		= &mTerm;
			}

			// back()の更新
			if (mTerm.prev)
			{
				mTerm.prev->next	= target;
				target->prev		= mTerm.prev;
			}
			mTerm.prev		= target;
			target->next	= &mTerm;
			return	true;
		}
		return	false;
	}


	template<class Key>
	bool del(Key&& _key)
	{
		auto*	node	= mBase.remove(std::forward<Key>(_key));
		if (node == nullptr)
		{
			return	false;
		}
		auto	target	= &node->value;

		if (mTerm.next == target)
		{
			// 対象が先頭なら先頭を変更する
			mTerm.next	= target->next != &mTerm
						? target->next
						: nullptr;
		}
		else
		{
			// 前方のノードのリンクを変更する
			target->prev->next	= target->next;
		}

		if (target->prev != target)
		{
			// 対象が最後尾なら最後尾を変更する
			mTerm.prev	= target->prev != &mTerm
						? target->prev
						: nullptr;
		}
		else
		{
			// 後方のノードのリンクを変更する
			target->next->prev	= target->prev;
		}

		delete	node;
		return	true;
	}

	
	template<class Key>
	value_type* get(Key&& _key)
	{
		__item*	target	= mBase.get(std::forward<Key>(_key));
		return	target
				? &target->val
				: nullptr;
	}

	iterator begin() noexcept { return iterator(mTerm.next ? static_cast<__item*>(mTerm.next) : static_cast<__item*>(&mTerm)); }
	const_iterator begin() const noexcept { return const_iterator(mTerm.next ? static_cast<__item const*>(mTerm.next) : static_cast<__item const*>(&mTerm)); }
	const_iterator cbegin() const noexcept { return const_iterator(mTerm.next ? static_cast<__item const*>(mTerm.next) : static_cast<__item const*>(&mTerm)); }
	iterator end() noexcept { return iterator(static_cast<__item*>(&mTerm)); }
	const_iterator end() const noexcept { return const_iterator(static_cast<__item const*>(&mTerm)); }
	const_iterator cend() const noexcept { return const_iterator(static_cast<__item const*>(&mTerm)); }

	reverse_iterator rbegin() noexcept { return reverse_iterator(mTerm.prev ? static_cast<__item*>(mTerm.prev) : static_cast<__item*>(&mTerm)); }
	const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(mTerm.prev ? static_cast<__item const*>(mTerm.prev) : static_cast<__item const*>(&mTerm)); }
	const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(mTerm.prev ? static_cast<__item const*>(mTerm.prev) : static_cast<__item const*>(&mTerm)); }
	reverse_iterator rend() noexcept { return reverse_iterator(static_cast<__item*>(&mTerm)); }
	const_reverse_iterator rend() const noexcept { return const_reverse_iterator(static_cast<__item const*>(&mTerm)); }
	const_reverse_iterator rcend() const noexcept { return const_reverse_iterator(static_cast<__item const*>(&mTerm)); }


protected:
	__tree_base	mBase;
	__link		mTerm;

	template<class, class, class>
	friend	class	__hash_tree_list_iterator;

};


template<class T, class List, class Incrementor>
class	__hash_tree_list_iterator
{
	using	__list_type	= List;
	using	__item_type	= typename std::conditional<
							std::is_const<List>::value
							, typename std::add_const<typename  __list_type::__item>::type
							, typename __list_type::__item
						>::type;
public:
	using	difference_type		= ptrdiff_t;
	using	value_type			= T;
	using	pointer				= value_type*;
	using	reference			= value_type&;
	using	iterator_category	= std::bidirectional_iterator_tag;


	__hash_tree_list_iterator() = default;
	__hash_tree_list_iterator(const __hash_tree_list_iterator&) = default;
	__hash_tree_list_iterator& operator = (const __hash_tree_list_iterator&) = default;

	bool operator == (__hash_tree_list_iterator const& _r) const noexcept { return mItem == _r.mItem; }
	bool operator != (__hash_tree_list_iterator const& _r) const noexcept { return mItem != _r.mItem; }

	value_type* operator -> () const noexcept { return &mItem->val; }
	value_type& operator *  () const noexcept { return  mItem->val; }


	__hash_tree_list_iterator& operator ++ () noexcept
	{
		mItem	= static_cast<__item_type*>(Incrementor{}.inc(mItem));
		return	*this;
	}

	__hash_tree_list_iterator operator ++ (int) noexcept
	{
		__hash_tree_list_iterator	retval(*this);
		mItem	= static_cast<__item_type*>(Incrementor{}.inc(mItem));
		return	retval;
	}


	__hash_tree_list_iterator& operator -- () noexcept
	{
		mItem	= static_cast<__item_type*>(Incrementor{}.dec(mItem));
		return	*this;
	}

	__hash_tree_list_iterator operator -- (int) noexcept
	{
		__hash_tree_list_iterator	retval(*this);
		mItem	= static_cast<__item_type*>(Incrementor{}.dec(mItem));
		return	retval;
	}

protected:
	__hash_tree_list_iterator(__item_type* _item)
		: mItem(_item)
	{}

	__item_type*	mItem;


	
	template<class, class>
	friend	class	hash_tree_link;
};




AMTRS_NAMESPACE_END
#endif

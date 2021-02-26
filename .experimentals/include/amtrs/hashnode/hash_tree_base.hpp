/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__hashnode__hash_node_tree_base__hpp
#define	__libamtrs__hashnode__hash_node_tree_base__hpp
#include "../hash/hash_stream.hpp"
#include "def.hpp"
#include "hash_node_traits.hpp"
AMTRS_NAMESPACE_BEGIN




template<class T, class Traits = hash_node_tree_traits<T>>
class	__hash_tree_base
{
public:
	using	traits_type					= Traits;
	using	value_type					= typename traits_type::value_type;
	using	size_type					= size_t;

	static constexpr int	node_size	= traits_type::node_size;

	struct	node
	{
		value_type	value;
		node*		children[node_size] = {0};


		template<class V>
		node(V&& _val)
			: value(_val)
		{}

		node** child()
		{
			for (int i = 0; i < node_size; ++i)
				if (children[i])
					return	&children[i];
			return	nullptr;
		}
	};


	__hash_tree_base() = default;
	__hash_tree_base(const __hash_tree_base&) = default;

	__hash_tree_base(__hash_tree_base&& _r)
		: mRoot(_r.mRoot)
		, mSize(_r.mSize)
	{
		_r.mRoot	= nullptr;
		_r.mSize	= 0;
	}

	template<class Key>
	value_type* get(Key&& _key)
	{
		node**	target	= _follow(_key);
		return	*target
				? &(*target)->value
				: nullptr;
	}


	template<class V>
	value_type* add(V&& _val)
	{
		node**	target = _follow(traits_type::key(_val));
		if (!*target)
		{
			*target	= new node(_val);
			++mSize;
			return	&(*target)->value;
		}
		return	nullptr;
	}


	template<class Key>
	node* remove(Key&& _key)
	{
		node**	target	= _follow(_key);
		if (*target == nullptr)
		{
			return	nullptr;
		}

		node** tail = (*target)->child();
		for (auto n = tail; n; n = (*n)->child())
		{
			tail = n;
		}

		node*	retval	= *target;
		if (tail)
		{
			*target = *tail;
			*tail	= nullptr;
			for (int i = 0; i < node_size; ++i)
			{
				(*target)->children[i] = (retval)->children[i];
			}
		}
		else
		{
			*target = nullptr;
		}
		--mSize;

		return	retval;
	}

	node* root() { return mRoot; }

	bool empty() const noexcept { return size() == 0; }
	size_type size() const noexcept { return mSize; }


	template<class Key>
	node** _follow(Key&& _key)
	{
		auto	h		= hash_stream<decltype(std::begin(_key)), decltype(std::end(_key))>(std::begin(_key), std::end(_key));
		node**	ground	= &mRoot;
		while (*ground)
		{
			if (traits_type::key((*ground)->value) == _key)
			{
				return	ground;
			}
			ground	= &(*ground)->children[((unsigned)h()) % node_size];
		}
		return	ground;
	}


	node*		mRoot	= nullptr;
	size_type	mSize	= 0;
};


AMTRS_NAMESPACE_END
#endif

/* Copyright (c) 2022, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__memory__hash_map__hpp
#define	__libamtrs__memory__hash_map__hpp
#include <algorithm>
#include <iterator>
#include <utility>
#include "../crypto/hash-csx.hpp"
AMTRS_NAMESPACE_BEGIN

template<class Map, class Ptr, bool Reverse>
class	hash_map_iterator;


// ==================================================================
// 1. cannot have duplicate keys.
// 2. An iterator is always valid unless a key has been deleted.
// ------------------------------------------------------------------
// 1. キーは重複しません。
// 2. イテレータは hash_map から削除されない限り使用可能です。
// ------------------------------------------------------------------
template<class Key, class T, class Compare = std::equal_to<Key>, size_t TableBitSize = 2>
class	hash_map
{
protected:
	struct	node;
	static constexpr size_t		table_bit_size		= TableBitSize;
	static constexpr size_t		table_size			= 1<<table_bit_size;
	static constexpr size_t		table_index_mask	= table_size-1;
	using	hashmap_type			= typename std::remove_const<hash_map>::type;
public:
	using	key_type				= Key;
	using	mapped_type				= T;
	using	value_type				= std::pair<Key const, T>;
	using	size_type				= size_t;
	using	difference_type			= ptrdiff_t;
	using	key_compare				= Compare;
	using	pointer					= value_type*;
	using	const_pointer			= typename std::add_const<value_type>::type*;
	using	reference				= value_type&;
	using	const_reference			= typename std::add_const<value_type>::type&;
	using	iterator				= hash_map_iterator<hashmap_type,       pointer, false>;
	using	const_iterator			= hash_map_iterator<hashmap_type, const_pointer, false>;
	using	reverse_iterator		= hash_map_iterator<hashmap_type,       pointer, true>;
	using	const_reverse_iterator	= hash_map_iterator<hashmap_type, const_pointer, true>;
	using	node_type				= node;


	hash_map() = default;
	hash_map(hash_map const&) = delete;
	hash_map& operator = (hash_map const&) = delete;
	hash_map(hash_map&& _r);
	auto operator = (hash_map&&) -> hash_map&;

	template<class K>
	auto operator [] (K _key) -> mapped_type& { return try_emplace(key_type(std::forward<K>(_key)), mapped_type{}).first->second; }

	// iterators
	auto begin() noexcept			->       iterator			{ return       iterator			(const_cast<hashmap_type*>(this), mHead ? mHead : head()); }
	auto begin() const noexcept		-> const_iterator			{ return const_iterator			(const_cast<hashmap_type*>(this), mHead ? mHead : head()); }
	auto end() noexcept				->       iterator			{ return       iterator			(const_cast<hashmap_type*>(this), tail());  }
	auto end() const noexcept		-> const_iterator			{ return const_iterator			(const_cast<hashmap_type*>(this), tail());  }
	auto rbegin() noexcept			->       reverse_iterator	{ return       reverse_iterator	(const_cast<hashmap_type*>(this), mTail ? mTail : tail()); }
	auto rbegin() const noexcept	-> const_reverse_iterator	{ return const_reverse_iterator	(const_cast<hashmap_type*>(this), mTail ? mTail : tail()); }
	auto rend() noexcept			->       reverse_iterator	{ return       reverse_iterator	(const_cast<hashmap_type*>(this), head());  }
	auto rend() const noexcept		-> const_reverse_iterator	{ return const_reverse_iterator	(const_cast<hashmap_type*>(this), head());  }
	auto cbegin() const noexcept	-> const_iterator			{ return const_iterator			(const_cast<hashmap_type*>(this), mHead ? mHead : head()); }
	auto cend() const noexcept		-> const_iterator			{ return const_iterator			(const_cast<hashmap_type*>(this), tail());  }
	auto crbegin() const noexcept	-> const_reverse_iterator	{ return const_reverse_iterator	(const_cast<hashmap_type*>(this), mTail ? mTail : tail()); }
	auto crend() const noexcept		-> const_reverse_iterator	{ return const_reverse_iterator	(const_cast<hashmap_type*>(this), head());  }

	// capacity
	auto size() const noexcept -> size_type { return mSize; }
	auto empty() const noexcept -> bool { return size() == 0; }

	// modifiers
	void clear();
	void swap(hash_map& _r);

	// modifiers : insert()
	auto insert(value_type const& _value) -> std::pair<iterator, bool> { return _emplace(tail(), _value); }

	template<class P>
	auto insert(P&& _value) -> std::pair<iterator, bool> { return _emplace(tail(), std::forward<P>(_value)); }

	auto insert(value_type&& _value) -> std::pair<iterator, bool> { return _emplace(tail(), std::move(_value)); }

	auto insert(iterator _hint, value_type const& _value) -> iterator { return _emplace(_hint, _value).first; }

	auto insert(const_iterator _hint, value_type const& _value) -> iterator { return _emplace(_hint, _value).first; }

	template<class P>
	auto insert(const_iterator _hint, P&& _value) -> std::pair<iterator, bool> { return _emplace(_hint, std::forward<P>(_value)); }

	auto insert(const_iterator _hint, value_type&& _value) -> std::pair<iterator, bool> { return _emplace(_hint, std::move(_value)); }

	template<class InputIt>
	void insert(InputIt _first, InputIt _last) { for (auto i = _first; i != _last; ++i) { _emplace(tail(), *i); } }

	void insert(std::initializer_list<value_type> _ilist) { insert(_ilist.begin(), _ilist.end()); }


	// modifiers : insert_or_assign()
	template<class M>
	auto insert_or_assign(key_type const& _key, M&& _obj) -> std::pair<iterator, bool> { return try_emplace(_key, std::forward(_obj)); }

	template<class M>
	auto insert_or_assign(key_type&& _key, M&& _obj) -> std::pair<iterator, bool> { return try_emplace(std::move(_key), std::forward(_obj)); }

	template<class M>
	auto insert_or_assign(const_iterator _hint, key_type const& _key, M&& _obj) -> std::pair<iterator, bool> { return try_emplace(_hint, _key, std::forward(_obj)); }

	template<class M>
	auto insert_or_assign(const_iterator _hint, key_type&& _key, M&& _obj) -> std::pair<iterator, bool> { return try_emplace(_hint, std::move(_key), std::forward(_obj)); }


	// modifiers : emplace()
	template<class... Args> auto emplace(Args&&... _args) -> std::pair<iterator, bool> { return _emplace(tail(), value_type(std::forward<Args>(_args)...)); }


	// modifiers : emplace()
	template<class... Args> auto emplace_hint(const_iterator _hint, Args&&... _args) -> iterator { return _emplace(const_cast<node_type*>(_hint.mCur), value_type(std::forward<Args>(_args)...)).first; }


	// modifiers : try_emplace()
	template <class... Args> auto try_emplace(key_type const& _k, Args&&... _args) -> std::pair<iterator, bool>;
	template <class... Args> auto try_emplace(key_type&& _k, Args&&... _args) -> std::pair<iterator, bool>;
	template <class... Args> auto try_emplace(const_iterator _hint, key_type const& _k, Args&&... _args) -> std::pair<iterator, bool>;
	template <class... Args> auto try_emplace(const_iterator _hint, key_type&& _k, Args&&... _args) -> std::pair<iterator, bool>;


	// modifiers : erase()
	auto erase(iterator _pos) -> iterator;
	auto erase(const_iterator _pos) -> iterator { return erase(iterator(this, const_cast<node_type*>(_pos.mCur))); }
	auto erase(key_type const& _key) -> size_type;


	// lookups
	auto find(key_type const& _key) -> iterator;
	auto count(key_type const& _key) -> size_type { return find(_key) != end() ? 1 : 0; };

protected:
	static node* head() noexcept { return reinterpret_cast<node*>(reinterpret_cast<void*>(static_cast<char*>(nullptr) - 1)); }
	static node* tail() noexcept { return reinterpret_cast<node*>(reinterpret_cast<void*>(static_cast<char*>(nullptr) - 2)); }

	struct	node
	{
		node(value_type _val, size_type _hash) : value(std::move(_val)) , hash(_hash) {}
		~node() { for (auto& n :children) { if (n) { delete n; } } }
		bool empty() noexcept { return fast() ? false : true; }
		auto fast() noexcept -> node** { for (auto& c : children) { if (c) { return &c; } } return nullptr; }

		value_type	value;
		size_type	hash;
		node*		children[table_size]	= {0};
		node*		prev	= nullptr;
		node*		next	= nullptr;
	};

	template<class... Args>	auto _emplace(node_type* _pos, key_type const& _key, Args&&... _args) -> std::pair<iterator, bool>;
	template<class... Args>	auto _emplace(node_type* _pos, key_type&& _key, Args&&... _args) -> std::pair<iterator, bool>;

	auto _emplace(node_type* _pos, value_type _val) -> std::pair<iterator, bool>;


	template<class Match, class Unmatch>
	auto _routing(key_type const& _key, Match _match, Unmatch _unmatch) -> std::pair<iterator, bool>
	{
		node**	ref(mRoot);
		auto	hash_o	= std::hash<key_type>{}(_key);
		auto	hash	= hash_o;
		for (;;)
		{
			auto	idx		= hash & table_index_mask;
			auto&	cur		= ref[idx];

			// unmatch node (insert)
			if (!cur)
			{
				return	{ iterator(this, _unmatch(cur, hash_o)), true };
			}

			// match node (update)
			if (is_equals(*cur, hash_o, _key))
			{
				return	{ iterator(this, _match(cur, hash_o)), false };
			}

			// next node
			hash	= rehash(hash);
			ref		= ref[idx]->children;
		}
	}

	auto _bind(node* _pos, node* _node) -> node*
	{
		if (_pos == head())
		{
			_node->next		= mHead;
			if (mHead)	mHead->prev	= _node;
			else		mTail		= _node;
			mHead			= _node;
		}
		else if (_pos == tail())
		{
			_node->prev		= mTail;
			if (mTail)	mTail->next	= _node;
			else		mHead		= _node;
			mTail			= _node;
		}
		else
		{
			_node->prev		= _pos->prev;
			_node->next		= _pos;
			if (_node->prev)	_node->prev->next	= _node;
			else				mHead				= _node;
			_pos->prev		= _node;
		}
		++mSize;
		return	_node;
	}

	static constexpr bool is_equals(node const& _n, size_type _hash, key_type const& _key) noexcept { return (_n.hash == _hash) || key_compare{}(_n.value.first, _key); }
	static constexpr size_type rehash(size_type _hash) noexcept { return (_hash * (table_size + 3)) >> table_bit_size; }


	size_type	mSize				= 0;
	node*		mRoot[table_size]	= {0};
	node*		mHead				= nullptr;
	node*		mTail				= nullptr;

	template<class, class, bool>
	friend	class	hash_map_iterator;
};


template<class Map, class Ptr, bool Reverse>
class	hash_map_iterator
{
	using	hashmap_type	= Map;
public:
	using	value_type		= Ptr;
	using	node_type		= typename hashmap_type::node_type;
	using	pointer			= value_type;
	using	reference		= typename std::remove_pointer<value_type>::type&;

	hash_map_iterator() noexcept = default;
	hash_map_iterator(hash_map_iterator const&) noexcept = default;
	hash_map_iterator& operator = (hash_map_iterator const&) noexcept = default;
	hash_map_iterator(hashmap_type* _map, node_type* _node) noexcept
		: mMap(_map)
		, mCur(_node)
	{}

	template<class R1, bool R2>
	hash_map_iterator(hash_map_iterator<Map, R1, R2> const& _r) noexcept
		: mMap(_r.mMap)
		, mCur(_r.mCur)
	{}

	template<class R1, bool R2>
	hash_map_iterator& operator = (hash_map_iterator const& _r) noexcept
	{
		mMap	= _r,mMap;
		mCur	= _r,mCur;
		return	*this;
	}

	auto operator -> () const noexcept -> pointer { return &mCur->value; }
	auto operator *  () const noexcept -> reference { return mCur->value; }
	auto operator ++ (int) noexcept -> hash_map_iterator { hash_map_iterator r(*this); ++(*this); return r; }
	auto operator -- (int) noexcept -> hash_map_iterator { hash_map_iterator r(*this); --(*this); return r; }
	auto operator ++ () noexcept -> hash_map_iterator& { if constexpr (Reverse) prev(); else next(); return *this; }
	auto operator -- () noexcept -> hash_map_iterator& { if constexpr (Reverse) next(); else prev(); return *this; }

	template<class R1, bool R2>	bool operator == (hash_map_iterator<Map, R1, R2> const& _r) const noexcept { return (reinterpret_cast<uintptr_t>(mCur) & ~3) == (reinterpret_cast<uintptr_t>(_r.mCur) & ~3); }
	template<class R1, bool R2>	bool operator != (hash_map_iterator<Map, R1, R2> const& _r) const noexcept { return (reinterpret_cast<uintptr_t>(mCur) & ~3) != (reinterpret_cast<uintptr_t>(_r.mCur) & ~3); }

protected:
	void next()
	{
		if (mCur == hashmap_type::head())
		{
			mCur	= mMap->mHead;
		}
		else
		{
			mCur	= mCur->next;
			if (!mCur)
			{
				mCur	= hashmap_type::tail();
			}
		}
	}

	void prev()
	{
		if (mCur == hashmap_type::tail())
		{
			mCur	= mMap->mTail;
		}
		else
		{
			mCur	= mCur->prev;
			if (!mCur)
			{
				mCur	= hashmap_type::head();
			}
		}
	}



	hashmap_type*	mMap	= nullptr;
	node_type*		mCur	= nullptr;

	template<class, class, class, size_t>
	friend	class	hash_map;

	template<class, class, bool>
	friend	class	hash_map_iterator;
};


template<class Key, class T, class Compare, size_t TableBitSize>
hash_map<Key, T, Compare, TableBitSize>::hash_map(hash_map&& _r)
{
	for (size_type i = 0; i < table_size; ++i)
	{
		mRoot[i] 	= _r.mRoot[i];
		_r.mRoot[i]	= nullptr;
	}
	mHead	= _r.mHead;	_r.mHead = nullptr;
	mTail	= _r.mTail;	_r.mTail = nullptr;
}


template<class Key, class T, class Compare, size_t TableBitSize>
auto hash_map<Key, T, Compare, TableBitSize>::operator = (hash_map&& _r) -> hash_map&
{
	clear();
	for (size_type i = 0; i < table_size; ++i)
	{
		mRoot[i] 	= _r.mRoot[i];
		_r.mRoot[i]	= nullptr;
	}
	mHead	= _r.mHead;	_r.mHead = nullptr;
	mTail	= _r.mTail;	_r.mTail = nullptr;
	return	*this;
}


template<class Key, class T, class Compare, size_t TableBitSize>
void hash_map<Key, T, Compare, TableBitSize>::clear()
{
	for (size_type i = 0; i < table_size; ++i)
	{
		if (mRoot[i])
		{
			delete	mRoot[i];
			mRoot[i]	= nullptr;
		}
	}
	mSize	= 0;
	mHead	= nullptr;
	mTail	= nullptr;
}


template<class Key, class T, class Compare, size_t TableBitSize>
void hash_map<Key, T, Compare, TableBitSize>::swap(hash_map& _r)
{
	for (size_type i = 0; i < table_size; ++i)
	{
		mRoot[i]->swap(*_r.mRoot[i]);
	}
	std::swap(mSize, _r.mSize);
	std::swap(mHead, _r.mHead);
	std::swap(mTail, _r.mTail);
}


template<class Key, class T, class Compare, size_t TableBitSize>
auto hash_map<Key, T, Compare, TableBitSize>::erase(iterator _pos) -> iterator
{
	return	_routing(_pos->first,
	// match
	[&](auto& cur, auto hash)
	{
		node*	tn	= nullptr;
		if (!cur->empty())
		{
			node**	pos	= cur->fast();
			while (!(*pos)->empty()) { pos = (*pos)->fast(); }
			tn	= *pos;
			*pos	= nullptr;
			for (int i = 0; i < table_size; ++i)
			{
				tn->children[i]		= cur->children[i];
				cur->children[i]	= nullptr;
			}
		}
		node*	nx	= cur->next;
		if (mHead == cur)	mHead	= cur->next;
		if (mTail == cur)	mTail	= cur->prev;
		if (cur->prev)		cur->prev->next	= cur->next;
		if (cur->next)		cur->next->prev	= cur->prev;
		delete	cur;
		cur	= tn;
		--mSize;
		return	nx ? nx : tail();
	},
	// unmatch
	[this](auto& cur, auto hash)
	{
		return tail();
	}).first;
}


template<class Key, class T, class Compare, size_t TableBitSize>
auto hash_map<Key, T, Compare, TableBitSize>::erase(key_type const& _key) -> size_type
{
	size_type	s	= 0;
	_routing(_key,
	// match
	[&](auto& cur, auto hash)
	{
		node*	tail	= nullptr;
		if (!cur->empty())
		{
			node**	pos	= cur->fast();
			while (!(*pos)->empty()) { pos = (*pos)->fast(); }
			tail	= *pos;
			*pos	= nullptr;
			for (int i = 0; i < table_size; ++i)
			{
				tail->children[i]	= cur->children[i];
				cur->children[i]	= nullptr;
			}
		}
		if (mHead == cur)	mHead	= cur->next;
		if (mTail == cur)	mTail	= cur->prev;
		if (cur->prev)		cur->prev->next	= cur->next;
		if (cur->next)		cur->next->prev	= cur->prev;
		delete	cur;
		cur	= tail;
		--mSize;
		s	= 1;
		return	nullptr;
	},
	// unmatch
	[](auto& cur, auto hash)
	{
		return nullptr;
	});
	return	s;
}


template<class Key, class T, class Compare, size_t TableBitSize>
auto hash_map<Key, T, Compare, TableBitSize>::find(key_type const& _key) -> iterator
{
	return	_routing(_key,
		[this](auto& n, auto hash) { return n; },
		[this](auto& n, auto hash) { return tail(); }
	).first;
}

template<class Key, class T, class Compare, size_t TableBitSize>
template<class... Args>
auto hash_map<Key, T, Compare, TableBitSize>::_emplace(node_type* _pos, key_type const& _key, Args&&... _args) -> std::pair<iterator, bool>
{
	return
	_routing(_key,
		[&](auto& n, auto hash) { n->value.second = value_type(_key, std::forward<Args>(_args)...); return n; },
		[&](auto& n, auto hash) { n = _bind(tail(), new node(_key, std::forward<Args>(_args)..., hash)); return n; }
	);
}


template<class Key, class T, class Compare, size_t TableBitSize>
template <class... Args>
auto hash_map<Key, T, Compare, TableBitSize>::try_emplace(key_type const& _key, Args&&... _args) -> std::pair<iterator, bool>
{
	return	_routing(_key,
		[&](auto& n, auto hash) { return n; },
		[&](auto& n, auto hash) { n = _bind(tail(), new node(value_type(_key, std::forward<Args>(_args)...), hash)); return n; }
	);
}


template<class Key, class T, class Compare, size_t TableBitSize>
template <class... Args>
auto hash_map<Key, T, Compare, TableBitSize>::try_emplace(key_type&& _key, Args&&... _args) -> std::pair<iterator, bool>
{
	return	_routing(_key,
		[&](auto& n, auto hash) { return n; },
		[&](auto& n, auto hash) { n = _bind(tail(), new node(value_type(std::move(_key), std::forward<Args>(_args)...), hash)); return n; }
	);
}


template<class Key, class T, class Compare, size_t TableBitSize>
template <class... Args>
auto hash_map<Key, T, Compare, TableBitSize>::try_emplace(const_iterator _hint, key_type const& _key, Args&&... _args) -> std::pair<iterator, bool>
{
	return	_routing(_key,
		[&](auto& n, auto hash) { return n; },
		[&](auto& n, auto hash) { n = _bind(_hint, new node(value_type(_key, std::forward<Args>(_args)...), hash)); return n; }
	);
}


template<class Key, class T, class Compare, size_t TableBitSize>
template <class... Args>
auto hash_map<Key, T, Compare, TableBitSize>::try_emplace(const_iterator _hint, key_type&& _key, Args&&... _args) -> std::pair<iterator, bool>
{
	return	_routing(_key,
		[&](auto& n, auto hash) { return n; },
		[&](auto& n, auto hash) { n = _bind(_hint, new node(value_type(std::move(_key), std::forward<Args>(_args)...), hash)); return n; }
	);
}


template<class Key, class T, class Compare, size_t TableBitSize>
template<class... Args>
auto hash_map<Key, T, Compare, TableBitSize>::_emplace(node_type* _pos, key_type&& _key, Args&&... _args) -> std::pair<iterator, bool>
{
	return
	_routing(_key,
		[&](auto& n, auto hash) { n->value.second = value_type(std::move(_key), std::forward<Args>(_args)...); return n; },
		[&](auto& n, auto hash) { n = _bind(tail(), new node(_key, std::forward<Args>(_args)..., hash)); return n; }
	);
}


template<class Key, class T, class Compare, size_t TableBitSize>
auto hash_map<Key, T, Compare, TableBitSize>::_emplace(node_type* _pos, value_type _val) -> std::pair<iterator, bool>
{
	return
	_routing(_val.first,
		[&](auto& n, auto hash) { n->value.second = std::move(_val.second); return n; },
		[&](auto& n, auto hash) { n = _bind(_pos, new node(std::move(_val), hash)); return n; }
	);
}

AMTRS_NAMESPACE_END
#endif

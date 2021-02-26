/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libcurry__tree__hash_node__hpp
#define	__libcurry__tree__hash_node__hpp
#include <iterator>
#include <utility>
namespace tree { namespace algorithm {

// ==========================================================================
// ハッシュノードアルゴリズム
// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
template<std::size_t TableBitSize = 2>
struct	hash_node
{
	template<class Traits> struct node;
	template<class Traits> struct root;


protected:
	// local values.
	static constexpr std::size_t	_shift_bit_size	= TableBitSize;
	static constexpr std::size_t	_table_size		= 1<<TableBitSize;
	static constexpr std::size_t	_table_mask		= _table_size-1;

	// ハッシュ値を割って次のハッシュを計算する
	static constexpr std::size_t make_xor_value(std::size_t _h)
	{
		return	(_h >> 2) ^ (_h >> 13);
	}

	struct	hash_compute
	{
		std::size_t		m_original;
		std::size_t		m_xor;
		std::size_t		m_hash;

		inline hash_compute(std::size_t _hash)
			: m_original(_hash)
			, m_xor(make_xor_value(_hash))
			, m_hash(_hash)
		{
		}

		inline std::size_t original() const noexcept
		{
			return	m_original;
		}

		inline std::size_t operator () () noexcept
		{
			std::size_t	retval = (m_hash & _table_mask);
			m_hash = (m_hash >> _shift_bit_size) ^ m_xor;
			return	retval;
		}

	};

public:
	// ======================================================================
	// イテレータのベース
	// ----------------------------------------------------------------------
	template<class Value, class Traits>
	struct	iterator_base
//			: public std::iterator<typename std::conditional<std::is_const<Value>::value, std::forward_iterator_tag, std::input_iterator_tag>::type, Value>
	{
		using	value_type	= typename Traits::value_type;
		using	root_type	= typename Traits::root_type;

		iterator_base() : m_cursor(nullptr), m_root(nullptr) {}
		iterator_base(const iterator_base&  _r) : m_cursor(_r.m_cursor), m_root(_r.m_root) {}
		iterator_base(      iterator_base&& _r) : m_cursor(_r.m_cursor), m_root(_r.m_root) { _r.m_cursor = nullptr; _r.m_root = nullptr; }
		template<class U>
		iterator_base(const iterator_base<U, Traits>& _r)
			: m_cursor(_r.m_cursor)
			, m_root(_r.m_root)
		{
		}
		iterator_base(value_type* _cursor, root_type* _root) : m_cursor(_cursor), m_root(_root) {}

		value_type* operator -> () const noexcept { return m_cursor; }
		value_type& operator *  () const noexcept { return *m_cursor; }

		bool operator != (const iterator_base& _r) const noexcept { return m_cursor != _r.m_cursor; }
		iterator_base& operator ++ ();

		iterator_base& operator = (const iterator_base& _r) noexcept
		{
			m_cursor	= _r.m_cursor;
			m_root		= _r.m_root;
			return	*this;
		}

		iterator_base& operator = (iterator_base&& _r) noexcept
		{
			m_cursor	= std::move(_r.m_cursor);
			m_root		= std::move(_r.m_root);
			return	*this;
		}

		value_type* get() const noexcept { return m_cursor; }

	private:
		value_type*		m_cursor;
		root_type*		m_root;
		friend	typename Traits::algorithm;
	};

	template<class Traits>
	using	iterator		= iterator_base<typename Traits::value_type, Traits>;

	template<class Traits>
	using	const_iterator	= iterator_base<const typename Traits::value_type, Traits>;

	template<class Traits>
	struct	node
	{
		using	value_type	= typename Traits::value_type;

		node()
		{
			for (std::size_t i = 0; i < _table_size; ++i)
			{
				m_childs[i] = nullptr;
			}
		}

		node(const node&)
		{
			for (std::size_t i = 0; i < _table_size; ++i)
			{
				m_childs[i] = nullptr;
			}
		}

		void swap(node& _other)
		{
			for (std::size_t i = 0; i < _table_size; ++i)
			{
				std::swap(m_childs[i], _other.m_childs[i]);
			}
		}

	protected:
		value_type*		m_childs[_table_size];
		friend	typename Traits::algorithm;
	};


	template<class Traits>
	struct	root
	{
		using	value_type	= typename Traits::value_type;

		root() : m_root(nullptr) {}

	protected:
		value_type*		m_root;
		friend	typename Traits::algorithm;
	};


	template<class Traits>
	static auto begin(root<Traits>& _root) noexcept -> iterator<Traits>
	{
		using	node_type	= typename Traits::node_type;
		using	value_type	= typename Traits::value_type;
		using	to_node		= typename Traits::to_node;

		value_type*		cursor	= _root.m_root;
		if (cursor)
		{
			// 末端のノードを探して返す
			to_node			toNode{};
			value_type**	childs	= toNode(*cursor).m_childs;
			for (std::size_t i = 0; i < _table_size; )
			{
				if (childs[i])
				{
					cursor	= childs[i];
					childs	= toNode(*cursor).m_childs;
					i		= 0;
					continue;
				}
				++i;
			}
		}
		return	{ cursor, &_root };
	}


	template<class Traits>
	static auto end(root<Traits>& _root) noexcept -> iterator<Traits>
	{
		return	{ nullptr, &_root };
	}



	template<class Traits, class Hint, class Compare>
	static auto find(root<Traits>& _root, const Hint& _hint, Compare _compare) -> iterator<Traits>
	{
		using	node_type	= typename Traits::node_type;
		using	value_type	= typename Traits::value_type;
		using	to_node		= typename Traits::to_node;

		if (_root.m_root)
		{
			to_node			toNode{};
			value_type*		val		= _root.m_root;
			hash_compute	hash(hash_code(_hint));
			do
			{
				if (_compare(const_iterator<Traits>{val, &_root}))
				{
					return	{val, &_root};
				}
				val		= toNode(*val).m_childs[hash()];
			} while (val);
		}
		return	{nullptr, &_root};
	}



	template<class Traits, class Hint, class Finder, class Factory>
	static auto insert(root<Traits>& _root, const Hint& _hint, Finder&& _finder, Factory&& _factory) -> iterator<Traits>
	{
		using	node_type	= typename Traits::node_type;
		using	value_type	= typename Traits::value_type;
		using	to_node		= typename Traits::to_node;

		to_node			toNode{};
		value_type**	valRef{&_root.m_root};
		if (*valRef)
		{
			hash_compute	hash(hash_code(_hint));
			do
			{
				if (hash_code(**valRef) == hash.original() && _finder(const_iterator<Traits>{*valRef, &_root}))
				{
					return	{*valRef, &_root};
				}
				node_type&	n(toNode(**valRef));
				valRef		= &n.m_childs[hash()];
			} while (*valRef);
		}

		value_type*		e{_factory()};
		node_type&	n(toNode(*e));
		memset(n.m_childs, 0, sizeof(n.m_childs));
		*valRef	= e;
		return	{e, &_root};
	}




	template<class Traits, class Key, class CreateNodeCallback>
	static auto insert(root<Traits>& _root, Key&& _key, CreateNodeCallback&& _factory) -> iterator<Traits>
	{
		using	node_type	= typename Traits::node_type;
		using	value_type	= typename Traits::value_type;
		using	to_node		= typename Traits::to_node;

		to_node			toNode{};
		value_type**	valRef{&_root.m_root};
		if (*valRef)
		{
			hash_compute	hash(hash_code(_key));
			do
			{
				if (key(**valRef) == _key)
				{
					return	{*valRef, &_root};
				}
				node_type&	n(toNode(**valRef));
				valRef		= &n.m_childs[hash()];
			} while (*valRef);
		}

		if (!*valRef)
		{
			value_type*		e{_factory()};
			node_type&	n(toNode(*e));
			memset(n.m_childs, 0, sizeof(n.m_childs));
			*valRef	= e;
		}
		return	{*valRef, &_root};
	}



	template<class Traits, class Target>
	static auto remove(root<Traits>& _root, Target&& _target) -> typename Traits::value_type*
	{
		using	node_type	= typename Traits::node_type;
		using	value_type	= typename Traits::value_type;
		using	to_node		= typename Traits::to_node;

		to_node			toNode{};
		value_type**	valRef	= &_root.m_root;
		hash_compute	hash(hash_code(_target));
		while (*valRef)
		{
			value_type*		val	= *valRef;
			node_type&		n	= toNode(*val);

			// 削除対象ではないなら次のノードへ上る
			if (!(key(*val) == _target))
			{
				valRef	= &n.m_childs[hash()];
				continue;
			}

			// 外したいノードが見つかったので、末端にある適当なノードと入れ替える。
			value_type**	termRef	= nullptr;
			value_type**	childs	= n.m_childs;
			for (std::size_t i = 0; i < _table_size; )
			{
				if (childs[i])
				{
					termRef	= &childs[i];
					childs	= toNode(**termRef).m_childs;
					i		= 0;
					continue;
				}
				++i;
			}

			if (termRef)
			{
				// 検出したノードの子情報を入れ替え先にコピーし、末端への参照を外しておく
				value_type*		term	= *termRef;
				*termRef	= nullptr;
				n.swap(toNode(*term));
				*valRef		= term;
			}
			else
			{
				// 削除対象を参照していたところを末端ノードの参照に置き換える
				*valRef	= nullptr;
			}
			return	val;
		}
		return	nullptr;
	}


	template<class Traits>
	static auto get_refereene(root<Traits>& _root, typename Traits::value_type* _node) -> std::pair<typename Traits::value_type*, std::size_t>
	{
		using	node_type	= typename Traits::node_type;
		using	value_type	= typename Traits::value_type;
		using	to_node		= typename Traits::to_node;

		if (!_root.m_root)
		{
			return	{nullptr, 0};
		}

		value_type*		val	= _root.m_root;
		hash_compute	hash(hash_code(*_node));
		to_node			toNode{};
		for (;;)
		{
			std::size_t		index	= hash();
			value_type**	childs	= toNode(*val).m_childs;
			if (childs[index] == _node || childs[index] == nullptr)
			{
				return	{val, index};
			}
			val		= childs[index];
		}
	}
};


template<std::size_t TableBitSize>
template<class Value, class Traits>
auto hash_node<TableBitSize>::iterator_base<Value, Traits>::operator ++ () -> iterator_base&
{
	using	node_type	= typename Traits::node_type;
	using	value_type	= typename Traits::value_type;
	using	to_node		= typename Traits::to_node;

	if (!m_cursor)
	{
		return	*this;
	}

	if (m_root->m_root == m_cursor)
	{
		// ルートノードの次はない
		m_cursor = nullptr;
		return	*this;
	}

	auto			ref		= get_refereene(*m_root, m_cursor);
	to_node			toNode{};
	m_cursor		= ref.first;
	value_type**	childs	= toNode(*m_cursor).m_childs;
	for (std::size_t i = ref.second + 1; i < _table_size;)
	{
		if (childs[i])
		{
			m_cursor	= childs[i];
			childs		= toNode(*m_cursor).m_childs;
			i			= 0;
			continue;
		}
		++i;
	}
	return	*this;
}

} }
#endif

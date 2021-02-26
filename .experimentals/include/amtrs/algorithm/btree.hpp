/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__algorithm__btree__hpp
#define	__libamtrs__algorithm__btree__hpp
#include "../amtrs.hpp"
AMTRS_NAMESPACE_BEGIN


template<class Key, class T>
class	btree
{
protected:
	static constexpr int	harfchildren	= 2;
	static constexpr int	maxchildren		= harfchildren * 2;
	struct	leaf;

	template<class RefT, class Leaf, class Cursor>
	class	iterator_base;
	struct	incrementer;
	struct	decrementer;
public:
	using	key_type				= Key;
	using	mapped_type				= T;
	using	value_type				= std::pair<key_type, mapped_type>;
	using	pointer					= value_type*;
	using	reference				= value_type&;
	using	const_pointer			= value_type const*;
	using	const_reference			= value_type const&;
	using	iterator				= iterator_base<value_type, leaf, incrementer>;
	using	reverse_iterator		= iterator_base<value_type, leaf, decrementer>;
	using	const_iterator			= iterator_base<value_type const, leaf const, incrementer>;
	using	const_reverse_iterator	= iterator_base<value_type const, leaf const, decrementer>;

	btree()
	{
		mTerm	= new leaf();
		mTerm->prev	= mTerm;
		mTerm->next	= mTerm;
	}

	~btree()
	{
		clear();
		delete	mTerm;
	}


	iterator begin() noexcept { return iterator(mTerm->next); }
	iterator end() noexcept { return iterator(mTerm); }
	const_iterator begin() const noexcept { return const_iterator(mTerm->next); }
	const_iterator end() const noexcept { return const_iterator(mTerm); }
	const_iterator cbegin() const noexcept { return const_iterator(mTerm->next); }
	const_iterator cend() const noexcept { return const_iterator(mTerm); }

	void clear();

	void erase(key_type const& _key);

	void erase(const_iterator _it);

	iterator find(key_type _key);

	const_iterator find(key_type const& _key) const noexcept;

	//! 指定するキーより小さなキーのうち最大のものを返す
	const_iterator find_maximam(key_type const& _key) const noexcept;

	//! 指定するキーより大きなキーのうち最大のものを返す
	const_iterator find_minimam(key_type _key) const noexcept;

	iterator insert(value_type _val);



protected:
	struct	leaf
	{
		int			refcount	= 1;
		leaf*		prev		= nullptr;		//!< 自分より小さなもの
		leaf*		next		= nullptr;		//!< 自分より大きなもの

		~leaf()
		{
			if (prev)
			{
				prev->next	= next;
				next->prev	= prev;
			}
		}
	};

	struct	valueref : leaf, value_type
	{
		valueref(value_type _src)
			: value_type(std::move(_src))
		{}

		valueref(key_type key, mapped_type val)
			: value_type(std::move(key), std::move(val))
		{}

		valueref* addref()
		{
			++leaf::refcount;
			return	this;
		}

		valueref* decref()
		{
			if ((--leaf::refcount) == 0)
			{
				delete	this;
			}
			return	nullptr;
		}
	};


	struct	node
	{
		valueref*	leaf[maxchildren];
		node*		child[maxchildren + 1];
		int			uses;


		//! キーの挿入位置を調べ、posへ返します。
		bool position(int& _pos, key_type const& _key)
		{
			int	pos = 0;
			while (pos < uses && leaf[pos]->first < _key)
			{
				++pos;
			}
			_pos	= pos;
			return	pos < uses && leaf[pos]->first == _key;
		}


		//! 指定位置に葉と枝を挿入します。
		bool attach(int _pos, valueref* _leaf, node* _child)
		{
			for (int i = uses; i > _pos; --i)
			{
				leaf[i]			= leaf[i - 1];
				child[i + 1]	= child[i];
			}
			leaf[_pos]		= _leaf->addref();
			child[_pos+1]	= _child;
			++uses;

			// イテレータ用のリンクを行う
			if (!_leaf->next)
			{
				if (child[_pos])
				{
					auto	p	= child[_pos]->maximam();
					_leaf->prev	 = p;
					_leaf->next	 = p->next;
					_leaf->prev->next	= _leaf;
					_leaf->next->prev	= _leaf;
				}
				else if (child[_pos+1])
				{
					auto	p	= child[_pos+1]->maximam();
					_leaf->prev	 = p->prev;
					_leaf->next	 = p;
					_leaf->prev->next	= _leaf;
					_leaf->next->prev	= _leaf;
				}
				else if (_pos > 0)
				{
					auto	p	= leaf[_pos-1];
					_leaf->prev	 = p;
					_leaf->next	 = p->next;
					_leaf->prev->next	= _leaf;
					_leaf->next->prev	= _leaf;
				}
				else if (_pos < uses)
				{
					auto	p	= leaf[_pos+1];
					_leaf->prev	 = p->prev;
					_leaf->next	 = p;
					_leaf->prev->next	= _leaf;
					_leaf->next->prev	= _leaf;
				}
				return	true;
			}
			return	false;
		}

		//! 指定位置のアイテムを削除してリストを詰める
		//! 半数以下になった場合は true を返す。
		bool remove(int _pos)
		{
			while (++_pos < uses)
			{
				leaf[_pos-1]	= leaf[_pos];
				child[_pos]	= child[_pos+1];
			}
			return	--uses < harfchildren;
		}

		//! 最小値を取得する
		valueref* minimam() const noexcept
		{
			node const*	n	= this;
			while (n->child[0])
			{
				n	= n->child[0];
			}
			return	n->leaf[0];
		}

		//! 最小値を取得する
		valueref* maximam() const noexcept
		{
			node const*	n	= this;
			while (n->child[n->uses])
			{
				n	= n->child[n->uses];
			}
			return	n->leaf[n->uses-1];
		}
	};

	template<class RefT, class Leaf, class Cursor>
	class	iterator_base
	{
		using	valuetype	= typename std::conditional<std::is_const<Leaf>::value, valueref const, valueref>::type;
	public:
		using	value_type			= RefT;
		using	pointer				= RefT*;
		using	reference			= RefT&;
		using	iterator_concept	= std::bidirectional_iterator_tag;
		using	difference_type		= ptrdiff_t;


		iterator_base() = default;
		iterator_base(iterator_base const&) = default;
		iterator_base(iterator_base&&) = default;
		iterator_base& operator = (iterator_base const&) = default;
		iterator_base& operator = (iterator_base&&) = default;

		template<class RefT2, class Leaf2, class Cursor2>
		iterator_base(iterator_base<RefT2, Leaf2, Cursor2> const& _leaf)
			: mLeaf(_leaf.mLeaf)
		{}

		iterator_base(Leaf* _leaf)
			: mLeaf(_leaf)
		{}

		pointer operator -> () const noexcept
		{
			return	static_cast<valuetype*>(mLeaf);
		}

		reference operator * () const noexcept
		{
			return	*static_cast<valuetype*>(mLeaf);
		}

		iterator_base& operator ++ ()
		{
			mLeaf	= Cursor{}.inc(mLeaf);
			return	*this;
		}

		iterator_base& operator ++ (int)
		{
			iterator_base	r(*this);
			mLeaf	= Cursor{}.inc(mLeaf);
			return	r;
		}

		iterator_base& operator -- ()
		{
			mLeaf	= Cursor{}.dec(mLeaf);
			return	*this;
		}

		iterator_base& operator -- (int)
		{
			iterator_base	r(*this);
			mLeaf	= Cursor{}.dec(mLeaf);
			return	r;
		}

		template<class RRefT, class RLeaf, class RCur>
		bool operator == (iterator_base<RRefT, RLeaf, RCur> const& _r) const noexcept
		{
			return	mLeaf == _r.mLeaf;
		}

		template<class RRefT, class RLeaf, class RCur>
		bool operator != (iterator_base<RRefT, RLeaf, RCur> const& _r) const noexcept
		{
			return	mLeaf != _r.mLeaf;
		}

	private:
		Leaf*	mLeaf;

		template<class, class, class>
		friend	class	iterator_base;
	};

	struct	incrementer
	{
		template<class L> auto inc(L* _leaf) { return _leaf->next; }
		template<class L> auto dec(L* _leaf) { return _leaf->prev; }
	};

	struct	decrementer
	{
		template<class L> auto inc(L* _leaf) { return _leaf->prev; }
		template<class L> auto dec(L* _leaf) { return _leaf->next; }
	};


	node*		mRoot	= nullptr;			//!< B木の根
	leaf*		mTerm	= nullptr;			//!< terminate
};


template<class Key, class T>
void btree<Key, T>::clear()
{
	while (mRoot && mRoot->uses)
	{
		erase(mRoot->leaf[0]->first);
	}
}


template<class Key, class T>
void btree<Key, T>::erase(key_type const& _key)
{
	struct
	{
		bool erase(node* n, key_type key)
		{
			int	pos;
			if (n->position(pos, key))
			{
				node* c = n->child[pos + 1];
				if (!c)
				{
					n->leaf[pos]->decref();
					return	n->remove(pos);
				}

				while (c->child[0])
				{
					c = c->child[0];
				}
				n->leaf[pos]->decref();
				n->leaf[pos] = c->leaf[0];
				n->leaf[pos]->addref();
				key	= n->leaf[pos]->first;
				++pos;
			}
			auto c = n->child[pos];
			if (!c)
			{
				return	false;
			}

			if (!erase(c, key))
			{
				return	false;
			}
			if (pos > 0)
			{
				if (n->child[pos - 1]->uses > harfchildren)
				{
					node*	left	= n->child[pos - 1];
					node*	right	= n->child[pos];
					for (int i = right->uses; i > 0; --i)
					{
						right->leaf[i]		= right->leaf[i-1];
						right->child[i+1]	= right->child[i];
					}
					right->child[1]	= right->child[0];
					right->leaf[0]	= n->leaf[pos - 1];
					right->child[0]	= left->child[left->uses];
					++(right->uses);

					n->leaf[pos - 1]	= left->leaf[left->uses - 1];
					--(left->uses);
					return	false;
				}
			}
			else
			{
				if (n->child[1]->uses > harfchildren)
				{
					node*	left = n->child[0];
					node*	right = n->child[1];

					left->leaf[left->uses]		= n->leaf[0];
					left->child[left->uses+1]	= right->child[0];
					++(left->uses);

					n->leaf[0]					= right->leaf[0];
					right->child[0]				= right->child[1];
					--(right->uses);

					for (int i = 1; i <= right->uses; ++i)
					{
						right->leaf[i - 1]	= right->leaf[i];
						right->child[i]		= right->child[i + 1];
					}
					return	false;
				}
				pos	= 1;
			}

			// 子１と子２の値を合体する
			node*	c1		= n->child[pos-1];
			node*	c2		= n->child[pos];
			c1->leaf[c1->uses]		= n->leaf[pos - 1];
			c1->child[c1->uses+1]	= c2->child[0];
			++(c1->uses);
			for (int i = 1; i <= c2->uses; ++i)
			{
				c1->leaf[c1->uses]		= c2->leaf[i - 1];
				c1->child[c1->uses+1]	= c2->child[i];
				++(c1->uses);
			}
			delete	c2;
			return	n->remove(pos-1);
		}
	} local;

	if (mRoot)
	{
		local.erase(mRoot, _key);
		if (mRoot->uses == 0)
		{
			node*	n	= mRoot;
			mRoot	= mRoot->child[0];
			delete n;
		}
	}
}



template<class Key, class T>
auto btree<Key, T>::find(key_type _key) -> iterator
{
	auto*	n = mRoot;
	while (n)
	{
		int	pos;
		if (n->position(pos, _key))
		{
			return	iterator(n->leaf[pos]);
		}
		n = n->child[pos];
	}
	return	end();
}


template<class Key, class T>
auto btree<Key, T>::find(key_type const& _key) const noexcept -> const_iterator
{
	auto*	n = mRoot;
	while (n)
	{
		int	pos;
		if (n->position(pos, _key))
		{
			return	iterator(n->leaf[pos]);
		}
		n = n->child[pos];
	}
	return	end();
}


template<class Key, class T>
auto btree<Key, T>::find_maximam(key_type const& _key) const noexcept -> const_iterator
{
	auto	n	= mRoot;
	while (n)
	{
		for (int pos = n->uses-1; pos >= 0; --pos)
		{
			if (_key <= n->leaf[pos]->first)
			{
				continue;
			}
			if (auto c = n->child[pos+1]; c)
			{
				if (auto l = c->minimam(); l->first < _key)
				{
					n	= c;
					pos	= c->uses;
					continue;
				}
			}
			return	n->leaf[pos];
		}
		n	= n->child[0];
	}
	return	cend();
}


template<class Key, class T>
auto btree<Key, T>::find_minimam(key_type _key) const noexcept -> const_iterator
{
	auto	n	= mRoot;
	while (n)
	{
		for (int pos = 0; pos < n->uses; ++pos)
		{
			if (n->leaf[pos]->first <= _key)
			{
				continue;
			}
			if (auto c = n->child[pos]; c)
			{
				if (auto l = c->maximam(); _key < l->first)
				{
					n	= c;
					pos	= -1;
					continue;
				}
			}
			return	n->leaf[pos];
		}
		n	= n->child[n->uses];
	}
	return	cend();
}


template<class Key, class T>
auto btree<Key, T>::insert(value_type _val) -> iterator
{
	struct
	{
		bool insert(node* n)
		{
			int	pos;
			if (n->position(pos, val->first))
			{
				it	= n->leaf[pos];
				return	true;	// 挿入済み
			}

			if (auto c = n->child[pos]; c)
			{
				if (insert(c))
				{
					return	true;
				}
			}
			else
			{
				brun = nullptr;
			}

			// 空きのあるノードに値を挿入
			if (n->uses < maxchildren)
			{
				if (n->attach(pos, val, brun))
				{
					it	= n->leaf[pos];
				}
				return	true;
			}

			node*	q	= new node();
			int		m	= pos <= harfchildren
						? harfchildren
						: harfchildren + 1;
			for (int i = m + 1; i <= maxchildren; ++i)
			{
				q->leaf[i - m - 1]	= n->leaf[i - 1];
				q->child[i - m]	= n->child[i];
			}

			q->uses	= maxchildren - m;
			n->uses	= m;
			if (pos <= harfchildren)
			{
				if (n->attach(pos, val, brun))
				{
					it	= n->leaf[pos];
				}
			}
			else
			{
				if (q->attach(pos - m, val, brun))
				{
					it	= q->leaf[pos - m];
				}
			}
			val->decref();
			val		= std::move(n->leaf[n->uses - 1]);

			q->child[0] = n->child[n->uses];
			--(n->uses);
			brun	= q;
			return	false;
		}

		node*		brun	= nullptr;	// 新しいブランチ
		valueref*	val		= nullptr;	// 動かしている値
		valueref*	it		= nullptr;
	}local;

	local.val	= new valueref(std::move(_val));
	if (!mRoot || !local.insert(mRoot))
	{
		node*	n	= new node();
		n->uses			= 1; 
		n->child[0]	= mRoot;
		n->leaf[0]	= local.val->addref();
		n->child[1]	= local.brun;
		if (!local.it)
		{
			local.it	= n->leaf[0];
		}
		if (!mRoot)
		{
			local.val->prev	= mTerm;
			local.val->next	= mTerm;
			mTerm->prev	= local.val;
			mTerm->next	= local.val;
		}
		mRoot			= n;
	}
	local.val->decref();
	return	local.it;
}



AMTRS_NAMESPACE_END
#endif

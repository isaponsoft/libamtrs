/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libcurry__tree__red_black__hpp
#define	__libcurry__tree__red_black__hpp
#include <iterator>
#include <utility>
namespace tree { namespace algorithm {
namespace detail { namespace red_black_impl {
template<class Traits> struct impl;
} }
// ==========================================================================
// 赤黒木のアルゴリズムを提供します。
// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
struct	red_black
{
	template<class Traits> struct node;
	template<class Traits> struct root;

	enum class color_t
	{
		none,
		red,
		black
	};



	// =======================================================================
	// イテレータ
	// -----------------------------------------------------------------------
	template<class Value, class Traits>
	struct	iterator_base
			: public std::iterator<typename std::conditional<std::is_const<Value>::value, std::forward_iterator_tag, std::input_iterator_tag>::type, Value>
	{
		using	value_type	= typename Traits::value_type;

		iterator_base() noexcept
			: m_cursor(nullptr)
			, m_depth(0)
		{}

		iterator_base(iterator_base&&) = default;
		iterator_base(const iterator_base&) = default;
		iterator_base(value_type* _cursor, std::size_t _depth = 0) noexcept
			: m_cursor(_cursor)
			, m_depth(_depth)
		{
		}
		template<class U>
		iterator_base(const iterator_base<U, Traits>& _r)
			: m_cursor(_r.m_cursor)
			, m_depth(_r.m_depth)
		{
		}

		value_type* operator -> () const noexcept { return m_cursor; }
		value_type& operator *  () const noexcept { return *m_cursor; }

		bool operator != (const iterator_base& _r) const noexcept { return m_cursor != _r.m_cursor; }

		iterator_base& operator ++ ();

		iterator_base& operator = (const iterator_base& _r) noexcept
		{
			m_cursor	= _r.m_cursor;
			m_depth		= _r.m_depth;
			return	*this;
		}

		iterator_base& operator = (iterator_base&& _r) noexcept
		{
			m_cursor	= std::move(_r.m_cursor);
			m_depth		= std::move(_r.m_depth);
			return	*this;
		}
	private:
		value_type*		m_cursor;
		std::size_t		m_depth;
		friend	typename Traits::algorithm;
		friend	detail::red_black_impl::impl<Traits>;
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
			: m_color	(color_t::none)
			, m_parent	(nullptr)
			, m_left	(nullptr)
			, m_right	(nullptr)
		{
		}

		void swap(node& _other)
		{
			std::swap(m_color,	_other.m_color);
			std::swap(m_parent,	_other.m_parent);
			std::swap(m_left,	_other.m_left);
			std::swap(m_right,	_other.m_right);
		}

	protected:
		color_t			m_color;
		value_type*		m_parent;
		value_type*		m_left;
		value_type*		m_right;
		friend	typename Traits::algorithm;
		friend	detail::red_black_impl::impl<Traits>;
	};



	// =======================================================================
	// このアルゴリズムで使用する木のルート
	// -----------------------------------------------------------------------
	template<class Traits>
	struct	root
	{
	public:
		using	value_type	= typename Traits::value_type;
		root() : m_root(nullptr) {}


	protected:
		value_type*		m_root;
		friend	typename Traits::algorithm;
		friend	detail::red_black_impl::impl<Traits>;
	};


	template<class Traits>
	static auto begin(root<Traits>& _root) noexcept -> iterator<Traits>;

	template<class Traits>
	static auto end(root<Traits>& _root) noexcept -> iterator<Traits>;

	template<class Traits, class Key, class EqualsTo>
	static auto find(root<Traits>& _root, const Key& _key, EqualsTo _equale_to) -> iterator<Traits>;

	template<class Traits, class Key, class CreateNodeCallback>
	static auto insert(root<Traits>& _root, const Key& _key, CreateNodeCallback&& _factory) -> iterator<Traits>;

	template<class Traits, class Key>
	static auto remove(root<Traits>& _root, const Key& _key) -> typename Traits::value_type*;
};

namespace detail { namespace red_black_impl {

template<class Traits>
struct	impl
{
	using	element_type	= typename Traits::value_type;
	using	to_node			= typename Traits::to_node;
	using	key_compare		= typename Traits::key_compare;
	using	iterator		= red_black::iterator<Traits>;
	using	root_type		= red_black::root<Traits>;
	using	color_t			= red_black::color_t;

	static bool is_red  (element_type* _e) { return _e ? color(_e) == color_t::red   : false; }
	static bool is_black(element_type* _e) { return _e ? color(_e) == color_t::black : false; }
	static color_t&       color (element_type* _e) { return to_node{}(*_e).m_color;  }
	static element_type*& parent(element_type* _e) { return to_node{}(*_e).m_parent; }
	static element_type*& left  (element_type* _e) { return to_node{}(*_e).m_left;   }
	static element_type*& right (element_type* _e) { return to_node{}(*_e).m_right;  }

	static auto begin(root_type& _root) noexcept -> iterator
	{
		element_type*	cursor	= _root.m_root;
		std::size_t		depth	= 0;
		if (cursor)
		{
			while (left(cursor))
			{
				cursor = left(cursor);
				++depth;
			}
		}
		return	{ cursor, depth };
	}


	static auto end(root_type& _root) noexcept -> iterator
	{
		return	{ nullptr, 0 };
	}



	template<class Key, class Finder>
	static auto find(root_type& _root, const Key& _key, Finder _finder) -> iterator
	{
		element_type*	cursor	= _root.m_root;
		while (cursor)
		{
			if (_finder(iterator{cursor}))
			{
				return	{cursor};
			}
			cursor	= (!(key_compare{}(key(*cursor), _key)))
					? left(cursor)
					: right(cursor);
		}
		return	{nullptr};
	}


	template<class Key, class Finder, class Factory>
	static auto insert(root_type& _root, const Key& _key, Finder&& _finder, Factory&& _factory) -> iterator
	{
		element_type*	cursor	= _root.m_root;
		while (cursor)
		{
			if (_finder(iterator{cursor}))
			{
				return	{cursor};
			}
			cursor	= (!(key_compare{}(key(*cursor), _key)))
					? left(cursor)
					: right(cursor);
		}
		return	{nullptr};
	}

	template<class Key, class CreateNodeCallback>
	static auto insert(root_type& _root, const Key& _key, CreateNodeCallback&& _factory) -> iterator
	{
		element_type*	select	= nullptr;
		_root.m_root			= _insert(&select, _root.m_root, _key, std::forward<CreateNodeCallback>(_factory)).first;
		color(_root.m_root)	= color_t::black;
		parent(_root.m_root)	= nullptr;
		return	{select};
	}


	template<class Key>
	static auto remove(root_type& _root, Key&& _key) -> element_type*
	{
		if (_root.m_root)
		{
			element_type*	sel = nullptr;
			_root.m_root	= _del(&sel, _root.m_root, _key).first;
			return	sel;
		}
		return	nullptr;	
	}


	template<class Key>
	static auto _del(element_type** _selected, element_type* _current, const Key& _key) -> std::pair<element_type*, bool>
	{
		if (_current == nullptr)
		{
			return	std::pair<element_type*, bool>(nullptr, true);
		}

		// 削除対象を検出した
		if (key(*_current) == _key)
		{
			*_selected	= _current;
			if (!left(_current) && !right(_current))
			{
				return	{nullptr,true};	// バランスは変えない
			}
			else
			if (!right(_current))		// left && !right
			{
				color(left(_current))	= color_t::black;
				return	{left(_current), true};
			}
			else
			if (!left(_current))		// !left && right
			{
				color(right(_current))	= color_t::black;
				return	{right(_current), true};
			}
			else
			{
				// 左の中から一番大きな値を自分の位置へ移動する。
				element_type*	minnode	= _del_minimam(&right(_current));
				color(minnode)	= color(_current);
				left(minnode)	= left(_current);
				right(minnode)	= right(_current);
				return	_del_balance_left(minnode, true);
			}
		}

		// 左右に再帰
		if (!(key_compare{}(key(*_current), _key)))
		{
			auto	ret	= _del(_selected, left(_current),  _key);
			left(_current)	= ret.first;
			if (ret.first)
			{
				parent(ret.first)	= _current;
			}
			return	_del_balance_left(_current, ret.second);
		}
		else
		{
			auto	ret	= _del(_selected, right(_current), _key);
			right(_current)	= ret.first;
			if (ret.first)
			{
				parent(ret.first)	= _current;
			}
			return	_del_balance_light(_current, ret.second);
		}
	}


	static auto _del_minimam(element_type** _root_ref) -> element_type*
	{
		if (!*_root_ref)
		{
			// そもそも自分が葉だった
			return	nullptr;
		}

		element_type**	ref	= _root_ref;
		while (left(*ref))
		{
			ref	= &left(*ref);
		}

		element_type*	ret	= *ref;
		*ref	= nullptr;
		return	ret;
	}


	static auto _del_balance_left(element_type* _current, bool _ballanced) -> std::pair<element_type*, bool>
	{
		// バランスが取れているなら何もしない
		if (_ballanced)
		{
			return	{_current, _ballanced};
		}

		if (is_black(left(right(_current))) && is_black(right(right(_current))))
		{
			if (is_black(right(_current)))
			{
				color(right(_current))	= color_t::red;
				if (is_black(_current))
				{
					return	{_current, false};
				}
				color(_current)			= color_t::black;
			}
			else
			{
				_current		= _rotate_left(_current);
				left(_current)	= _del_balance_left(left(_current), false).first;
			}
		}	
		else
		{
			if (is_red(left(right(_current))))
			{
				right(_current)	= _rotate_right(right(_current));
			}
			_current	= _rotate_left(_current);
			color(left(_current))	= color_t::black;
			color(right(_current))	= color_t::black;
		}

		return	{_current, true};
	}


	static auto _del_balance_light(element_type* _current, bool _ballanced) -> std::pair<element_type*, bool>
	{
		// バランスが取れているなら何もしない
		if (_ballanced)
		{
			return	{_current, _ballanced};
		}

		if (is_black(left(left(_current))) && is_black(right(left(_current))))
		{
			if (is_black(left(_current)))
			{
				color(left(_current))	= color_t::red;
				if (is_black(_current))
				{
					return	{_current, false};
				}
				color(_current)			= color_t::black;
			}
			else
			{
				_current		= _rotate_right(_current);
				right(_current)	= _del_balance_light(right(_current), false).first;
			}
		}	
		else
		{
			if (is_red(right(left(_current))))
			{
				left(_current)	= _rotate_left(left(_current));
			}
			_current	= _rotate_right(_current);
			color(right(_current))	= color_t::black;
			color(left(_current))	= color_t::black;
		}

		return	{_current, true};
	}


	template<class Key, class CreateNodeCallback>
	static auto _insert(element_type** _selected, element_type* _current, const Key& _key, CreateNodeCallback&& _factory) -> std::pair<element_type*, bool>
	{
		// 終端まで辿り着いたので、新規にノードを作成する。
		if (!_current)
		{
			element_type*	e	= _factory();
			color(e)	= color_t::red;
			parent(e)	= nullptr;
			left(e)		= nullptr;
			right(e)	= nullptr;
			*_selected	= e;
			return	{e, false};			// バランスが崩れた
		}

		// 同一のキーを発見した
		if (key(*_current) == _key)
		{
			*_selected	= _current;
			return	{_current, true};	// バランスは保持したまま
		}

		// 左右に移動
		if (!(key_compare{}(key(*_current), _key)))
		{
			auto	res	= _insert<Key, CreateNodeCallback>(_selected, left(_current),  _key, std::forward<CreateNodeCallback>(_factory));
			left(_current)		= res.first;
			parent(res.first)	= _current;
			if (!res.second)
			{
				if (is_black(_current))
				{
					res.second	= true;
					if (is_red(right(left(_current))))
					{
						left(_current)	= _rotate_left(left(_current));
					}
					if (is_red(left(left(_current))))
					{
						if (is_red(right(_current)))
						{
							color(_current)			= color_t::red;
							color(left(_current))	= color_t::black;
							color(right(_current))	= color_t::black;
							res.second	= false;
						}
						else
						{
							_current	= _rotate_right(_current);
						}
					}
				}
			}
			return	{_current, res.second};
		}
		else
		{
			auto	res	= _insert<Key, CreateNodeCallback>(_selected, right(_current), _key, std::forward<CreateNodeCallback>(_factory));
			right(_current)		= res.first;
			parent(res.first)	= _current;
			if (!res.second)
			{
				if (is_black(_current))
				{
					res.second	= true;
					if (is_red(left(right(_current))))
					{
						right(_current)	= _rotate_right(right(_current));
					}
					if (is_red(right(right(_current))))
					{
						if (is_red(left(_current)))
						{
							color(_current)			= color_t::red;
							color(left(_current))	= color_t::black;
							color(right(_current))	= color_t::black;
							res.second	= false;
						}
						else
						{
							_current	= _rotate_left(_current);
						}
					}
				}
			}
			return	{_current, res.second};
		}
	}


	static auto _rotate_left(element_type* _current) -> element_type*
	{
		element_type*	p	= parent(_current);
		element_type*	r	= right(_current);

		right(_current)		= left(r);
		if (left(r))
		{
			parent(left(r))		= _current;
		}
		left(r)				= _current;
		color(r)			= color(_current);
		color(_current)		= color_t::red;

		parent(r)			= p;
		parent(_current)	= r;
		return	r;
	}


	static auto _rotate_right(element_type* _current) -> element_type*
	{
		element_type*	p	= parent(_current);
		element_type*	l	= left(_current);

		left(_current)		= right(l);
		if (right(l))
		{
			parent(right(l))	= _current;
		}
		right(l)			= _current;
		color(l)			= color(_current);
		color(_current)		= color_t::red;

		parent(l)			= p;
		parent(_current)	= l;
		return	l;
	}
};

} }



template<class Value, class Traits>
auto red_black::iterator_base<Value, Traits>::operator ++ () -> iterator_base&
{
	using	key_compare		= typename Traits::key_compare;

	if (detail::red_black_impl::impl<Traits>::right(m_cursor))
	{
		// 自分より大きなノードの中の最小値を次の値にする
		m_cursor	= detail::red_black_impl::impl<Traits>::right(m_cursor);
		++m_depth;
		while (detail::red_black_impl::impl<Traits>::left(m_cursor))
		{
			m_cursor	= detail::red_black_impl::impl<Traits>::left(m_cursor);
			++m_depth;
		}
	}
	else
	{
		// 親を返す
		for (;;)
		{
			auto	prev	= m_cursor;
			m_cursor	= detail::red_black_impl::impl<Traits>::parent(m_cursor);
			--m_depth;
			if (!m_cursor || !(key_compare{}(key(*m_cursor), key(*prev))))
			{
				break;
			}
		}
	}
	return	*this;
}


template<class Traits>
auto red_black::begin(root<Traits>& _root) noexcept -> iterator<Traits>
{
	return	detail::red_black_impl::impl<Traits>::begin(_root);
}


template<class Traits>
auto red_black::end(root<Traits>& _root) noexcept -> iterator<Traits>
{
	return	detail::red_black_impl::impl<Traits>::end(_root);
}


template<class Traits, class Key, class EqualsTo>
auto red_black::find(root<Traits>& _root, const Key& _key, EqualsTo _equale_to) -> iterator<Traits>
{
	return	detail::red_black_impl::impl<Traits>::find(_root, key(_key), _equale_to);
}


template<class Traits, class Key, class CreateNodeCallback>
auto red_black::insert(root<Traits>& _root, const Key& _key, CreateNodeCallback&& _factory) -> iterator<Traits>
{
	return	detail::red_black_impl::impl<Traits>::insert(_root, _key, _factory);
}


template<class Traits, class Key>
auto red_black::remove(root<Traits>& _root, const Key& _key) -> typename Traits::value_type*
{
	return	detail::red_black_impl::impl<Traits>::remove(_root, key(_key));
}

} }
#endif

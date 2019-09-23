/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libcurry__tree__hpp
#define	__libcurry__tree__hpp
#include <utility>
namespace tree {
template<class T>			const T& key		(const T& _value) noexcept { return _value; }
template<class K, class V>	const K& key		(const std::pair<K, V>& _pair) noexcept { return _pair.first; }
template<class T>			const T& mapped		(const T& _value) noexcept { return _value; }
template<class K, class V>	const K& mapped		(const std::pair<K, V>& _pair) noexcept { return _pair.second; }
}
#include "hash.hpp"
#include "tree/hash_node.hpp"
#include "tree/red_black.hpp"


// ==================================================================
// tree �͖؍\���������e���v���[�g�ł��B
// ------------------------------------------------------------------
// std::set, std::map, std::unsorted_set, std::unsorted_map �ɋ߂�
// �@�\��񋟂��܂��BSTL�Ƃ̈Ⴂ�́A���O�Ƀm�[�h�����܂񂾒l��
// �Ăяo�����Ŋm�ۂ��A��������̂܂ܓo�^�ł��邱�Ƃł��B
// ------------------------------------------------------------------
namespace tree {
using namespace algorithm;


// ==================================================================
// �؂������f�[�^�ɂ��Ă̓������`���܂��B
// ------------------------------------------------------------------
template<class T, class Compare, class Algorithm>
struct	basic_tree_traits
{
	using	algorithm			= Algorithm;
	using	value_type			= T;
	using	key_compare			= Compare;
	using	node_type			= typename algorithm::template node<basic_tree_traits<T, Compare, Algorithm>>;
	using	root_type			= typename algorithm::template root<basic_tree_traits<T, Compare, Algorithm>>;
	using	iterator			= typename algorithm::template iterator<basic_tree_traits<T, Compare, Algorithm>>;
	using	const_iterator		= typename algorithm::template const_iterator<basic_tree_traits<T, Compare, Algorithm>>;

	// ==============================================================
	// �A���S���Y�����g�p����m�[�h�������o��
	// --------------------------------------------------------------
	struct	to_node
	{
		node_type& operator () (value_type& _value) const noexcept { return _value; }
	};
};


// ==================================================================
// �؂������W���I�ȃm�[�h
// ------------------------------------------------------------------
// �L�[���������Ȃ��m�[�h�ł��B
// ------------------------------------------------------------------
template<class Value, class Compare, class Algorithm>
struct	basic_set_node
		: basic_tree_traits<basic_set_node<Value, Compare, Algorithm>, Compare, Algorithm>::node_type
		, Value
{
	using	algorithm	= Algorithm;
	using	key_type	= Value;
	using	mapped_type	= Value;
	using	value_type	= Value;
	using	node_type	= typename basic_tree_traits<basic_set_node<Value, Compare, Algorithm>, Compare, Algorithm>::node_type;
	using	key_compare	= Compare;
/*
	                        basic_set_node(){}
	                        basic_set_node(const basic_set_node&  _r) : value_type(          _r.value ), node_type(_r           ) {}
	                        basic_set_node(      basic_set_node&& _r) : value_type(std::move(_r.value)), node_type(std::move(_r)) {}
	template<class... Args> basic_set_node(Args&&... _args) : value_type(std::forward<Args>(_args)...) {}
*/
	operator       value_type& ()       noexcept { return *this; }
	operator const value_type& () const noexcept { return *this; }

	                  bool operator == (const key_type& _key) { return key(*this) == _key; }
	template<class T> bool operator == (const T       & _key) { return key(*this) == _key; }

	hash_t hash_code() const noexcept { return hash_code(key(*this)); }


	basic_set_node& operator = (const basic_set_node& ) = delete;
	basic_set_node& operator = (      basic_set_node&&) = delete;
};
template<class Value, class Compare, class Algorithm> auto key		(const basic_set_node<Value, Compare, Algorithm>& _value) noexcept -> decltype(key   (_value.value)) { return key   (_value.value); }
template<class Value, class Compare, class Algorithm> auto mapped	(      basic_set_node<Value, Compare, Algorithm>& _value) noexcept -> decltype(mapped(_value.value)) { return mapped(_value.value); }
template<class Value, class Compare, class Algorithm> auto mapped	(const basic_set_node<Value, Compare, Algorithm>& _value) noexcept -> decltype(mapped(_value.value)) { return mapped(_value.value); }



// ==================================================================
// �؂������W���I�ȃm�[�h
// ------------------------------------------------------------------
// std::pair ���g�����L�[�ƒl�����m�[�h�ł��B
// ------------------------------------------------------------------
template<class Key, class Value, class Compare, class Algorithm>
struct	basic_pair_node
		: basic_tree_traits<basic_pair_node<Key, Value, Compare, Algorithm>, Compare, Algorithm>::node_type
		, std::pair<Key, Value>
{
	using	algorithm	= Algorithm;
	using	key_type	= Key;
	using	mapped_type	= Value;
	using	value_type	= std::pair<Key, Value>;
	using	node_type	= typename basic_tree_traits<basic_pair_node<Key, Value, Compare, Algorithm>, Compare, Algorithm>::node_type;
	using	key_compare	= Compare;

	basic_pair_node(const basic_pair_node&  _r) : value_type(          _r ), node_type(          _r ) {}
	basic_pair_node(      basic_pair_node&& _r) : value_type(std::move(_r)), node_type(std::move(_r)) {}
	basic_pair_node(const value_type&       _v) : value_type(std::forward<const value_type>(_v)), node_type() {}
	basic_pair_node(      value_type&&      _v) : value_type(std::move                     (_v)), node_type() {}
	basic_pair_node(const key_type&  _k, const mapped_type&  _m) : value_type(_k, _m), node_type() {}
	basic_pair_node(      key_type&& _k,       mapped_type&& _m) : value_type(std::move(_k), std::move(_m)), node_type() {}

	operator       value_type& ()       noexcept { return static_cast<      value_type>(*this); }
	operator const value_type& () const noexcept { return static_cast<const value_type>(*this); }

	                  bool operator == (const key_type& _key) { return value_type::first == _key; }
	template<class T> bool operator == (const T       & _key) { return value_type::first == _key; }

	hash_t hash_code() const noexcept { return ::hash_code(key(static_cast<const value_type&>(*this))); }

	basic_pair_node& operator = (const basic_pair_node& ) = delete;
	basic_pair_node& operator = (      basic_pair_node&&) = delete;
};
template<class Key, class Value, class Compare, class Algorithm> auto key		(const basic_pair_node<Key, Value, Compare, Algorithm>& _value) noexcept -> decltype(key   (_value.first )) { return key   (_value.first ); }
template<class Key, class Value, class Compare, class Algorithm> auto mapped	(      basic_pair_node<Key, Value, Compare, Algorithm>& _value) noexcept -> decltype(mapped(_value.second)) { return mapped(_value.second); }
template<class Key, class Value, class Compare, class Algorithm> auto mapped	(const basic_pair_node<Key, Value, Compare, Algorithm>& _value) noexcept -> decltype(mapped(_value.second)) { return mapped(_value.second); }


// ==================================================================
// ���̃N���X�͖؍\���A���S���Y�����������߂̊��N���X�ł��B
// ------------------------------------------------------------------
// set��map�Ȃǂ̃N���X�̃x�[�X�ɂȂ�܂��B
// ------------------------------------------------------------------
template<class Node, class Allocator = std::allocator<Node>>
class	basic_tree
{
public:
	using	tree_type			= basic_tree<Node, Allocator>;
	using	algorithm			= typename Node::algorithm;
	using	key_type			= typename Node::key_type;
	using	mapped_type			= typename Node::mapped_type;
	using	value_type			= typename Node::value_type;
	using	key_compare			= typename Node::key_compare;
	using	size_type			= std::size_t;
	using	diffrence_type		= std::ptrdiff_t;
	using	node_type			= Node;
	using	tree_traits_type	= basic_tree_traits<Node, key_compare, algorithm>;
	using	root_type			= typename tree_traits_type::root_type;
	using	allocator_type		= typename Allocator::template rebind<node_type>::other;

protected:
	using	_node_allocator_type	= typename Allocator::template rebind<node_type>::other;

public:
	struct	iterator : protected tree_traits_type::iterator				// �C�e���[�^�̖��O��Z�k���邽�߂� using ���g�킸�ɐV���ɐ錾
	{
	private:
		using	base_iterator	= typename tree_traits_type::iterator;
	public:
		using	value_type		= typename tree_type::value_type;
		using	base_iterator::base_iterator;

		iterator() {}
		iterator(const iterator&  _s) : base_iterator(static_cast<const base_iterator&>(_s)) {}
		iterator(      iterator&& _s) : base_iterator(std::move(static_cast<base_iterator&&>(_s))) {}
		iterator(const base_iterator&  _s) : base_iterator(_s) {}
		iterator(      base_iterator&& _s) : base_iterator(std::move(_s)) {}
		value_type* operator -> () const noexcept { return &static_cast<value_type&>(base_iterator::operator * ()); }
		value_type& operator *  () const noexcept { return  base_iterator::operator * (); }
		bool operator == (const iterator& _r) const noexcept { return !base_iterator::operator != (_r); }
		bool operator != (const iterator& _r) const noexcept { return  base_iterator::operator != (_r); }
		iterator& operator ++ () { base_iterator::operator ++(); return *this; }
		iterator  operator ++ (int) { auto retval(*this); base_iterator::operator ++(); return retval; }
		iterator& operator = (const iterator&  _other){ base_iterator::operator = (_other); return *this; }
		iterator& operator = (      iterator&& _other){ base_iterator::operator = (_other); return *this; }
	};

	struct	const_iterator : protected tree_traits_type::const_iterator	// �C�e���[�^�̖��O��Z�k���邽�߂� using ���g�킸�ɐV���ɐ錾
	{
	private:
		using	base_iterator	= typename tree_traits_type::const_iterator;
	public:
		using	value_type		= typename tree_type::value_type;
		using	base_iterator::base_iterator;

		const_iterator() {}
		const_iterator(const const_iterator&  _s) : base_iterator(_s) {}
		const_iterator(      const_iterator&& _s) : base_iterator(std::move(_s)) {}
		const_iterator(const base_iterator&  _s) : base_iterator(_s) {}
		const_iterator(      base_iterator&& _s) : base_iterator(std::move(_s)) {}
		const value_type* operator -> () const noexcept { return &static_cast<const value_type&>(base_iterator::operator * ()); }
		const value_type& operator *  () const noexcept { return  base_iterator::operator * (); }
		bool operator == (const const_iterator& _r) const noexcept { return !base_iterator::operator != (_r); }
		bool operator != (const const_iterator& _r) const noexcept { return  base_iterator::operator != (_r); }
		const_iterator& operator ++ () { base_iterator::operator ++(); return *this; }
		const_iterator  operator ++ (int) { auto retval(*this); base_iterator::operator ++(); return retval; }
		const_iterator& operator = (const const_iterator&  _other) = default;
		const_iterator& operator = (      const_iterator&& _other) = default;
	};

#if 0
	using	reverse_iterator
	using	const_reverse_iterator
	using	node_type			= Node;
	using	insert_return_type
#endif


private:
	// =================================================================
	// find() �̃R�[���o�b�N�֐����L���Ȃ��̂����ׂ邽�߂̊֐��I�u�W�F�N�g
	// is_match_conditions<> �ƃZ�b�g�Ŏg�p����܂��B
	// -----------------------------------------------------------------
	struct	is_finder
	{
		template<class F> auto operator () (F _f) -> decltype(_f(std::declval<const_iterator>()), std::true_type{});
		template<class C> auto operator () (std::in_place_type_t<C>) -> decltype(&C::operator ());
		template<class C> auto operator () (bool (C::*_f)(const_iterator)const) -> std::true_type;		// lambda
		template<class C> auto operator () (bool (C::*_f)(const_iterator)     ) -> std::true_type;
		                  auto operator () (bool (   *_f)(const_iterator)     ) -> std::true_type;
	};


public:
	basic_tree(const allocator_type& _allocator = allocator_type());


	// =======================================================================
	// �C�e���[�^���擾���܂��B
	// -----------------------------------------------------------------------
	auto begin()        ->       iterator { return algorithm::begin(m_root); }
	auto end()          ->       iterator { return algorithm::end(m_root); }
	auto begin()  const -> const_iterator { return cbegin(); }
	auto end()    const -> const_iterator { return cend(); }
	auto cbegin() const -> const_iterator { return algorithm::begin(const_cast<root_type&>(m_root)); }
	auto cend()   const -> const_iterator { return algorithm::end(const_cast<root_type&>(m_root)); }


	// =======================================================================
	// _key ���������܂��B
	// -----------------------------------------------------------------------
	auto find(const key_type& _key) -> iterator;
	auto find(const key_type& _key) const -> const_iterator;

	// =======================================================================
	// _key ���������܂��B
	// -----------------------------------------------------------------------
	//	_hint �Ŏw�肳���q���g�����Ƀm�[�h���������܂��B
	//	RedBlack tree �Ȃ�L�[�AHashNode �Ȃ�n�b�V���l�ł��B
	//	�q���g�Ŏw�肵���m�[�h��������� _finder �Ŏw�肷��R�[���o�b�N��
	//	�Ăяo���܂��B�R�[���o�b�N�̒��Ńm�[�h���r���A��v�����ꍇ�̓R�[��
	//	�o�b�N�� true ��Ԃ��Ă��������B
	//	find() �̓R�[���o�b�N�� true ��Ԃ����m�[�h�̃C�e���[�^��Ԃ��܂��B
	// -----------------------------------------------------------------------
	template<class T, class Finder> auto find(const T& _hint, Finder _finder)       ->       iterator;
	template<class T, class Finder> auto find(const T& _hint, Finder _finder) const -> const_iterator;

	// =======================================================================
	// _key ���폜���܂��B
	// -----------------------------------------------------------------------
	//	�m�[�h�̓A���P�[�^�ɂ���č폜����܂��B
	//	insert(_key, _factory) �ɂ���ēƎ��Ɋm�ۂ�����������ǉ������ꍇ�A
	//	erase() �ł͂Ȃ��Aremove ��p���ăm�[�h����O���A�����ō폜���Ă��������B
	// -----------------------------------------------------------------------
	auto erase(const key_type& _key) -> size_type;

	// =======================================================================
	// �m�[�h��ǉ����܂��B
	// -----------------------------------------------------------------------
	//	key(_value) �Ŏ擾�ł���L�[���������A��v����m�[�h��������Ȃ��ꍇ
	//	�A���P�[�^�ɂ���ă��������m�ۂ��Ēǉ����܂��B
	//	���łɓ���̃m�[�h���o�^�ς݂̏ꍇ���������A�o�^�ς݂̃m�[�h������
	//	�C�e���[�^��Ԃ��܂��B
	//	���̃��\�b�h�Œǉ����ꂽ�m�[�h�� erase() �ō폜�ł��܂��B
	// -----------------------------------------------------------------------
	auto insert(const value_type&  _value) -> std::pair<iterator, bool>;
	auto insert(      value_type&& _value) -> std::pair<iterator, bool>;

	template<class _Iterator>
	auto insert(_Iterator _first, _Iterator _last) -> void;

	// =======================================================================
	// _key ���V�����v�f�ł���΁A�R�[���o�b�N���Ăяo���A�m�[�h��}�����܂��B
	// -----------------------------------------------------------------------
	// _key �Ŏw�肷��m�[�h���������A������Ȃ��i�m�[�h�̗t�܂œ��B�����j
	// �ꍇ�̓R�[���o�b�N���Ăяo���A�R�[���o�b�N���Ԃ����m�[�h��V����
	// �m�[�h�Ƃ��Ė؂ɒǉ����܂��B
	// -----------------------------------------------------------------------
	template<class T, class Factory>
	auto _insert(const T& _value, Factory _factory) -> std::pair<iterator, bool>;

	// =======================================================================
	// _key ����菜���܂��B
	// -----------------------------------------------------------------------
	//	���̃��\�b�h�͎w�肵���L�[�̒l����菜���܂����A�I�u�W�F�N�g�̍폜��
	//	�s���܂���B
	// -----------------------------------------------------------------------
	auto remove(const key_type& _key) -> node_type*;

	// =======================================================================
	// �������Ă���L�[�̐����擾���܂��B
	// -----------------------------------------------------------------------
	size_type size() const noexcept { return m_size; }


	allocator_type& get_allocator() noexcept { return m_allocator; }

protected:
	allocator_type	m_allocator;
	size_type		m_size;
	root_type		m_root;
};




// ==================================================================
// Red Black Tree �ɂ��؍\����񋟂��܂��B
// ------------------------------------------------------------------
// ------------------------------------------------------------------
template<class Value, class Compare, class Allocator = std::allocator<Value>>
class	rbset
		: public	basic_tree
					<
						basic_set_node<Value, Compare, tree::algorithm::red_black>,
						Allocator
					>
{};


// ==================================================================
// Red Black Tree �ɂ��؍\����񋟂��܂��B
// ------------------------------------------------------------------
// ------------------------------------------------------------------
template<class Key, class Value, class Compare = std::less<Key>, class Allocator = std::allocator<Value>>
class	rbmap
		: public	basic_tree
					<
						
						basic_pair_node<Key, Value, Compare, tree::algorithm::red_black>,
						Allocator
					>
{};



// ==================================================================
// Hash Node Tree �ɂ��؍\����񋟂��܂��B
// ------------------------------------------------------------------
// ------------------------------------------------------------------
template<class Value, class Compare = std::equal_to<Value>, class Allocator = std::allocator<Value>>
class	hnset
		: public	basic_tree
					<
						basic_set_node<Value, Compare, tree::algorithm::hash_node<2>>,
						Allocator
					>
{};


// ==================================================================
// Hash Node Tree �ɂ��؍\����񋟂��܂��B
// ------------------------------------------------------------------
// ------------------------------------------------------------------
template<class Key, class Value, class Compare = std::equal_to<Key>, class Allocator = std::allocator<Value>>
class	hnmap
		: public	basic_tree
					<
						basic_pair_node<Key, Value, Compare, tree::algorithm::hash_node<2>>,
						Allocator
					>
{};



// ==================================================================
// std::set �݊�
// ------------------------------------------------------------------
template<class Value, class Compare = std::less<Value>, class Allocator = std::allocator<Value>>
using	basic_set			= rbset<Value, Allocator>;

// ==================================================================
// std::map �݊�
// ------------------------------------------------------------------
template<class Key, class Value, class Compare = std::less<Key>, class Allocator = std::allocator<Value>>
using	basic_map			= rbmap<Key, Value, Compare, Allocator>;

// ==================================================================
// std::unsorted_set �݊�
// ------------------------------------------------------------------
template<class Value, class Allocator = std::allocator<Value>>
using	basic_unordered_set	= hnset<Value, Allocator>;

// ==================================================================
// std::unsorted_map �݊�
// ------------------------------------------------------------------
template<class Key, class Value, class Allocator = std::allocator<Value>>
using	basic_unordered_map	= hnmap<Key, Value, Allocator>;





template<class Value, class Allocator>
basic_tree<Value, Allocator>::basic_tree(const allocator_type& _allocator)
	: m_allocator(_allocator)
	, m_size(0)
{
}


template<class Value, class Allocator>
auto basic_tree<Value, Allocator>::find(const key_type& _key) -> iterator
{
	return	find(_key, [&_key](const_iterator _node) -> bool
			{
				return	key(*_node) == key(_key);
			});
}


template<class Value, class Allocator>
auto basic_tree<Value, Allocator>::find(const key_type& _key) const -> const_iterator
{
	return	find(_key, [&_key](const_iterator _node) -> bool
			{
				return	key(*_node) == _key;
			});
}


template<class Value, class Allocator>
template<class T, class Finder>
auto basic_tree<Value, Allocator>::find(const T& _hint, Finder _finder) -> iterator
{
	static_assert(is_match_conditions<Finder, is_finder>::value, "Compare is not bool (cons_iterator).");
	return	algorithm::find(m_root, _hint, _finder);
}


template<class Value, class Allocator>
template<class T, class Finder>
auto basic_tree<Value, Allocator>::find(const T& _hint, Finder _finder) const -> const_iterator
{
	static_assert(is_match_conditions<Finder, is_finder>::value, "Compare is not bool (cons_iterator).");
	return	algorithm::find(const_cast<root_type&>(m_root), _hint, _finder);
}


template<class Value, class Allocator>
auto basic_tree<Value, Allocator>::erase(const key_type& _key) -> size_type
{
	auto*	node	= remove(key(_key));
	if (node)
	{
		_node_allocator_type(get_allocator()).destroy(node);
		_node_allocator_type(get_allocator()).deallocate(node, 1);
		return	1;
	}
	return	0;
}


template<class Value, class Allocator>
auto basic_tree<Value, Allocator>::insert(value_type&& _value) -> std::pair<iterator, bool>
{
	return	_insert(key(const_cast<const value_type&>(_value)), [this, &_value]() -> node_type*
			{
				_node_allocator_type	allocator(get_allocator());
				node_type*	retval	= allocator.allocate(1);
				allocator.construct(retval, std::forward<value_type>(_value));
				return	retval;
			});
}


template<class Value, class Allocator>
auto basic_tree<Value, Allocator>::insert(const value_type& _value) -> std::pair<iterator, bool>
{
	return	_insert(key(_value), [this, &_value]() -> node_type*
			{
				_node_allocator_type	allocator(get_allocator());
				node_type*	retval	= allocator.allocate(1);
				allocator.construct(retval, _value);
				return	retval;
			});
}


template<class Value, class Allocator>
template<class _Iterator>
auto basic_tree<Value, Allocator>::insert(_Iterator _first, _Iterator _last) -> void
{
	for (auto it = _first; it != _last; ++it)
	{
		insert(*it);
	}
}


template<class Value, class Allocator>
template<class T, class Factory>
auto basic_tree<Value, Allocator>::_insert(const T& _value, Factory _factory) -> std::pair<iterator, bool>
{
	bool		isCreate	= false;
	iterator	it			= algorithm::insert(m_root, _value, [&_factory, &isCreate]() -> node_type*
							{
								isCreate	= true;
								return	_factory();
							});
	if (isCreate)
	{
		++m_size;
	}
	return	{ it, isCreate };
}


template<class Value, class Allocator>
auto basic_tree<Value, Allocator>::remove(const key_type& _key) -> node_type*
{
	auto*	rval = algorithm::remove(m_root, _key);
	if (rval)
	{
		--m_size;
	}
	return	rval;
}


}
#endif

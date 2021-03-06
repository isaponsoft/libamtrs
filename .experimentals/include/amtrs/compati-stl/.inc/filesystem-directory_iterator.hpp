﻿/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__filesystem__directory_iterator__hpp
#define	__libamtrs__filesystem__directory_iterator__hpp
#include <memory>
#include <stack>
#include <system_error>
#include <string_view>
#include "types.hpp"
#include "directory_entry.hpp"
#include "filesystem_error.hpp"
AMTRS_STD_FILESYSTEM__NAMESPACE_BEGIN

// Prototype. Not use.
template<class DirEntryT>    class basic_directory_iterator;
template<class DirIteratorT> class basic_recursive_directory_iterator;


using		directory_iterator				= basic_directory_iterator<directory_entry>;
using		recursive_directory_iterator	= basic_recursive_directory_iterator<directory_iterator>;

// ==================================================================
//!	@brief	ディレクトリイテレータ。
//!			http://en.cppreference.com/w/cpp/filesystem/basic_directory_iterator
// ------------------------------------------------------------------
template<class DirEntryT>
class	basic_directory_iterator
{
public:
	using	this_type			= basic_directory_iterator<DirEntryT>;
	using	value_type			= DirEntryT;
	using	difference_type		= std::ptrdiff_t;
	using	pointer 			= const value_type*;
	using	reference 			= const value_type&;
	using	iterator_category	= std::input_iterator_tag;
	using	path_type			= typename value_type::path_type;

	~basic_directory_iterator() = default;

	// コンストラクタ
	         basic_directory_iterator() {}
	explicit basic_directory_iterator(const path_type& _path                      ) : basic_directory_iterator(_path, error::make<path_type>(   )) {}
	         basic_directory_iterator(const path_type& _path, std::error_code& _ec) : basic_directory_iterator(_path, error::make<path_type>(_ec)) {}
	         basic_directory_iterator(const basic_directory_iterator& ) = default;
	         basic_directory_iterator(      basic_directory_iterator&&) = default;

	basic_directory_iterator& operator = (const basic_directory_iterator& ) = default;
	basic_directory_iterator& operator = (      basic_directory_iterator&&) = default;

	const value_type& operator *  () const { return  m_entry; }
	const value_type* operator -> () const { return &m_entry; }

	basic_directory_iterator& operator++ ()
	{
		if (m_instance->next())
		{
			m_instance->set(m_entry);
		}
		else
		{
			m_instance.reset();
		}
		return	*this;
	}

	basic_directory_iterator& increment(std::error_code& ec) noexcept;

	bool operator == (const this_type& _rhs) const noexcept
	{
		return	m_instance == _rhs.m_instance;
	}

	bool operator != (const this_type& _rhs) const noexcept
	{
		return	!(*this == _rhs);
	}

protected:
	template<class ErrorType>
	basic_directory_iterator(const path_type& _path, ErrorType _error)
	{
		std::shared_ptr<instance>	inst	= std::make_shared<instance>(_path);
		if (inst->open(_error))
		{
			inst->set(m_entry);
			m_instance = std::move(inst);
		}
	}

private:
	class	instance;

	std::shared_ptr<instance>	m_instance;
	value_type					m_entry;

	template<class _DirEntryT>
	friend	bool operator == (const basic_directory_iterator<_DirEntryT>& __lhs, const basic_directory_iterator<_DirEntryT>& __rhs) noexcept;
};




// ==================================================================
//!	@brief	再帰的なディレクトリイテレータ。
//!			http://en.cppreference.com/w/cpp/filesystem/recursive_directory_iterator
// ------------------------------------------------------------------
template<class DirIteratorT>
class	basic_recursive_directory_iterator
{
public:
	using	this_type			= basic_recursive_directory_iterator<DirIteratorT>;
	using	iterator_type		= DirIteratorT;
	using	value_type			= typename iterator_type::value_type;
	using	difference_type		= std::ptrdiff_t;
	using	pointer 			= const value_type*;
	using	reference 			= const value_type&;
	using	iterator_category	= std::input_iterator_tag;
	using	path_type			= typename value_type::path_type;

	basic_recursive_directory_iterator() {}
	explicit basic_recursive_directory_iterator(const path_type& _path)
	{
		m_instance = std::make_shared<instance>();
		m_instance->stack.push(iterator_type{_path});
	}

	basic_recursive_directory_iterator(const path_type& _path, std::error_code& ec );
	basic_recursive_directory_iterator(const basic_recursive_directory_iterator& ) = default;
	basic_recursive_directory_iterator(      basic_recursive_directory_iterator&&) = default;
	~basic_recursive_directory_iterator() = default;
	basic_recursive_directory_iterator& operator = (const basic_recursive_directory_iterator& ) = default;
	basic_recursive_directory_iterator& operator = (      basic_recursive_directory_iterator&&) = default;

	const value_type& operator *  () const { return  *(m_instance->stack.top()); }
	const value_type* operator -> () const { return &*(m_instance->stack.top()); }

	this_type& operator++ ()
	{
		auto&		it			= m_instance->stack.top();
		path_type	root;
		if (it->is_directory())
		{
			root	= it->path();
		}
		++it;
		if (it == end(it))
		{
			m_instance->stack.pop();
		}
		if (!root.empty())
		{
			auto	e = iterator_type{root};
			if (e != end(e))
			{
				m_instance->stack.push(std::move(e));
			}
		}
		if (m_instance->stack.size() == 0)
		{
			m_instance.reset();
		}
		return	*this;
	}

	this_type& increment(std::error_code& ec);

	std::size_t depth() const { return m_instance->stack.size(); }


private:
	struct	instance
	{
		std::stack<iterator_type>	stack;
	};

	std::shared_ptr<instance>	m_instance;

	template<class _DirEntryT>
	friend	bool operator == (const basic_recursive_directory_iterator<_DirEntryT>& __lhs, const basic_recursive_directory_iterator<_DirEntryT>& __rhs) noexcept;
};


template<class DirEntryT>
inline basic_directory_iterator<DirEntryT> begin(basic_directory_iterator<DirEntryT> _it) noexcept
{
	return	_it;
}

template<class DirEntryT>
inline basic_directory_iterator<DirEntryT> end(basic_directory_iterator<DirEntryT> _it) noexcept
{
	return	{};
}

template<class DirIteratorT>
inline basic_recursive_directory_iterator<DirIteratorT> begin(basic_recursive_directory_iterator<DirIteratorT> _it) noexcept
{
	return	_it;
}

template<class DirIteratorT>
inline basic_recursive_directory_iterator<DirIteratorT> end(basic_recursive_directory_iterator<DirIteratorT> _it) noexcept
{
	return	{};
}

template<class DirEntryT>
inline bool operator == (const basic_directory_iterator<DirEntryT>& __lhs, const basic_directory_iterator<DirEntryT>& __rhs) noexcept
{
	return	__lhs.m_instance == __rhs.m_instance;
}

template<class DirEntryT>
inline bool operator != (const basic_directory_iterator<DirEntryT>& __lhs, const basic_directory_iterator<DirEntryT>& __rhs) noexcept
{
	return	!(__lhs == __rhs);
}


template<class DirIteratorT>
inline bool operator == (const basic_recursive_directory_iterator<DirIteratorT>& __lhs, const basic_recursive_directory_iterator<DirIteratorT>& __rhs) noexcept
{
	return	__lhs.m_instance == __rhs.m_instance;
}

template<class DirIteratorT>
inline bool operator != (const basic_recursive_directory_iterator<DirIteratorT>& __lhs, const basic_recursive_directory_iterator<DirIteratorT>& __rhs) noexcept
{
	return	!(__lhs == __rhs);
}


AMTRS_STD_FILESYSTEM__NAMESPACE_END
#if		AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_WIN32
#include "directory_iterator-win32.hpp"
#else
#include "directory_iterator-posix.hpp"
#endif
#endif

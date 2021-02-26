/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__hashnode__hash_node_traits__hpp
#define	__libamtrs__hashnode__hash_node_traits__hpp
#include <utility>
#include "def.hpp"
AMTRS_NAMESPACE_BEGIN




template<class T>
class	hash_node_tree_traits
{
public:
	static constexpr int	node_size	= 4;

	using	value_type		= T;
	using	key_type		= T;
	using	mapped_type		= T;

	static key_type const& key(value_type const& _val) noexcept
	{
		return	_val;
	}
};



template<class K, class V>
class	hash_node_tree_traits<std::pair<K, V>>
{
public:
	static constexpr int	node_size	= 4;

	using	value_type		= std::pair<K, V>;
	using	key_type		= K;
	using	mapped_type		= V;


	static key_type const& key(value_type const& _val) noexcept
	{
		return	_val.first;
	}

};


AMTRS_NAMESPACE_END
#endif

/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__component__name_component__hpp
#define	__libamtrs__node__component__name_component__hpp
AMTRS_NAMESPACE_BEGIN namespace component {


template<class NodeT>
struct	name_traits
{
	using	name_type	= std::string;
};


template<class NodeT, class Traits = name_traits<NodeT>>
class	name_component
{
public:
	using	name_type	= typename Traits::name_type;

	void set_name(name_type _name)
	{
		mName	= std::move(_name);
	}

	const name_type& name() const noexcept
	{
		return	mName;
	}

	template<class S>
	NodeT* find_by_name(S _name) const noexcept
	{
		std::basic_string_view<typename name_type::value_type>	n(_name);
		NodeT*													r	= nullptr;
		static_cast<NodeT const*>(this)->visit_children([&](auto c)
		{
			if (c->name() == n)
			{
				r	= c;
				return	true;
			}
			return	false;
		}, false);
		return	r;
	}

private:
	name_type		mName;
};



} AMTRS_NAMESPACE_END
#endif

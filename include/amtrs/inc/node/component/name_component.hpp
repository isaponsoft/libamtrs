/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__node__component__name_component__hpp
#define	__libamtrs__node__component__name_component__hpp
AMTRS_NAMESPACE_BEGIN namespace component {

template<class Node, class NameT>
class	name_component
{
public:
	using	name_type	= NameT;

	template<class N>
	void set_name(N _name)
	{
		mName	= _name;
	}

	const name_type& name() const noexcept
	{
		return	mName;
	}

private:
	name_type		mName;
};


} AMTRS_NAMESPACE_END
#endif

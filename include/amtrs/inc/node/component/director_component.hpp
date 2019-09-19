/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__node__component__director_component__hpp
#define	__libamtrs__node__component__director_component__hpp
#include <map>
#include <string>
#include "../../../g3d.hpp"
AMTRS_NAMESPACE_BEGIN namespace component {

template<class Node, class DirectorT>
class	director_component
{
public:
	using	director_type	= DirectorT;

	const director_type* get_director() const noexcept
	{
		return	mDirector;
	}

	const director_type* set_director(const director_type* _director) noexcept
	{
		mDirector	= _director;
		return	_director;
	}

	static director_type* get_default_director()
	{
		return	default_director();
	}

	static void set_default_director(director_type* _director)
	{
		default_director()	= _director;
	}

private:
	static director_type*& default_director()
	{
		static director_type*	_	= nullptr;
		return	_;
	}

	ref<const director_type>	mDirector;
};


} AMTRS_NAMESPACE_END
#endif

/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__graphics__g3d__realizer__hpp
#define	__libamtrs__graphics__g3d__realizer__hpp
#include "type.hpp"
AMTRS_G3D_NAMESPACE_BEGIN

template<class T, class R>
struct	realizer
{
	using	instance_type	= typename T::instance_type;
	using	realized_type	= R;

	template<class... Args>
	ref<realized_type> operator () (T& _obj, Args&&... _args)
	{
		if (_obj.mInstance.empty())
		{
			return	{};
		}
		if (_obj.mInstance->realized)
		{
			return	static_cast<R*>(_obj.mInstance->realized.get());
		}
		ref<realized_type>	r	= new realized_type();
		r->initialize(_obj.mInstance.get(), std::forward<Args>(_args)...);
		_obj.mInstance->realized	= r;
		return	r;
	}
};


template<class Traits, class T, class... Args>
auto realizing(T& _obj, Args&&... _args)
{
	using	realizer_type	= realizer<T, Traits>;
	realizer_type	r;
	return	r(_obj, std::forward<Args>(_args)...);
}


AMTRS_G3D_NAMESPACE_END
#endif

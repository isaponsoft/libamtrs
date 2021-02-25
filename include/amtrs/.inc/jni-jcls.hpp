/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__java__jcls__hpp
#define	__libamtrs__java__jcls__hpp
AMTRS_JAVA_NAMESPACE_BEGIN


template<class T>
struct	jcls : T::statics
{
	using	super_type	= typename T::statics;
	using	handle_type	= typename super_type::handle_type;

	using	super_type::super_type;
	using	super_type::operator =;

	explicit operator handle_type () const noexcept { return super_type::get(); }
};



AMTRS_JAVA_NAMESPACE_END
#endif

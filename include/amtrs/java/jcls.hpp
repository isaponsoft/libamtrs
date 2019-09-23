#ifndef	__libamtrs__java__jcls__hpp
#define	__libamtrs__java__jcls__hpp
#include "def.hpp"
#include "ref.hpp"
#include "call_init.hpp"
#include "call_method.hpp"
#include "new_object.hpp"
#include "jni_traits.hpp"
AMTRS_JAVA_NAMESPACE_BEGIN


template<class T>
struct	jcls : T::statics
{
	using	_base_type	= typename T::statics;
	using	handle_type	= typename _base_type::handle_type;
	using	_base_type::_base_type;
	using	_base_type::operator =;
	explicit operator handle_type () const noexcept { return _base_type::get(); }
};



AMTRS_JAVA_NAMESPACE_END
#endif

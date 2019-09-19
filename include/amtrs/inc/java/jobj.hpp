#ifndef	__libamtrs__java__jobj__hpp
#define	__libamtrs__java__jobj__hpp
#include "def.hpp"
#include "ref.hpp"
#include "jni_traits.hpp"
AMTRS_JAVA_NAMESPACE_BEGIN



template<class T>
struct	jobj : T::dynamics
{
	using	_base_type	= typename T::dynamics;
	using	handle_type	= typename _base_type::handle_type;
	using	ref_type	= typename _base_type::ref_type;
	using	_base_type::_base_type;
	using	_base_type::operator =;
	explicit operator handle_type () const noexcept { return ref_type::get(); }
};




template<class T>
struct	argv<jobj<T>>
{
	using	handle_type	= typename jobj<T>::handle_type;

	//constexpr argv(handle_type _o) : obj(_o) {}
	constexpr argv(const jobj<T>& _o) : obj(_o.get()) {}
//	constexpr argv(jobj<T> _o) : obj(_o.get()) {}
	constexpr operator handle_type () const noexcept { return obj; }

	handle_type	obj;
};





AMTRS_JAVA_NAMESPACE_END
#endif

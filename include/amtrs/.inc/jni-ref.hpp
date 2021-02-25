/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__java__ref__hpp
#define	__libamtrs__java__ref__hpp
AMTRS_JAVA_NAMESPACE_BEGIN


template<class T> using localref	= basic_ref<T, lref<T>>;
template<class T> using globalref	= basic_ref<T, gref<T>>;


using	local_obj		= localref<jobject>;
using	global_obj		= globalref<jobject>;
using	local_cls		= localref<jclass>;
using	global_cls		= globalref<jclass>;
using	local_string	= localref<jstring>;
using	global_string	= globalref<jstring>;




template<class JniT>
class	lref
{
public:
	using	value_type	= typename std::remove_pointer<JniT>::type;
	using	pointer		= JniT;
	using	reference	= value_type&;
	using	object_type	= value_type*;


	static constexpr object_type construct(lref const& _traits)
	{
		return	nullptr;
	}

	template<class RTraits, class RPtr> static constexpr object_type copy(lref const& _traits, RTraits const& _rtaits, RPtr const& _ptr)
	{
		return	(pointer)get_jnienv()->NewLocalRef(static_cast<pointer>(_ptr));
	}

	template<class RTraits, class RPointer>
	static constexpr object_type move(lref const& _traits, lref<RTraits>, RPointer _ptr)
	{
		return	static_cast<pointer>(_ptr);
	}

	template<class RTraits, class RPointer>
	static constexpr object_type move(lref const& _traits, gref<RTraits>, RPointer _ptr)
	{
		object_type	r	= (pointer)get_jnienv()->NewLocalRef(static_cast<pointer>(_ptr));
		get_jnienv()->DeleteGlobalRef((pointer)_ptr);
		return	r;
	}

	constexpr void destruct(pointer _ptr)
	{
		get_jnienv()->DeleteLocalRef(_ptr);
	}

	static constexpr bool empty(object_type const& _o)
	{
		return	_o == nullptr;
	}
};



template<class JniT>
class	gref
{
public:
	using	value_type	= typename std::remove_pointer<JniT>::type;
	using	pointer		= JniT;
	using	reference	= value_type&;
	using	object_type	= value_type*;

	static constexpr object_type construct(gref const& _traits)
	{
		return	nullptr;
	}

	template<class RTraits, class RPtr> static constexpr object_type copy(gref const& _traits, RTraits const& _rtaits, RPtr const& _ptr)
	{
		return	(pointer)get_jnienv()->NewGlobalRef(static_cast<pointer>(_ptr));
	}

	template<class RTraits, class RPointer>
	static constexpr object_type move(gref const& _traits, gref<RTraits>, RPointer _ptr)
	{
		return	static_cast<pointer>(_ptr);
	}

	template<class RTraits, class RPointer>
	static constexpr object_type move(gref const& _traits, lref<RTraits>, RPointer _ptr)
	{
		object_type	r	= (pointer)get_jnienv()->NewGlobalRef(static_cast<pointer>(_ptr));
		get_jnienv()->DeleteLocalRef((pointer)_ptr);
		return	r;
	}


	constexpr void destruct(pointer _ptr)
	{
		get_jnienv()->DeleteGlobalRef(_ptr);
	}

	static constexpr bool empty(object_type const& _o)
	{
		return	_o == nullptr;
	}
};



AMTRS_JAVA_NAMESPACE_END
#endif

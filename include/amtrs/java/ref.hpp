#ifndef	__libamtrs__java__ref__hpp
#define	__libamtrs__java__ref__hpp
#include <jni.h>
#include "def.hpp"
#include "jnienv.hpp"

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

	constexpr lref(pointer _ptr = nullptr)
	{
		if (_ptr)
		{
			p	= (pointer)get_jnienv()->NewLocalRef(_ptr);
		}
	}

	constexpr lref(const lref& _ptr)
	{
		if (_ptr.p)
		{
			p	= (pointer)get_jnienv()->NewLocalRef(_ptr.p);
		}
	}

	constexpr lref(lref&& _ptr)
		: p(_ptr.p)
	{
		_ptr.p	= nullptr;
	}

	~lref()
	{
		if (p)
		{
			get_jnienv()->DeleteLocalRef(p);
			p	= nullptr;
		}
	}

	constexpr pointer get() const noexcept
	{
		return	p;
	}

	constexpr void set(pointer _ptr) noexcept
	{
		p	= _ptr;
	}

	pointer release() const noexcept
	{
		pointer	retval(p);
		p	= nullptr;
		return	retval;
	}

	constexpr bool empty() const noexcept { return p == nullptr; }
	constexpr void clear() noexcept { p = nullptr; }
	constexpr bool operator == (pointer _ptr) const noexcept { return p == _ptr; }
	constexpr bool operator == (const lref& _ptr) const noexcept { return p == _ptr.p; }

private:
	pointer	p	= nullptr;
};



template<class JniT>
class	gref
{
public:
	using	value_type	= typename std::remove_pointer<JniT>::type;
	using	pointer		= JniT;
	using	reference	= value_type&;

	constexpr gref() : p(nullptr) {}

	constexpr gref(pointer _ptr)
		: p(nullptr)
	{
		if (_ptr)
		{
			p	= (pointer)get_jnienv()->NewGlobalRef(_ptr);
		}
	}

	constexpr gref(const gref& _ptr)
		: p(nullptr)
	{
		if (_ptr.p)
		{
			p	= (pointer)get_jnienv()->NewGlobalRef(_ptr.p);
		}
	}

	constexpr gref(gref&& _ptr)
		: p(_ptr.p)
	{
		_ptr.p	= nullptr;
	}


	~gref()
	{
		if (p)
		{
			get_jnienv()->DeleteGlobalRef(p);
			p	= nullptr;
		}
	}

	constexpr pointer get() const noexcept
	{
		return	p;
	}

	constexpr void set(pointer _ptr) noexcept
	{
		p	= _ptr;
	}

	pointer release() const noexcept
	{
		pointer	retval(p);
		p	= nullptr;
		return	retval;
	}

	constexpr bool empty() const noexcept { return p == nullptr; }
	constexpr void clear() noexcept { p = nullptr; }
	constexpr bool operator == (pointer _ptr) const noexcept { return p == _ptr; }
	constexpr bool operator == (const gref& _ptr) const noexcept { return p == _ptr.p; }

private:
	pointer	p;
};



AMTRS_JAVA_NAMESPACE_END
#endif

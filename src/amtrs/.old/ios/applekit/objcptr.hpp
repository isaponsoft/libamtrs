/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__driver__ios__objcptr__hpp
#define	__libamtrs__driver__ios__objcptr__hpp
#include "../../../amtrs.hpp"
AMTRS_NAMESPACE_BEGIN
template<class T>
class	objcptr
{
public:
	objcptr() = default;

	objcptr(T* _obj)
		: mObj(_obj)
	{
		if (mObj)
		{
			[mObj retain];
		}
	}

	objcptr(objcptr const& _r)
		: mObj(_r.get())
	{}

	objcptr(objcptr&& _r)
		: mObj(_r.mObj)
	{
		_r.mObj = nullptr;
	}

	~objcptr()
	{
		clear();
	}

	objcptr& operator = (objcptr const& _r)
	{
		if (mObj != _r.mObj)
		{
			clear();
			mObj	= _r.mObj;
			if (mObj)
			{
				[mObj retain];
			}
		}
		return	*this;
	}

	objcptr& operator = (objcptr&& _r)
	{
		if (mObj != _r.mObj)
		{
			clear();
			mObj	= _r.mObj;
			_r.mObj	= nullptr;
		}
		return	*this;
	}

	operator T* () const noexcept
	{
		return	mObj;
	}

	T* get() const noexcept
	{
		return	mObj;
	}

	void clear()
	{
		if (mObj)
		{
			[mObj release];
			mObj	= nullptr;
		}
	}

protected:
	T*	mObj	= nullptr;
};
AMTRS_NAMESPACE_END
#endif

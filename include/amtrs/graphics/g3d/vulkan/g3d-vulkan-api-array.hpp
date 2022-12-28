/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__g3d__vulkan__array__hpp
#define	__libamtrs__g3d__vulkan__array__hpp
AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN

template<class T>
class	vkarray
{
public:
	using	value_type		= T;
	using	pointer			= T*;
	using	const_pointer	= T const*;
	using	reference		= T&;
	using	const_reference	= T const&;
	using	size_type		= size_t;

	vkarray() = default;

	vkarray(size_type _size)
	{
		if (_size > 0)
		{
			mData	= new value_type[_size];
			mSize	= _size;
		}
	}

	vkarray(vkarray const& _r)
	{
		if (!_r.empty())
		{
			mData	= new value_type[_r.size()];
			mSize	= _r.size();
			for (size_t i = 0; i < mSize; ++i)
			{
				mData[i] = _r.mData[i];
			}
		}
	}

	vkarray(vkarray&& _r)
		: mSize(_r.mSize)
		, mData(_r.mData)
	{
		_r.mSize	= 0;
		_r.mData	= nullptr;
	}

	vkarray(std::initializer_list<T> _init)
	{
		auto	size	= std::distance(_init.begin(), _init.end());
		if (size > 0)
		{
			mData	= new value_type[size];
			mSize	= size;
			std::copy(_init.begin(), _init.end(), mData);
		}
	}

	~vkarray()
	{
		clear();
	}

	vkarray& operator = (vkarray const& _r)
	{
		clear();
		new (this) vkarray(_r);
		return	*this;
	}

	vkarray& operator = (vkarray&& _r)
	{
		clear();
		new (this) vkarray(_r);
		return	*this;
	}

	pointer begin() noexcept { return mData; }
	pointer end() noexcept { return mData + mSize; }
	const_pointer cbegin() noexcept { return mData; }
	const_pointer cend() noexcept { return mData + mSize; }
	const_pointer begin() const noexcept { return mData; }
	const_pointer end() const noexcept { return mData + mSize; }


	reference front() noexcept { return mData[0]; }
	reference back() noexcept { return mData[mSize-1]; }
	const_reference front() const noexcept { return mData[0]; }
	const_reference back() const noexcept { return mData[mSize-1]; }

	reference operator [] (int _i) noexcept { return mData[_i]; }
	const_reference operator [] (int _i) const noexcept { return mData[_i]; }

	void clear()
	{
		delete[] mData;
		mSize	= 0;
		mData	= nullptr;
	}

	bool empty() const noexcept { return mSize == 0; }
	auto size() const noexcept { return mSize; }

	      pointer data()       noexcept { return mData; }
	const_pointer data() const noexcept { return mData; }

protected:
	size_type		mSize	= 0;
	value_type*		mData	= nullptr;
};



AMTRS_G3D_VKUTIL_NAMESPACE_END
#endif

/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__graphics__view2d__hpp
#define	__libamtrs__graphics__view2d__hpp
#include <iosfwd>
#include <type_traits>
#include "geometory/vec.hpp"
#include "geometory/size.hpp"
AMTRS_NAMESPACE_BEGIN
#if	0
	view2d<uint32_t>	v({256, 256}, buff);

	v[{0, 0}]	= 1;
	v[0][0]		= 1;
	v[0].data();
#endif


template<class T>			class	view2d;
template<class T, class O>	class	view2d_iterator;



template<class T>
class	view2d
{
public:
	using	value_type				= T;
	using	position2_type			= graph::vec2<int>;
	using	size2_type				= graph::size2<int>;
	using	pitch_type				= graph::vec2<int>;
	using	iterator				= view2d_iterator<value_type, view2d<T>>;
	using	const_iterator			= view2d_iterator<typename std::add_const<value_type>::type, view2d<T>>;
	using	reverse_iterator		= view2d_iterator<value_type, view2d<T>>;
	using	const_reverse_iterator	= view2d_iterator<typename std::add_const<value_type>::type, view2d<T>>;
	using	pointer					= value_type*;
	using	reference				= value_type&;

	constexpr view2d() = default;
	constexpr view2d(view2d const&) = default;
	constexpr view2d& operator = (view2d const&) = default;
	constexpr view2d(size2_type _size, value_type* _buff, pitch_type _pitch = {1, 0});

	constexpr auto operator [] (position2_type _pos) const noexcept -> reference { return mBuff[mPitch.y * _pos.y + mPitch.x * _pos.x]; }
	constexpr auto operator [] (int _index) const noexcept -> reference { return mBuff[_index]; }


	constexpr auto size2() const noexcept -> size2_type { return mSize; }
	constexpr auto pitch() const noexcept -> pitch_type { return mPitch; }

	constexpr auto begin()       noexcept -> iterator       { return {this, {0,0}, {1,0}}; }
	constexpr auto begin() const noexcept -> const_iterator { return {this, {0,0}, {1,0}}; }
	constexpr auto end()         noexcept -> iterator       { return {this, {0,mSize.height}, {1,0}}; }
	constexpr auto end()   const noexcept -> const_iterator { return {this, {0,mSize.height}, {1,0}}; }

	constexpr auto rbegin()       noexcept -> reverse_iterator       { return {this, {mSize.width-1,mSize.height-1}, {-1,0}}; };
	constexpr auto rbegin() const noexcept -> const_reverse_iterator { return {this, {mSize.width-1,mSize.height-1}, {-1,0}}; };
	constexpr auto rend()         noexcept -> reverse_iterator       { return {this, {mSize.width,-1}, {-1,0}}; };
	constexpr auto rend()   const noexcept -> const_reverse_iterator { return {this, {mSize.width,-1}, {-1,0}}; };

	constexpr auto subbmp(position2_type _pos) const noexcept { return subbmp(_pos, size2() - size2_type(_pos)); }
	constexpr auto subbmp(position2_type _pos, size2_type _size) const noexcept;

protected:
	size2_type		mSize	= {0, 0};
	pitch_type		mPitch	= {0, 0};
	value_type*		mBuff	= nullptr;

	template<class, class>
	friend	class	view2d_iterator;
};


template<class T, class V>
class	view2d_iterator
{
public:
	using	value_type		= T;
	using	view_type		= V;
	using	position2_type	= typename view_type::position2_type;
	using	size2_type		= typename view_type::size2_type;
	using	pitch_type		= typename view_type::pitch_type;
	using	pointer			= value_type*;
	using	reference		= value_type&;

	view2d_iterator() = default;
	view2d_iterator(view2d_iterator const&) = default;
	view2d_iterator& operator = (view2d_iterator const&) = default;

	auto operator * () const noexcept -> reference { return *mCur; }
	auto operator -> () const noexcept -> pointer { return mCur; }

	auto operator ++ () -> view2d_iterator& { return increment( mVec); }
	auto operator -- () -> view2d_iterator& { return increment(-mVec); }
	auto operator ++ (int) -> view2d_iterator { view2d_iterator r(*this); ++(*this); return r; }
	auto operator -- (int) -> view2d_iterator { view2d_iterator r(*this); --(*this); return r; }

	bool operator == (view2d_iterator const& _r) const noexcept { return mCur == _r.mCur; }
	bool operator != (view2d_iterator const& _r) const noexcept { return mCur != _r.mCur; }

protected:
	view_type*		mView	= nullptr;
	position2_type	mPos	= {0, 0};
	position2_type	mVec	= {1, 0};
	value_type*		mCur	= nullptr;

	view2d_iterator(view_type* _view, position2_type _pos, position2_type _vec)
		: mView(_view)
		, mPos(_pos)
		, mVec(_vec)
		, mCur(_view->mBuff + _view->mPitch.y * _pos.y + _view->mPitch.x * _pos.x)
	{}

	view2d_iterator& increment(position2_type _vec)
	{
		auto	pos		= mPos;
		auto	siz		= mView->size2();
		auto	pitch	= mView->pitch();
		mPos += _vec;
		while (mPos.x < 0)
		{
			mPos.x += siz.width;
			--mPos.y;
		}
		while (mPos.x >= siz.width)
		{
			mPos.x -= siz.width;
			++mPos.y;
		}
		auto	off	=  mPos - pos;
		mCur	+= off.y * pitch.y + off.x * pitch.x;
		return	*this;
	}

	template<class>
	friend	class	view2d;
};


template<class T>
constexpr view2d<T>::view2d(size2_type _size, value_type* _buff, pitch_type _pitch)
	: mSize(_size)
	, mPitch{_pitch.x, _pitch.y ? _pitch.y : _size.width}
	, mBuff(_buff)
{}


template<class T>
constexpr auto view2d<T>::subbmp(position2_type _pos, size2_type _size) const noexcept
{
	size2_type	sz	= size2();
	value_type*	buf	= mBuff + mPitch.y * _pos.y + mPitch.x * _pos.x;
	sz.width	-= _pos.x;
	sz.height	-= _pos.y;
	return	view2d({ sz.width < _size.width ? sz.width : _size.width, sz.height < _size.height ? sz.height : _size.height }, buf, mPitch);
}


AMTRS_NAMESPACE_END
#endif

/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__images__bitmap_view__hpp
#define	__libamtrs__images__bitmap_view__hpp
AMTRS_NAMESPACE_BEGIN

template<class ViewT>
class	bitmap_view_iterator;


//! イメージにおける string_view のような存在。
//! このクラスは実態となるデータは持たず、実態のデータへの参照を保持します。
template<class PixelT>
class	basic_bitmap_view
{
public:
	using	value_type		= PixelT;
	using	pixel_type		= PixelT;
	using	pointer			= value_type*;
	using	reference		= value_type&;
	using	const_pointer	= typename std::add_const<value_type>::type*;
	using	const_reference	= typename std::add_const<value_type>::type&;
	using	rect_type		= rect<int>;
	using	position_type	= typename rect_type::position_type;
	using	size_type		= typename rect_type::size_type;
	using	pitch_type		= std::ptrdiff_t;						// data size of next line.


	using	iterator		= bitmap_view_iterator<basic_bitmap_view>;
	using	const_iterator	= bitmap_view_iterator<typename std::add_const<basic_bitmap_view>::type>;


	constexpr basic_bitmap_view() = default;
	constexpr basic_bitmap_view(const basic_bitmap_view&) = default;
	constexpr basic_bitmap_view& operator = (const basic_bitmap_view&) = default;

	basic_bitmap_view(pointer _pixels, size_type _size)
		: basic_bitmap_view(_pixels, _size, _size.width)
	{}

	basic_bitmap_view(pointer _pixels, size_type _size, pitch_type _pitch)
		: mPixels(_pixels)
		, mSize(_size)
		, mPitch(_pitch)
	{}

	      pointer data()       noexcept { return mPixels; }
	const_pointer data() const noexcept { return mPixels; }

	      iterator begin()        noexcept { return iterator(mSize.width, mPitch, mPixels, 0); }
	      iterator end()          noexcept { return iterator(mSize.width, mPitch, mPixels + mPitch * mSize.height, 0); }
	const_iterator begin()  const noexcept { return cbegin(); }
	const_iterator end()    const noexcept { return cend();   }
	const_iterator cbegin() const noexcept { return iterator(mSize.width, mPitch, mPixels, 0); }
	const_iterator cend()   const noexcept { return iterator(mSize.width, mPitch, mPixels + mPitch * mSize.height, 0); }


	const size_type& size() const noexcept { return mSize; }


	pitch_type pitch() const noexcept { return mPitch; }

	basic_bitmap_view flip_vertical() const noexcept
	{
		return	basic_bitmap_view(mPixels + mPitch * (size().height - 1), size(), -mPitch);
	}

	constexpr basic_bitmap_view subimg(rect_type _rect) const noexcept
	{
		return	basic_bitmap_view(mPixels + mPitch * _rect.y + _rect.x, _rect.size(), mPitch);
	}

private:
	pointer			mPixels;
	size_type		mSize;
	pitch_type		mPitch;
};



template<class ViewT>
class	bitmap_view_iterator
{
public:
	using	view_type			= ViewT;
	using	value_type			= typename view_type::value_type;
	using	pointer				= typename view_type::pointer;
	using	reference			= typename view_type::reference;
	using	const_pointer		= typename view_type::const_pointer;
	using	const_reference		= typename view_type::const_reference;
	using	difference_type 	= std::ptrdiff_t ;
	using	iterator_category	= std::forward_iterator_tag;
	using	pitch_type			= typename view_type::pitch_type;

	constexpr bitmap_view_iterator() = default;
	constexpr bitmap_view_iterator(const bitmap_view_iterator&) = default;
	constexpr bitmap_view_iterator& operator = (const bitmap_view_iterator&) = default;
	constexpr bitmap_view_iterator(int _width, pitch_type _pitch, pointer _cursor, int _x) : mCursor(_cursor), mX(_x), mWidth(_width), mPitch(_pitch) {}
	constexpr bool operator == (const bitmap_view_iterator& _r) const noexcept { return mCursor == _r.mCursor; }
	constexpr bool operator != (const bitmap_view_iterator& _r) const noexcept { return !(*this == _r); }
	constexpr pointer operator -> () const noexcept { return (mCursor + mX); }
	constexpr reference operator * () const noexcept { return *(mCursor + mX); }
	constexpr bitmap_view_iterator& operator ++ () noexcept { ++mX; if (mX >= mWidth) { mX = 0; mCursor += mPitch; } return *this; }
	constexpr bitmap_view_iterator  operator ++ (int) noexcept { bitmap_view_iterator retval(*this); ++(*this); return retval; }
	constexpr bitmap_view_iterator& operator -- () noexcept { --mX; if (mX < 0) { mX = mWidth - 1; mCursor -= mPitch; } return *this; }
	constexpr bitmap_view_iterator  operator -- (int) noexcept { bitmap_view_iterator retval(*this); --(*this); return retval; }
private:
	pointer			mCursor;
	int				mX;
	int				mWidth;
	pitch_type		mPitch;
};



AMTRS_NAMESPACE_END
#endif

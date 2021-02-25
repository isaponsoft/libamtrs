/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__images__imageview__hpp
#define	__libamtrs__images__imageview__hpp
AMTRS_NAMESPACE_BEGIN
//! imageviewのソースになれるかどうか調べる.
//! imageviewのコンストラクタの SFINAE 用
template<class T>
struct	can_image_view_source
{
private:
	template<class S>
	static auto has_data(S s) -> decltype(s.data(), std::true_type{});
	static auto has_data(...) -> std::false_type;

	template<class S>
	static auto has_size(S s) -> decltype(s.size().width, s.size().height, std::true_type{});
	static auto has_size(...) -> std::false_type;

	template<class S>
	static auto has_stride(S s) -> decltype(s.stride().x, s.stride().y, std::true_type{});
	static auto has_stride(...) -> std::false_type;

public:
	using	type	= typename std::conditional<
								decltype(has_data  (std::declval<T>())){}
							 && decltype(has_size  (std::declval<T>())){}
							 && decltype(has_stride(std::declval<T>())){}
							, std::true_type
							, std::false_type
					>::type;
	static constexpr type	value	= type{};
};


template<class T>	class	imageplane_iterator;


// ****************************************************************************
//! 位置と大きさ、ピクセルの歩幅を示す。
// ----------------------------------------------------------------------------
template<class T>
class	imageview
{
public:
	using		value_type		= T;
	using		pixel_type		= T;
	using		rect_type		= rect<int>;
	using		size_type		= size2<int>;
	using		stride_type		= vec2<int>;
	using		iterator		= imageplane_iterator<value_type>;
	using		const_iterator	= imageplane_iterator<typename std::add_const<value_type>::type>;


	value_type* data() const noexcept { return mData; }
	size_type size() const noexcept { return mSize; }
	stride_type stride() const noexcept { return mStride; }
	imageview& data(value_type* _data) noexcept { mData = _data; return *this; }
	imageview& size(size_type _size) noexcept { mSize = _size; return *this; }
	imageview& stride(stride_type _stride) noexcept { mStride = _stride; return *this; }


	// ========================================================================
	//! サイズとストライドを指定して生成する
	// ------------------------------------------------------------------------
	imageview(size_type _size, stride_type _stride, value_type* _data = nullptr)
		: mData(_data), mSize(_size), mStride(_stride)
	{}

	// ========================================================================
	//! コピーコンストラクタ
	// ------------------------------------------------------------------------
	template<class RT>
	explicit imageview(imageview<RT> const& _r) noexcept
		: mData(_r.data()), mSize(size_type(_r.size())), mStride(stride_type(_r.stride()))
	{}

	// ========================================================================
	//! data(), size(), stride() を持つimageviewと互換のある型からのキャスト
	// ------------------------------------------------------------------------
	template<class R>
	explicit imageview(R& _r) noexcept
		: mData(reinterpret_cast<value_type*>(_r.data())), mSize(size_type(_r.size())), mStride(stride_type(_r.stride()))
	{}

	// ========================================================================
	//! data(), size(), stride() を持つimageviewと互換のある型からのキャスト
	// ------------------------------------------------------------------------
	template<class R>
	explicit imageview(R const& _r) noexcept
		: mData(reinterpret_cast<value_type*>(_r.data())), mSize(size_type(_r.size())), mStride(stride_type(_r.stride()))
	{}


	// ========================================================================
	//! 指定した範囲を切り取ったビューを作ります。
	// ------------------------------------------------------------------------
	imageview subimg(rect_type _rect) const noexcept;


	// ========================================================================
	//! 上下を逆転したビューを作ります。
	// ------------------------------------------------------------------------
	imageview vertical_flip() const noexcept;


	imageview() = default;
	imageview(imageview const&) = default;


	// ========================================================================
	//! イテレータ
	// ------------------------------------------------------------------------
	      iterator begin()        noexcept { return iterator(*this); }
	const_iterator begin()  const noexcept { return const_iterator(*this); }
	const_iterator cbegin() const noexcept { return const_iterator(*this); }

	      iterator end()        noexcept { return       iterator(*this, reinterpret_cast<value_type*>(reinterpret_cast<std::intptr_t>(data()) + size().height * stride().y)); }
	const_iterator end()  const noexcept { return const_iterator(*this, reinterpret_cast<value_type*>(reinterpret_cast<std::intptr_t>(data()) + size().height * stride().y)); }
	const_iterator cend() const noexcept { return const_iterator(*this, reinterpret_cast<value_type*>(reinterpret_cast<std::intptr_t>(data()) + size().height * stride().y)); }

private:
	value_type*	mData;			//!< ピクセルデータの先頭
	size_type	mSize;			//!< 画像の大きさ
	stride_type	mStride;		//!< ピクセルの移動量
};


template<class T>
class	imageplane_iterator
{
public:
	using	pixel_type			= T;
	using	iterator_category	= std::bidirectional_iterator_tag;
	using	difference_type		= ptrdiff_t;
	using	value_type			= pixel_type;
	using	pointer				= value_type*;
	using	const_pointer		= typename std::add_const<value_type>::type*;
	using	reference			= value_type&;

	imageplane_iterator() = default;
	imageplane_iterator(imageplane_iterator const&) = default;
	imageplane_iterator& operator = (imageplane_iterator const&) = default;

	template<class T2>
	imageplane_iterator(imageview<T2> const& _plane) noexcept
		: mData(_plane.data())
		, mCursor(0)
		, mStride(_plane.stride())
		, mCursorMax(_plane.stride().x * _plane.size().width)
	{}

	template<class T2>
	imageplane_iterator(imageview<T2> const& _plane, value_type* _data) noexcept
		: mData(_data)
		, mCursor(0)
		, mStride(_plane.stride())
		, mCursorMax(_plane.stride().x * _plane.size().width)
	{}

	imageplane_iterator& operator ++ () noexcept
	{
		mCursor	+= mStride.x;
		if (mCursor >= mCursorMax)
		{
			mData	= reinterpret_cast<value_type*>(reinterpret_cast<std::intptr_t>(mData) + mStride.y);
			mCursor	= 0;
		}
		return	*this;
	}

	imageplane_iterator operator ++ (int) noexcept
	{
		imageplane_iterator	r(*this);
		operator ++ ();
		return	r;
	}

	value_type& operator * () const noexcept
	{
		return	*reinterpret_cast<value_type*>(reinterpret_cast<std::intptr_t>(mData) + mCursor);
	}

	bool operator == (imageplane_iterator const& _r) const noexcept
	{
		return	reinterpret_cast<value_type*>(reinterpret_cast<std::intptr_t>(mData) + mCursor)
		==		reinterpret_cast<value_type*>(reinterpret_cast<std::intptr_t>(_r.mData) + _r.mCursor);
	}

	bool operator != (imageplane_iterator const& _r) const noexcept
	{
		return	!operator == (_r);
	}

	difference_type operator - (imageplane_iterator const& _r) const noexcept
	{
		const_pointer	l	= &**this;
		const_pointer	r	= &*_r;
		return	l - r;
	}

protected:
	value_type*	mData;
	int			mCursor;
	vec2<int>	mStride;
	int			mCursorMax;
};





template<class T>
auto imageview<T>::subimg(rect_type _rect) const noexcept -> imageview
{
	imageview	r(*this);
	r.mData		= reinterpret_cast<value_type*>(reinterpret_cast<std::intptr_t>(mData) + (_rect.y * mStride.y) + (_rect.x * mStride.x));
	r.mSize		= _rect.size();
	r.mStride	= mStride;
	return	r;
}


template<class T>
auto imageview<T>::vertical_flip() const noexcept -> imageview
{
	imageview	r(*this);
	r.mData		= reinterpret_cast<value_type*>(reinterpret_cast<std::intptr_t>(mData) + ((mSize.height - 1) * mStride.y));
	r.mStride.y	= -mStride.y;
	return	r;
}



AMTRS_NAMESPACE_END
#endif

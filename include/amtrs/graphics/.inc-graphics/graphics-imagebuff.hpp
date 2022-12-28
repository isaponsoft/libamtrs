/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__images__imagebuff__hpp
#define	__libamtrs__images__imagebuff__hpp
AMTRS_NAMESPACE_BEGIN
namespace filesystem
{
class	fileloader;		// prototype only.
}


template<class...>
struct	loadimage_traits;



template<class T>
class	imagebuff
{
public:
	using		value_type		= T;
	using		pixel_type		= T;
	using		rect_type		= rect<int>;
	using		offest_type		= vec2<int>;
	using		size_type		= size2<int>;
	using		stride_type		= vec2<int>;
	using		iterator		= imageplane_iterator<value_type>;
	using		const_iterator	= imageplane_iterator<typename std::add_const<value_type>::type>;
	using		view_type		= imageview<value_type>;
	using		const_view_type	= imageview<typename std::add_const<value_type>::type>;
	using		buffer_type		= shared_buffer<value_type>;

	imagebuff() = default;
	imagebuff(imagebuff const& _s) = default;
	imagebuff(imagebuff&& _s) = default;

	imagebuff(size_type _size);
	imagebuff(view_type const& _s) : imagebuff(const_view_type(_s)) {}
	imagebuff(const_view_type const& _s);

	template<class It>
	imagebuff(size_type _size, It _from);

	static imagebuff load(std::string_view _filename, filesystem::fileloader* _loader = nullptr);

	imagebuff& operator = (imagebuff const& _s) = default;
	imagebuff& operator = (imagebuff&& _s) = default;

	bool empty() const noexcept { return mValue.empty() || mSize.empty(); }
	value_type*       data()       noexcept { return empty() ? nullptr : mValue.data(); }
	value_type const* data() const noexcept { return empty() ? nullptr : mValue.data(); }
	size_type size() const noexcept { return empty() ? size_type(0,0) : mSize; }
	size_t count() const noexcept { return empty() ? (size_t)0 : (size_t)(size().width * size().height); }

	auto begin() noexcept { return ((view_type)*this).begin(); }
	auto begin() const noexcept { return ((const_view_type)*this).begin(); }
	auto cbegin() const noexcept { return ((const_view_type)*this).cbegin(); }
	auto end() noexcept { return ((view_type)*this).end(); }
	auto end() const noexcept { return ((const_view_type)*this).end(); }
	auto cend() const noexcept { return ((const_view_type)*this).cend(); }

	// ==============================================================
	//! ピクセルの移動量を返します。
	// --------------------------------------------------------------
	stride_type stride() const noexcept { return mStride; }

	// ==============================================================
	//! バッファを取得します。
	// --------------------------------------------------------------
	buffer_type const& buffer() const noexcept { return	mValue; }

private:
	offest_type		mOffest	= { 0, 0 };
	size_type		mSize	= { 0, 0 };
	stride_type		mStride	= { 0, 0 };
	buffer_type		mValue;
};



imagebuff<uint8_t> create_imagebuff(imageview<void const> _view, color_format _color);



template<class T>
imagebuff<T>::imagebuff(size_type _size)
{
	if (!_size.empty())
	{
		mValue		= buffer_type(_size.width * _size.height);
		mSize		= _size;
		mStride		= { (int)sizeof(value_type), (int)(sizeof(value_type) * mSize.width) };
	}
}


template<class T>
template<class It>
imagebuff<T>::imagebuff(size_type _size, It _from)
{
	if (!_size.empty())
	{
		mValue		= buffer_type(_size.width * _size.height);
		mSize		= _size;
		mStride		= { (int)sizeof(value_type), (int)(sizeof(value_type) * mSize.width) };
		std::copy_n(_from, mValue.size(), (*this).begin());
	}
}



template<class T>
imagebuff<T>::imagebuff(const_view_type const& _s)
{
	if (!_s.size().empty())
	{
		mValue		= buffer_type(_s.size().width * _s.size().height);
		mSize		= _s.size();
		mStride		= { sizeof(value_type), sizeof(value_type) * mSize.width };
		std::copy(_s.begin(), _s.end(), (*this).begin());
	}
}


template<>
struct	loadimage_traits<rgba<uint8_t>>
{
	using	image_type	= imagebuff<rgba<uint8_t>>;

	image_type load(std::string_view _filename, filesystem::fileloader* _loader);
};


template<class T>
auto imagebuff<T>::load(std::string_view _filename, filesystem::fileloader* _loader) -> imagebuff<T>
{
	return	loadimage_traits<T>{}.load(_filename, _loader);
}


AMTRS_NAMESPACE_END
#endif

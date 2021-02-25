/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__bitmaps__bitmap__hpp
#define	__libamtrs__bitmaps__bitmap__hpp
AMTRS_NAMESPACE_BEGIN

template<class...>
class	basic_bitmap;

enum class color_type
{
	i4		= 1,
	i8		= 2,
	rgba8	= 3,
};



template<class PixelT, class BufferT>
class	basic_bitmap<PixelT, BufferT>
{
public:
	using	value_type				= PixelT;
	using	pixel_type				= PixelT;
	using	reference				= PixelT&;
	using	const_reference			= typename std::add_const<PixelT>::type&;
	using	bitmap_view_type		= basic_bitmap_view<pixel_type>;
	using	const_bitmap_view_type	= basic_bitmap_view<const pixel_type>;
	using	iterator				= typename bitmap_view_type::iterator;
	using	const_iterator			= typename const_bitmap_view_type::const_iterator;

	using	point_type				= int;
	using	rect_type				= rect<point_type>;
	using	position_type			= typename rect_type::position_type;
	using	size_type				= typename rect_type::size_type;
	using	stride_type				= vec2<int>;

	// ========================================================================
	//! 指定した入力ソースから画像を読み取り、bitmapとして返します。
	// ------------------------------------------------------------------------
	//! 入力ソースは amtrs::ios::rstream で識別可能な型に限ります。
	//! また、画像の読み込みそのものはプラットフォームが提供する画像読み込み用の
	//! APIを利用します。
	// ------------------------------------------------------------------------
	template<class In>
	static basic_bitmap load(In&& _in);


	constexpr std::size_t bitrate() const noexcept { return sizeof(value_type) * 8; }
	constexpr color_type format() const noexcept { return color_type::rgba8; }

	basic_bitmap() = default;
	basic_bitmap(const basic_bitmap&) = default;
	basic_bitmap(basic_bitmap&&) = default;
	basic_bitmap& operator = (const basic_bitmap&) = default;
	basic_bitmap& operator = (basic_bitmap&&) = default;
	
	basic_bitmap(const size_type& _size)
		: mSize(_size)
		, mPixelData(static_cast<std::size_t>(_size.width * _size.height))
	{}

	template<class Ite>
	basic_bitmap(const size_type& _size, Ite _first, Ite _last)
		: mSize(_size)
	{
		mPixelData.reserve(static_cast<std::size_t>(_size.width * _size.height));
		while (_first != _last)
		{
			mPixelData.push_back(*_first);
			++_first;
		}
		mPixelData.resize(static_cast<std::size_t>(_size.width * _size.height));
	}

	basic_bitmap(bitmap_view_type _sub)
		: mSize(_sub.size())
		, mPixelData(static_cast<std::size_t>(_sub.size().width * _sub.size().height))
	{
		std::copy(_sub.begin(), _sub.end(), begin());
	}

	//! bitmap_view へ変換します。
	operator bitmap_view_type () { return bitmap_view_type(mPixelData.data(), size(), size().width); }

	bitmap_view_type view() { return bitmap_view_type(mPixelData.data(), size(), size().width); }

	//! 要素へアクセスします。
	template<class IT>
	      reference operator [] (vec2<IT> _pos)       { return mPixelData.data()[size().width * (static_cast<int>(_pos.y)) + static_cast<int>(_pos.x)]; }
	template<class IT>
	const_reference operator [] (vec2<IT> _pos) const { return mPixelData.data()[size().width * (static_cast<int>(_pos.y)) + static_cast<int>(_pos.x)]; }

	//! 要素へアクセスするためのイテレータ
	iterator begin() noexcept { return bitmap_view_type(*this).begin(); }
	iterator end()   noexcept { return bitmap_view_type(*this).end();   }
	const_iterator begin() const noexcept { return const_bitmap_view_type(*this).begin(); }
	const_iterator end()   const noexcept { return const_bitmap_view_type(*this).end();   }
	const_iterator cbegin() const noexcept { return const_bitmap_view_type(*this).begin(); }
	const_iterator cend()   const noexcept { return const_bitmap_view_type(*this).end();   }

	//! 矩形を指定し、bitmap_view を生成します。
	      bitmap_view_type subimg(const rect_type& _rect)       noexcept { return       bitmap_view_type(mPixelData.data() + _rect.y * size().width + _rect.x, _rect.size(), size().width); }
	const_bitmap_view_type subimg(const rect_type& _rect) const noexcept { return const_bitmap_view_type(mPixelData.data() + _rect.y * size().width + _rect.x, _rect.size(), size().width); }

	size_type size() const noexcept { return mSize; }
	stride_type stride() const noexcept { return stride_type(sizeof(value_type), sizeof(value_type) * mSize.width); };

	bool empty() const noexcept { return mSize.empty(); }

	template<class It>
	void assign(It _first, It _last)
	{
		mPixelData.assign(_first, _last);
	}


	      pixel_type* data()       noexcept { return mPixelData.data(); }
	const pixel_type* data() const noexcept { return mPixelData.data(); }

	      BufferT& pixels()       noexcept { return mPixelData; }
	const BufferT& pixels() const noexcept { return mPixelData; }

	// ========================================================================
	//! ２ｘ２でアンチエリアスをかけた画像を生成します。
	// ------------------------------------------------------------------------
	//! 半分のサイズの画像が生成されます。
	// ------------------------------------------------------------------------
	basic_bitmap<PixelT, BufferT> antialias2x2() const noexcept
	{
		basic_bitmap<PixelT, BufferT>	img2(size()/2);
		auto							dest		= img2.pixels().data();
		auto							src			= pixels().data();
		auto							srcPitch	= size().width;
		for (int y = 0; y < img2.size().height; ++y)
		{
			for (int x = 0; x < img2.size().width; ++x)
			{
				dest->r	= (uint8_t)(((unsigned)src[0].r + (unsigned)src[1].r + (unsigned)src[srcPitch + 0].r + (unsigned)src[srcPitch + 1].r) / 4);
				dest->g	= (uint8_t)(((unsigned)src[0].g + (unsigned)src[1].g + (unsigned)src[srcPitch + 0].g + (unsigned)src[srcPitch + 1].g) / 4);
				dest->b	= (uint8_t)(((unsigned)src[0].b + (unsigned)src[1].b + (unsigned)src[srcPitch + 0].b + (unsigned)src[srcPitch + 1].b) / 4);
				dest->a	= (uint8_t)(((unsigned)src[0].a + (unsigned)src[1].a + (unsigned)src[srcPitch + 0].a + (unsigned)src[srcPitch + 1].a) / 4);
				dest += 1;
				src  += 2;
			}
			src  += srcPitch;
		}
		return	img2;
	}

private:
	size_type		mSize		= {0, 0};
	BufferT			mPixelData;
};


template<class PixelT>
using	bitmap		= basic_bitmap<PixelT, std::vector<PixelT>>;

using	bitmap32	= bitmap<rgba<uint8_t>>;


AMTRS_NAMESPACE_END
#include AMTRS_DRIVER_INCLUDE(graphics-bitmap.hpp)
#endif

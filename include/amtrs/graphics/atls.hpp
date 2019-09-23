#ifndef	__libamtrs__graphics__atls__hpp
#define	__libamtrs__graphics__atls__hpp
#include <tuple>
#include <vector>
#include "def.hpp"
#include "rect.hpp"
AMTRS_NAMESPACE_BEGIN


// ****************************************************************************
//! ２次元矩形の領域管理を行います。
// ----------------------------------------------------------------------------
//! コンストラクタで指定した２次元サイズから、allocate() で指定する２次元領域を
//! 確保したり、開放したりします。
//!	
//!	atls<int>			my_image_area({100, 100});
//!	rect<int>			area1 = my_image_area.allocate(50, 100);
//!	rect<int>			area2 = my_image_area.allocate(50, 100);
//!	my_image_area.deallocate(area1);
//!	my_image_area.deallocate(area2);
// ----------------------------------------------------------------------------
template<class T, class Containor = std::vector<rect<T>>>
class	atls
{
	using	containor_type	= Containor;
public:
	using	value_type		= T;
	using	position_type	= vec2<value_type>;
	using	size_type		= size2<value_type>;
	using	rect_type		= rect<value_type>;

	atls() = default;
	atls(const atls&) = default;
	atls(atls&&) = default;
	atls& operator = (const atls&) = default;
	atls& operator = (atls&&) = default;

	void swap(atls& _b) noexcept
	{
		std::swap(mSize, _b.mSize);
		std::swap(mBlock, _b.mBlock);
	}

	atls(size_type _size)
		: mSize(_size)
	{}

	size_type size() const noexcept
	{
		return	mSize;
	}

	rect_type allocate(size_type _size)
	{
		rect_type	b{0, 0, _size.width, _size.height};
		if (is_free(b))
		{
			b	= rect_type{0, 0, _size.width, _size.height};
			mBlock.push_back(b);
			return	b;
		}
		for (auto& r : mBlock)
		{
			b	= rect_type{r.x + r.width, r.y, _size.width, _size.height};		// 右側
			if (b.right() <= mSize.width && b.bottom() <= mSize.height && is_free(b))
			{
				mBlock.push_back(b);
				return	b;
			}
			b	= rect_type{r.x, r.y + r.height, _size.width, _size.height};		// 下側
			if (b.right() <= mSize.width && b.bottom() <= mSize.height && is_free(b))
			{
				mBlock.push_back(b);
				return	b;
			}
		}
		return	{0,0,0,0};
	}

	void deallocate(position_type _pos) noexcept
	{
		auto	it	= std::remove_if(mBlock.begin(), mBlock.end(), [_pos](auto& r) -> bool
					{
						return	r.position() == _pos;
					});
		mBlock.erase(it, mBlock.end());
	}

	auto begin() const noexcept { return mBlock.begin(); }
	auto end() const noexcept { return mBlock.end(); }

private:

	//! ぶつかる領域があるか調べる
	bool is_free(rect_type _rect)
	{
		auto	it	= std::find_if(mBlock.begin(), mBlock.end(), [_rect](auto& r) -> bool
					{
						return	r.is_collision(_rect);
					});
		return	it == mBlock.end();
	}

	size_type		mSize;
	containor_type	mBlock;
};


AMTRS_NAMESPACE_END
#endif

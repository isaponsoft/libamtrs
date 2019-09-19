#ifndef	__libamtrs__bitmaps__tests__hpp
#define	__libamtrs__bitmaps__tests__hpp
// bitmaps library test codes.

#include <fstream>
#include <vector>


#include "bitmap_view.hpp"
#include "bitmap.hpp"
#include "bmp.hpp"
#include "draw.hpp"
AMTRS_IMAGE_NAMESPACE_BEGIN

template<class Report>
inline int tests(Report&& _report)
{
	using	bitmap_t	= basic_bitmap<rgba<uint8_t>, std::vector<rgba<uint8_t>>>;

	// 128x128 のイメージを作成
	bitmap_t	pixels({128, 128});
	
	for (auto& pix : pixels)
	{
		pix = {0, 0, 0, 255};
	}

	basic_bitmap_view<rgba<uint8_t>>	view;
	view	= pixels;
	for (auto& pix : view.subimg({10,10,108,108}))
	{
		pix = {0, 128, 128, 255};
	}
	for (auto& pix : view.subimg({10,10,108,108}).subimg({1,1,30,30}))
	{
		pix = {0, 128, 0, 255};
	}



	// bmpファイルにして画像を出力
	std::ofstream	f("bitmaps_tests_128x128.bmp");
	f << bmp<decltype(pixels)>(pixels);
	f.flush();

	return	0;
}



AMTRS_IMAGE_NAMESPACE_END
#endif

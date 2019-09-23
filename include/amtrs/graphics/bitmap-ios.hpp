#ifndef	__libamtrs__bitmaps__bitmap_ios__hpp
#define	__libamtrs__bitmaps__bitmap_ios__hpp
// Framework
//	CoreGraphics.framework
#include <UIKit/UIKit.h>
#include <atomic>
#include <iostream>
#include "../../iostream.hpp"
AMTRS_NAMESPACE_BEGIN



template<class PixelT, class BufferT>
template<class In>
auto basic_bitmap<PixelT, BufferT>::load(In&& _in) -> basic_bitmap
{
	// ファイルサイズ取得＆バッファに読み込み
	auto		in			= ios::make_rstream(std::forward<In>(_in));
	auto		inSize		= static_cast<std::size_t>(in.seekg(0, std::ios_base::end).tellg());
	in.seekg(0, std::ios_base::beg);

	std::vector<char>	buff(inSize);
	in.read(buff.data(), inSize);

	
    
    UIImage*	image	= [[UIImage alloc] initWithData:[NSData
											dataWithBytes:	(const void *)buff.data()
											length:			inSize
										]
						];
	buff	= {};

	CGImageRef  		imageRef		= image.CGImage;
	CFDataRef			dataRef			= CGDataProviderCopyData(CGImageGetDataProvider(imageRef));
	int8_t*				pixels			= (int8_t*)CFDataGetBytePtr(dataRef);
	std::size_t			pitch			= CGImageGetBytesPerRow(imageRef);

	basic_bitmap<PixelT, BufferT>	retval(typename basic_bitmap<PixelT, BufferT>::size_type(image.size.width, image.size.height));
	auto*							dest	= retval.pixels().data();
	for (unsigned int y = 0; y < retval.size().height; ++y)
	{
		auto*	s	= pixels + (y * pitch);
		for (unsigned int x = 0; x < retval.size().width; ++x)
		{
			dest->r	= *(s + 0);
			dest->g	= *(s + 1);
			dest->b	= *(s + 2);
			dest->a	= *(s + 3);
			++dest;
			s += 4;
		}
	}
	CFRelease(dataRef);

	return	retval;
}





AMTRS_NAMESPACE_END
#endif

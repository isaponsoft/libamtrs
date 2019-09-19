#ifndef	__libamtrs__bitmaps__bitmap_android__hpp
#define	__libamtrs__bitmaps__bitmap_android__hpp
#include <atomic>
#include <iostream>
#include <android/bitmap.h>							// Android NDK
#include "../../iostream.hpp"
#include "../../os/android/android_app.hpp"
#include "../../java/android/graphics/Bitmap.hpp"
AMTRS_NAMESPACE_BEGIN


// リンク時に
// 	jnigraphics
// が必要になります。


// ****************************************************************************
// Android専用関数群
// ----------------------------------------------------------------------------


// ============================================================================
//! android/graphics/Bitmap オブジェクトから amtrs::bitmap を生成します。
// ----------------------------------------------------------------------------
template<class PixelT, class BufferT>
inline basic_bitmap<PixelT, BufferT> create_bitmap_from_android_bitmap(JNIEnv* _env, jobject _bitmap);


// ============================================================================
//! android/graphics/Bitmap オブジェクトから amtrs::bitmap を生成します。
// ----------------------------------------------------------------------------
template<class PixelT, class BufferT>
inline void create_bitmap_from_android_bitmap(basic_bitmap<PixelT, BufferT>& _dest, JNIEnv* _env, jobject _bitmap);





template<class PixelT, class BufferT>
template<class In>
auto basic_bitmap<PixelT, BufferT>::load(In&& _in) -> basic_bitmap
{
	using	BitmapFactory		= java::classes::android::graphics::BitmapFactory;

	basic_bitmap<PixelT, BufferT>	retval;
	JNIEnv*		env			= java::get_jnienv();
	auto		clsBmp		= BitmapFactory::find();
	auto		in			= ios::make_rstream(std::forward<In>(_in));
	std::size_t	inSize		= in.seekg(0, std::ios_base::end).tellg();
	in.seekg(0, std::ios_base::beg);
	auto		bitmapData	= java::localref<jbyteArray>::make(env->NewByteArray(inSize));
	jbyte*		array		= env->GetByteArrayElements(bitmapData.get(), nullptr);
	in.read((char*)array, inSize);
	env->ReleaseByteArrayElements(bitmapData.get(), array, 0);

	auto	bmp				= clsBmp.decodeByteArray(bitmapData.get(), 0, (jint)inSize);
	retval	= 	create_bitmap_from_android_bitmap<PixelT, BufferT>
			(
				env,
				bmp.get()
			);
	return	retval;
}


template<class PixelT, class BufferT>
basic_bitmap<PixelT, BufferT> create_bitmap_from_android_bitmap(JNIEnv* _env, jobject _bitmap)
{
	AndroidBitmapInfo	info;
	AndroidBitmap_getInfo(_env, _bitmap, &info);

	basic_bitmap<PixelT, BufferT>	retval(typename basic_bitmap<PixelT, BufferT>::size_type(info.width, info.height));

	void*	pixelsTop;
	switch (AndroidBitmap_lockPixels(_env, _bitmap, &pixelsTop))
	{
		case ANDROID_BITMAP_RESULT_SUCCESS  :
			break;
		case ANDROID_BITMAP_RESULT_BAD_PARAMETER   :
			AMTRS_DEBUG_LOG("ANDROID_BITMAP_RESULT_BAD_PARAMETER");
			break;
		case ANDROID_BITMAP_RESULT_JNI_EXCEPTION   :
			AMTRS_DEBUG_LOG("ANDROID_BITMAP_RESULT_JNI_EXCEPTION");
			break;
		case ANDROID_BITMAP_RESULT_ALLOCATION_FAILED   :
			AMTRS_DEBUG_LOG("ANDROID_BITMAP_RESULT_ALLOCATION_FAILED");
			break;
	}

	for (uint32_t y = 0; y < info.height; ++y)
	{
		rgba<uint8_t>*	pixels	= reinterpret_cast<rgba<uint8_t>*>(static_cast<char*>(pixelsTop) + info.stride * y);
		for (uint32_t x = 0; x < info.width; ++x)
		{
			retval[vec2<uint32_t>(x, y)]	= *pixels++;
		}
	}
	AndroidBitmap_unlockPixels(_env, _bitmap);

	return	retval;
}


template<class PixelT, class BufferT>
void create_bitmap_from_android_bitmap(basic_bitmap<PixelT, BufferT>& _dest, JNIEnv* _env, jobject _bitmap)
{
	_dest	= create_bitmap_from_android_bitmap<PixelT, BufferT>(_env, _bitmap);
}




AMTRS_NAMESPACE_END
#endif

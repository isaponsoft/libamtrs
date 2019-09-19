#ifndef	__libamtrs__os__win32__gdi_bitmap__hpp
#define	__libamtrs__os__win32__gdi_bitmap__hpp
#include <system_error>
#include "windows.hpp"
#include "../../inc/graphics/bitmap.hpp"
#include "../../inc/graphics/color.hpp"
#include "../../inc/graphics/size.hpp"
#include "def.hpp"
#include "error_code.hpp"
AMTRS_OS_WIN32_NAMESPACE_BEGIN


struct	bitmap
{
	template<class ST = unsigned int>
	static HBITMAP create_compatible(HDC _dc, size2<ST> _size)
	{
		HBITMAP		bmp	= CreateCompatibleBitmap(_dc, _size.width, _size.height);
		if (bmp == nullptr)
		{
			throw	std::system_error(make_last_error_code());
		}
		return	bmp;
	}

	template<class ST>
	static void get_dibits(HDC _dc, HBITMAP _bmp, size2<ST> _size, rgba<uint8_t>* _buffer)
	{
		::BITMAPINFOHEADER	bmih	= {};
		bmih.biSize			= sizeof(bmih);
		bmih.biWidth		= (DWORD)_size.width;
		bmih.biHeight		= (DWORD)_size.height;
		bmih.biPlanes		= 1;
		bmih.biBitCount		= 32;
		bmih.biSizeImage	= bmih.biBitCount * bmih.biWidth * bmih.biHeight / 8;
		if (!GetDIBits(_dc, _bmp, 0, bmih.biHeight, _buffer, (LPBITMAPINFO)&bmih, DIB_RGB_COLORS))
		{
			throw	std::system_error(make_last_error_code());
		}
	}

	static size2<unsigned int> get_size(HBITMAP /*_bmp*/)
	{
		::BITMAP	bmp;
		return	{(unsigned int)bmp.bmWidth, (unsigned int)bmp.bmHeight};
	}

	template<class PixelT, class BufferT, class ST = unsigned int>
	static void create_bitmap(basic_bitmap<PixelT, BufferT>& _dest, HDC _dc, HBITMAP _bmp, size2<ST> _size)
	{
		_dest	= basic_bitmap<PixelT, BufferT>(_size.cast_to<unsigned int>());

		basic_bitmap<PixelT, BufferT>	tmp	= _dest;
		get_dibits(_dc, _bmp, _size, tmp.pixels().data());
		auto	f = tmp.subimg({{0, 0}, tmp.size()}).flip_vertical();
		_dest.assign(f.begin(), f.end());
		for (auto& c : _dest)
		{
			std::swap(c.r, c.b);
			c.a = 255;
		}
	}

	template<class PixelT, class BufferT = std::vector<PixelT>, class ST = unsigned int>
	static basic_bitmap<PixelT, BufferT> create_bitmap(HDC _dc, HBITMAP _bmp, size2<ST> _size)
	{
		basic_bitmap<PixelT, BufferT>	retval(_size.cast_to<unsigned int>());
		create_bitmap(retval, _dc, _bmp, _size);
		return	retval;
	}

	template<class PixelT, class BufferT = std::vector<PixelT>, class ST = unsigned int>
	static basic_bitmap<PixelT, BufferT> create_bitmap(HDC _dc, HBITMAP _bmp)
	{
		auto							bmpSize	= get_size(_bmp);
		basic_bitmap<PixelT, BufferT>	retval(bmpSize.cast_to<unsigned int>());
		create_bitmap(retval, _dc, _bmp, bmpSize);
		return	retval;
	}
};


AMTRS_OS_WIN32_NAMESPACE_END
#endif

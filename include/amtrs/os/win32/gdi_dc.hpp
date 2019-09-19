#ifndef	__libamtrs__os__win32__gdi_dc__hpp
#define	__libamtrs__os__win32__gdi_dc__hpp
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")
#include <system_error>
#include "windows.hpp"
#include "def.hpp"
AMTRS_OS_WIN32_NAMESPACE_BEGIN


//! Compatible DC
class	compatible_dc
{
public:
	constexpr compatible_dc() noexcept = default;
	constexpr compatible_dc(const compatible_dc&) = delete;
	constexpr compatible_dc(compatible_dc&& _r) noexcept
		: mDC(_r.mDC)
	{
		_r.mDC = nullptr;
	}
	constexpr compatible_dc& operator = (const compatible_dc&) = delete;
	compatible_dc& operator = (compatible_dc&& _r) noexcept
	{
		_deletedc();
		mDC = _r.mDC;
		_r.mDC = nullptr;
		return	*this;
	}
	~compatible_dc()
	{
		_deletedc();
	}

	explicit compatible_dc(HDC _base_dc)
	{
		mDC = ::CreateCompatibleDC(_base_dc);
	}

	bool empty() const noexcept { return mDC == nullptr; }

	HDC get() const noexcept { return mDC; }

	HDC release() noexcept
	{
		HDC	retval(mDC);
		mDC = nullptr;
		return	retval;
	}

protected:
	void _deletedc()
	{
		if (mDC)
		{
			DeleteDC(mDC);
			mDC = nullptr;
		}
	}

	HDC		mDC	= nullptr;
};


AMTRS_OS_WIN32_NAMESPACE_END
#endif

/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__driver__winrt__winrt_std__hpp
#define	__libamtrs__driver__winrt__winrt_std__hpp
#include ".api-windows.hpp"
#include <unknwn.h>
#include <winrt\Windows.Foundation.h>
#include <winrt\Windows.Foundation.Collections.h>
#include "../../string/to_string.hpp"
#pragma comment(lib, "windowsapp")


namespace winrt::Windows::Foundation {

	struct __declspec(uuid("5b0d3235-4dba-4d44-865e-8f1d0e4fd04d")) __declspec(novtable) IMemoryBufferByteAccess : ::IUnknown
	{
		virtual HRESULT __stdcall GetBuffer(uint8_t** value, uint32_t* capacity) = 0;
	};

}


AMTRS_NAMESPACE_BEGIN

template<>
struct	cvtstring<winrt::hstring>
{
	std::string to(winrt::hstring const& _value) const
	{
		return	winrt::to_string(_value);
	}
};


AMTRS_NAMESPACE_END
#endif

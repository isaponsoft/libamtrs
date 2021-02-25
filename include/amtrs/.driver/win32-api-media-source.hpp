/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__driver__winrt__api_media_source__hpp
#define	__libamtrs__driver__winrt__api_media_source__hpp
#include <winrt\Windows.Media.Core.h>
#include <winrt\Windows.Media.Devices.h>
#include <winrt\Windows.Media.MediaProperties.h>
#include ".api-winrt-std.hpp"
AMTRS_OS_WIN32_NAMESPACE_BEGIN



template<class If>
auto enumrate_media_source(If&& _iffunc) -> std::vector<std::tuple<winrt::Windows::Media::Capture::Frames::MediaFrameSourceGroup, winrt::Windows::Media::Capture::Frames::MediaFrameSourceInfo>>
{
	using namespace winrt::Windows::Media::Capture::Frames;

	std::vector<std::tuple<MediaFrameSourceGroup, MediaFrameSourceInfo>>	r;
	for (MediaFrameSourceGroup group : MediaFrameSourceGroup::FindAllAsync().get())
	{
		for (MediaFrameSourceInfo info : group.SourceInfos())
		{
			if (_iffunc(group, info))
			{
				r.push_back({group, info});
			}
		}
	}
	return	r;
}



AMTRS_OS_WIN32_NAMESPACE_END
#endif

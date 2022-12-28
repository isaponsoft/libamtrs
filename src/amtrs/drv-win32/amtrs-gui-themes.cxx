/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <dwmapi.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <amtrs/gui/themes.hpp>

#pragma	comment(lib, "windowsapp.lib")
#pragma	comment(lib, "Dwmapi.lib")

// https://learn.microsoft.com/ja-jp/windows/apps/desktop/modernize/apply-windows-themes
#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif

using namespace winrt::Windows::UI::ViewManagement;

//#pragma	comment(lib, "uxtheme.dll")
AMTRS_OS_NAMESPACE_BEGIN
class	themes : public AMTRS_NAMESPACE::themes
{
public:
	themes(themes_type _type)
		: mType(_type)
	{}

	void init()
	{
		mSetting	= UISettings();
	}

	inline auto type() const noexcept
	{
		return	mType;
	}

	inline auto foreground() -> color_type
	{
		auto	c		= mSetting.GetColorValue(UIColorType::Foreground);
		return	color_type(c.R, c.G, c.B, c.A);
	}

	inline auto background() -> color_type
	{
		auto	c		= mSetting.GetColorValue(UIColorType::Background);
		return	color_type(c.R, c.G, c.B, c.A);
	}

	UISettings	mSetting;
	themes_type	mType;
};
AMTRS_OS_NAMESPACE_END
AMTRS_NAMESPACE_BEGIN
auto themes::create(themes_type _type) -> ref<themes>
{
	if (_type == sysdefault)
	{
		_type	= current();
	}
	ref<os::themes>	thiz	= new os::themes(_type);
	thiz->init();
	return	thiz.get();
}


auto themes::current() -> themes_type
{
	BOOL	isDarkMode	= TRUE;
	::DwmGetWindowAttribute(GetDesktopWindow(), DWMWA_USE_IMMERSIVE_DARK_MODE, &isDarkMode, sizeof(isDarkMode));
	return	isDarkMode
			? darkmode
			: lightmode;
}

auto themes::type() const noexcept -> themes_type { return static_cast<os::themes const*>(this)->type(); }
auto themes::foreground() -> color_type { return static_cast<os::themes*>(this)->foreground(); }
auto themes::background() -> color_type { return static_cast<os::themes*>(this)->background(); }


AMTRS_NAMESPACE_END

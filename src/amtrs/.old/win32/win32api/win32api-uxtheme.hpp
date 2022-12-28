/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__driver__win32__win32api_uxtheme__hpp
#define	__libamtrs__driver__win32__win32api_uxtheme__hpp
#define WIN32_LEAN_AND_MEAN
#ifndef	NOMINMAX
#define NOMINMAX
#endif
#include "../win32-api-windows.hpp"
#include <CommCtrl.h>
#include <Uxtheme.h>
#include <WindowsX.h>
#include <Vssym32.h>
#pragma comment(lib, "Comctl32.lib")
#pragma comment(lib, "Uxtheme.lib")

enum IMMERSIVE_HC_CACHE_MODE
{
	IHCM_USE_CACHED_VALUE,
	IHCM_REFRESH
};

enum	PREFERREDAPPMODE
{
	APPMODE_DEFAULT,
	APPMODE_ALLOWDARK,
	APPMODE_FORCEDARK,
	APPMODE_FORCELIGHT,
};


enum WINDOWCOMPOSITIONATTRIB
{
	WCA_UNDEFINED = 0,
	WCA_NCRENDERING_ENABLED = 1,
	WCA_NCRENDERING_POLICY = 2,
	WCA_TRANSITIONS_FORCEDISABLED = 3,
	WCA_ALLOW_NCPAINT = 4,
	WCA_CAPTION_BUTTON_BOUNDS = 5,
	WCA_NONCLIENT_RTL_LAYOUT = 6,
	WCA_FORCE_ICONIC_REPRESENTATION = 7,
	WCA_EXTENDED_FRAME_BOUNDS = 8,
	WCA_HAS_ICONIC_BITMAP = 9,
	WCA_THEME_ATTRIBUTES = 10,
	WCA_NCRENDERING_EXILED = 11,
	WCA_NCADORNMENTINFO = 12,
	WCA_EXCLUDED_FROM_LIVEPREVIEW = 13,
	WCA_VIDEO_OVERLAY_ACTIVE = 14,
	WCA_FORCE_ACTIVEWINDOW_APPEARANCE = 15,
	WCA_DISALLOW_PEEK = 16,
	WCA_CLOAK = 17,
	WCA_CLOAKED = 18,
	WCA_ACCENT_POLICY = 19,
	WCA_FREEZE_REPRESENTATION = 20,
	WCA_EVER_UNCLOAKED = 21,
	WCA_VISUAL_OWNER = 22,
	WCA_HOLOGRAPHIC = 23,
	WCA_EXCLUDED_FROM_DDA = 24,
	WCA_PASSIVEUPDATEMODE = 25,
	WCA_USEDARKMODECOLORS = 26,
	WCA_LAST = 27
};

struct	WINDOWCOMPOSITIONATTRIBDATA
{
	WINDOWCOMPOSITIONATTRIB	Attrib;
	PVOID					pvData;
	SIZE_T					cbData;
};

extern	HTHEME				(WINAPI*	OpenNcThemeData)(HWND, LPCWSTR);
extern	void				(WINAPI*	RefreshImmersiveColorPolicyState)();
extern	bool				(WINAPI*	GetIsImmersiveColorUsingHighContrast)(IMMERSIVE_HC_CACHE_MODE);
extern	bool				(WINAPI*	ShouldAppsUseDarkMode)();
extern	PREFERREDAPPMODE	(WINAPI*	SetPreferredAppMode)(PREFERREDAPPMODE);
extern	bool				(WINAPI*	IsDarkModeAllowedForWindow)();
extern	BOOL				(WINAPI*	SetWindowCompositionAttribute)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);
extern	bool				(WINAPI*	AllowDarkModeForWindow)(HWND, bool);


AMTRS_OS_WIN32_NAMESPACE_BEGIN

bool init_uxtheme();
bool allow_theme(HWND _hwnd, PREFERREDAPPMODE _allow = APPMODE_ALLOWDARK);

AMTRS_OS_WIN32_NAMESPACE_END
#endif

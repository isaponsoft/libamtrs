/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/amtrs.hpp>
#include <amtrs/.driver/win32/win32api/win32api-uxtheme.hpp>
#include <iostream>

HTHEME				(WINAPI*	OpenNcThemeData)(HWND, LPCWSTR);
void				(WINAPI*	RefreshImmersiveColorPolicyState)();
bool				(WINAPI*	GetIsImmersiveColorUsingHighContrast)(IMMERSIVE_HC_CACHE_MODE);
bool				(WINAPI*	ShouldAppsUseDarkMode)();
PREFERREDAPPMODE	(WINAPI*	SetPreferredAppMode)(PREFERREDAPPMODE);
bool				(WINAPI*	AllowDarkModeForWindow)(HWND, bool);
bool				(WINAPI*	IsDarkModeAllowedForWindow)();
BOOL				(WINAPI*	SetWindowCompositionAttribute)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);


AMTRS_OS_WIN32_NAMESPACE_BEGIN

static bool		sInitEngine		= false;
static DWORD	sBuildNumber	= 0;
static HMODULE	sNTDll			= NULL;
static HMODULE	sUxThemeDll		= NULL;

template<class Fn, class... Args>
void load_fn(Fn& _fn, HMODULE _module, Args&&... _args)
{
	_fn	= reinterpret_cast<Fn>(GetProcAddress(_module, std::forward<Args>(_args)...));
}

bool init_uxtheme()
{
	if (sInitEngine)
	{
		return	true;
	}
	sInitEngine	= true;

	sNTDll					= GetModuleHandleW(L"ntdll.dll");
	if (!sNTDll)
	{
		return	false;
	}
	sUxThemeDll = LoadLibraryExW(L"uxtheme.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
	if (!sUxThemeDll)
	{
		return	false;
	}
	using	FnRtlGetNtVersionNumbers	= void (WINAPI *)(LPDWORD major, LPDWORD minor, LPDWORD build);
	auto	RtlGetNtVersionNumbers		= reinterpret_cast<FnRtlGetNtVersionNumbers>(GetProcAddress(sNTDll, "RtlGetNtVersionNumbers"));
	if (!RtlGetNtVersionNumbers)
	{
		return	false;
	}

	DWORD	major, minor, build;
	RtlGetNtVersionNumbers(&major, &minor, &build);
	// smaller Windows 10 19H2 (November 2019 Update)
	if ((major < 10) || (major == 10 && minor == 0 && build < 18363))
	{
		return	false;
	}
	sBuildNumber	= build;

	// Get API
	load_fn(OpenNcThemeData,						sUxThemeDll, MAKEINTRESOURCEA(49));
	load_fn(RefreshImmersiveColorPolicyState,		sUxThemeDll, MAKEINTRESOURCEA(104));
	load_fn(GetIsImmersiveColorUsingHighContrast,	sUxThemeDll, MAKEINTRESOURCEA(106));
	load_fn(ShouldAppsUseDarkMode,					sUxThemeDll, MAKEINTRESOURCEA(132));
	load_fn(SetPreferredAppMode,					sUxThemeDll, MAKEINTRESOURCEA(135));
	load_fn(AllowDarkModeForWindow,					sUxThemeDll, MAKEINTRESOURCEA(133));
	load_fn(IsDarkModeAllowedForWindow,				sUxThemeDll, MAKEINTRESOURCEA(137));
	load_fn(SetWindowCompositionAttribute,			GetModuleHandleW(L"user32.dll"), "SetWindowCompositionAttribute");

	// Dark mode enable.
	SetPreferredAppMode(APPMODE_ALLOWDARK);
	RefreshImmersiveColorPolicyState();
	ShouldAppsUseDarkMode();

	return	true;
}

bool allow_theme(HWND _hwnd, PREFERREDAPPMODE _allow)
{
	init_uxtheme();
	if (!SetPreferredAppMode)
	{
		return	false;
	}
	AllowDarkModeForWindow(_hwnd, TRUE);

	bool	dark 			= true;

	WINDOWCOMPOSITIONATTRIBDATA data = { WCA_USEDARKMODECOLORS, &dark, sizeof(dark) };
	SetWindowCompositionAttribute(_hwnd, &data);

/*
	bool			dark 			= false;
	HIGHCONTRASTW	highContrast	= { sizeof(highContrast) };
	if (SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(highContrast), &highContrast, FALSE))
	{
		dark	= highContrast.dwFlags & HCF_HIGHCONTRASTON
				? true
				: false;
	}

		dark 			= true;

	WINDOWCOMPOSITIONATTRIBDATA data = { WCA_USEDARKMODECOLORS, &dark, sizeof(dark) };
	SetWindowCompositionAttribute(_hwnd, &data);
*/	return	true;
}

/*

static DWORD	sBuildNumber	= 0;
static HMODULE	sNTDll			= NULL;
static HMODULE	sUxThemeDll		= NULL;

bool is_support_darkmode()
{
	if (!sNTDll)
	{
		sNTDll					= GetModuleHandleW(L"ntdll.dll");
		if (!sNTDll)
		{
			return	false;
		}
	}
	using	FnRtlGetNtVersionNumbers	= void (WINAPI *)(LPDWORD major, LPDWORD minor, LPDWORD build);
	auto	RtlGetNtVersionNumbers		= reinterpret_cast<FnRtlGetNtVersionNumbers>(GetProcAddress(sNTDll, "RtlGetNtVersionNumbers"));
	if (!RtlGetNtVersionNumbers)
	{
		return	false;
	}

	DWORD	major, minor, build;
	RtlGetNtVersionNumbers(&major, &minor, &build);
	// smaller Windows 10 19H2 (November 2019 Update)
	if ((major < 10) || (major == 10 && minor == 0 && build < 18363))
	{
		return	false;
	}
	sBuildNumber	= build;
	return	true;
}


bool is_darkmode()
{
	if (!sUxThemeDll)
	{
		sUxThemeDll = LoadLibraryExW(L"uxtheme.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
		if (!sUxThemeDll)
		{
			return	false;
		}
	}

	using	FnOpenNcThemeData						= HTHEME(WINAPI *)(HWND, LPCWSTR);			// 49
	using	FnRefreshImmersiveColorPolicyState		= void(WINAPI *)();							// 104
	using	FnGetIsImmersiveColorUsingHighContrast	= bool(WINAPI *)(IMMERSIVE_HC_CACHE_MODE);	// 106
	using	FnShouldAppsUseDarkMode					= bool(WINAPI *)();							// 132


	auto	OpenNcThemeData							= reinterpret_cast<FnOpenNcThemeData>(GetProcAddress(sUxThemeDll, MAKEINTRESOURCEA(49)));
	auto	RefreshImmersiveColorPolicyState		= reinterpret_cast<FnRefreshImmersiveColorPolicyState>(GetProcAddress(sUxThemeDll, MAKEINTRESOURCEA(104)));
	auto	GetIsImmersiveColorUsingHighContrast	= reinterpret_cast<FnGetIsImmersiveColorUsingHighContrast>(GetProcAddress(sUxThemeDll, MAKEINTRESOURCEA(106)));
	auto	ShouldAppsUseDarkMode					= reinterpret_cast<FnShouldAppsUseDarkMode>(GetProcAddress(sUxThemeDll, MAKEINTRESOURCEA(132)));
	auto	SetPreferredAppMode						= reinterpret_cast<PREFERREDAPPMODE(WINAPI *)(PREFERREDAPPMODE appMode)>(GetProcAddress(sUxThemeDll, MAKEINTRESOURCEA(135)));
	auto	IsDarkModeAllowedForWindow				= reinterpret_cast<bool(WINAPI*)()>(GetProcAddress(sUxThemeDll, MAKEINTRESOURCEA(137)));
	auto	SetWindowCompositionAttribute			= reinterpret_cast<BOOL(WINAPI*)(HWND, WINDOWCOMPOSITIONATTRIBDATA*)>(GetProcAddress(GetModuleHandleW(L"user32.dll"), "SetWindowCompositionAttribute"));

	SetPreferredAppMode(APPMODE_ALLOWDARK);
	RefreshImmersiveColorPolicyState();
	ShouldAppsUseDarkMode();
	return	true;
}

*/
AMTRS_OS_WIN32_NAMESPACE_END


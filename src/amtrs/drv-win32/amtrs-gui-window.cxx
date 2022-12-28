/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <dwmapi.h>
#include <shellapi.h>
#include <uxtheme.h>

#include <unknwn.h>
#include <winrt/base.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Composition.Desktop.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <windows.ui.composition.interop.h>
#include <DispatcherQueue.h>

#include "amtrs-gui-window.hpp"

#pragma	comment(lib, "Dwmapi.lib")
#pragma	comment(lib, "windowsapp.lib")

// https://learn.microsoft.com/ja-jp/windows/apps/desktop/modernize/apply-windows-themes
#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif

using namespace winrt;
using namespace Windows::Foundation::Numerics;
using namespace Windows::System;
using namespace Windows::UI;
using namespace Windows::UI::Composition;
using namespace Windows::UI::Composition::Desktop;
using namespace Windows::UI::ViewManagement;

AMTRS_OS_NAMESPACE_BEGIN

LRESULT window::wndproc(HWND _hwnd, UINT _msg, WPARAM _wparam, LPARAM _lparam)
{
	switch (_msg)
	{
		case WM_CREATE :
		{
			mWnd	= _hwnd;
/*
			// dispatch queue
			DispatcherQueueOptions	options;
			options.dwSize			= sizeof(DispatcherQueueOptions);
			options.threadType		= DQTYPE_THREAD_CURRENT;
			options.apartmentType	= DQTAT_COM_ASTA;
			Windows::System::DispatcherQueueController	controller{ nullptr };
			check_hresult(CreateDispatcherQueueController(options, reinterpret_cast<ABI::Windows::System::IDispatcherQueueController**>(put_abi(controller))));
			mDispatcherQueueController	= controller;
			mCompositor					= winrt::Windows::UI::Composition::Compositor();
			// 
			auto	interop = mCompositor.as<ABI::Windows::UI::Composition::Desktop::ICompositorDesktopInterop>();
			check_hresult(interop->CreateDesktopWindowTarget(mWnd, false, reinterpret_cast<ABI::Windows::UI::Composition::Desktop::IDesktopWindowTarget**>(put_abi(mTarget))));
			auto	root = mCompositor.CreateContainerVisual();
			root.RelativeSizeAdjustment({ 1.0f, 1.0f });
			root.Offset({ 124, 12, 0 });
			mTarget.Root(root);
*/
			if (mListener)
			{
				mListener->on_created(this);
			}
			break;
		}
		case WM_NCDESTROY :
		{
			if (mListener)
			{
				mListener->on_destroyed(this);
			}
			SetWindowLongPtr(_hwnd, GWLP_USERDATA, (LONG_PTR)nullptr);
			PostQuitMessage(0);
			break;
		}
		case WM_SIZE :
		{
			mSize	= { LOWORD(_lparam), LOWORD(_lparam) };
			if (mListener)
			{
				mListener->on_size(this, mSize, false);
			}
			break;
		}

		case WM_PAINT :
		{
			auto		callback	= std::move(mRepaint);
			PAINTSTRUCT	ps;
			HDC			hdc	= BeginPaint(_hwnd, &ps);
			if (!callback.empty())
			{
				callback();
			}
			EndPaint(_hwnd, &ps);
			break;
		}
	}
	return	DefWindowProc(_hwnd, _msg, _wparam, _lparam);
}


auto window::repaint(repaint_callback _callback) -> bool
{
	if (!mRepaint.empty())
	{
		return	false;
	}
	mRepaint	= std::move(_callback);
	InvalidateRect(mWnd, NULL, false);
	return	true;
}

void window::create(createparams& _cp, ref<window> _parent)
{
	// デスクトップウィンドウからダークモードかどうか判別する
	auto	currentTheme	= themes::create();
	auto	bgcolor			= currentTheme->background();
	mClassName	= gen_classname();
	mListener	= _cp.listener;
	WNDCLASSEX	wcls;
	wcls.cbSize			= sizeof(wcls);
	wcls.style			= CS_HREDRAW | CS_VREDRAW;
	wcls.cbClsExtra		= 0;
	wcls.cbWndExtra		= sizeof(window*);
	wcls.hInstance		= GetModuleHandle(nullptr);
	wcls.hIcon			= LoadIcon(GetModuleHandle(NULL), IDI_APPLICATION);
	wcls.hCursor		= LoadCursor(nullptr, IDC_ARROW);
	wcls.hbrBackground	= CreateSolidBrush(RGB(bgcolor.r, bgcolor.g, bgcolor.b));
	wcls.lpszMenuName	= nullptr;
	wcls.lpszClassName	= mClassName.c_str();
	wcls.hIconSm		= LoadIcon(GetModuleHandle(NULL), IDI_APPLICATION);
	wcls.lpfnWndProc	= [](HWND _hwnd, UINT _msg, WPARAM _wparam, LPARAM _lparam) -> LRESULT
	{
		LONG_PTR		ptr		= GetWindowLongPtr(_hwnd, GWLP_USERDATA);
		if (_msg == WM_NCCREATE)
		{
			CREATESTRUCT*	cs		= reinterpret_cast<CREATESTRUCT*>(_lparam);
			window*			thiz	= reinterpret_cast<window*>(cs->lpCreateParams);
			SetWindowLongPtr(_hwnd, GWLP_USERDATA, (LONG_PTR)thiz);
			thiz->mDC	= GetDC(_hwnd);
			ptr	= (LONG_PTR)thiz;
		//	os::win32::allow_theme(_hwnd, APPMODE_ALLOWDARK);
		//	SetWindowTheme(_hwnd, L"Explorer", NULL);
		}
		if (ptr)
		{
			return	reinterpret_cast<window*>(ptr)->wndproc(_hwnd, _msg, _wparam, _lparam);
		}
		return	DefWindowProc(_hwnd, _msg, _wparam, _lparam);
	}; // wcls.lpfnWndProc
	if (!RegisterClassEx(&wcls))
	{
		return;
	}
	DWORD	dwStyle		= WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN;
	DWORD	dwExStyle	= WS_EX_APPWINDOW;
	if (_parent)
	{
		dwStyle		= WS_CHILDWINDOW;
		dwExStyle	= WS_EX_TRANSPARENT;
	}
	DWORD	dwWidth		= CW_USEDEFAULT;
	DWORD	dwHeight	= CW_USEDEFAULT;
	if (_cp.size.width != use_default || _cp.size.height != use_default)
	{
		RECT	rect;
		rect.left	= 0;
		rect.top	= 0;
		rect.right	= _cp.size.width  != use_default ? (DWORD)_cp.size.width  : 100;
		rect.bottom	= _cp.size.height != use_default ? (DWORD)_cp.size.height : 100;
		AdjustWindowRectEx(&rect, dwStyle, FALSE, dwExStyle);
		if (_cp.size.width  != use_default)
		{
			dwWidth		= rect.right - rect.left;
		}
		if (_cp.size.height != use_default)
		{
			dwHeight	= rect.bottom - rect.top;
		}
	}
	mWnd	= CreateWindowEx(
				dwExStyle,
				wcls.lpszClassName,
				_cp.title.c_str(),
				dwStyle,
				CW_USEDEFAULT, CW_USEDEFAULT, dwWidth, dwHeight,
				_parent ? static_cast<window*>(_parent.get())->mWnd : NULL,
				nullptr,
				wcls.hInstance,
				this
			);
	if (!mWnd)
	{
//		throw	std::system_error(os::win32::make_last_error_code(), "CreateWindowEx()");
	}
	// Dark mode / light mode
	BOOL	isDarkMode	= currentTheme->type() == themes::darkmode;
	::DwmSetWindowAttribute(mWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &isDarkMode, sizeof(isDarkMode));
	ShowWindow(mWnd, TRUE);
	UpdateWindow(mWnd);
}


sstring window::gen_classname()
{
	static constexpr char	c[] = "0123456789abcdef";
	uintmax_t	num	= reinterpret_cast<uintmax_t>(this);
	char		name[sizeof(num) * 2];
	for (int i = 0; i < sizeof(num); ++i)
	{
		name[i * 2 + 0]	= c[ num     % 0x0f];
		name[i * 2 + 1]	= c[(num>>4) % 0x0f];
		num >>= 8;
	}
	return	sstring(name, sizeof(num) * 2);
}
AMTRS_OS_NAMESPACE_END
AMTRS_NAMESPACE_BEGIN
auto window::create(createparams& _cp, ref<window> _parent) -> ref<window>
{
	ref<os::window>	thiz	= new os::window();
	thiz->create(_cp, _parent ? static_cast<os::window*>(_parent.get()) : nullptr);
	return	thiz.get();
}

auto window::size() const noexcept -> window_size_type { return static_cast<os::window const*>(this)->mSize; }
auto window::repaint(repaint_callback _repaint) -> bool { return static_cast<os::window*>(this)->repaint(std::move(_repaint)); }

AMTRS_NAMESPACE_END

/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__driver__windows__api_xaml__hpp
#define	__libamtrs__driver__windows__api_xaml__hpp
#include ".api-windows.hpp"
#include <DispatcherQueue.h>
#include <winrt\base.h>
#include <winrt\Windows.UI.Xaml.Hosting.h>
//#include <Windows.UI.Composition.Interop.h>
//#include <Windows.UI.Xaml.Hosting.Desktopwindowxamlsource.h>

// winrt.xaml を使うには XXXX.exe.manifest をプロジェクトに追加する必要があります。
AMTRS_OS_WIN32_NAMESPACE_BEGIN

class	xamlisland
{
public:
	using	DesktopWindowXamlSource	= winrt::Windows::UI::Xaml::Hosting::DesktopWindowXamlSource;

	~xamlisland()
	{
		close();
	}


	void initialize(HWND _owner = nullptr)
	{
		mIsland	= island::sInstance;
		if (!mIsland)
		{
			HWND		own	= _owner
							? _owner
							: static_cast<g3d::window_win32_opengl*>(window::get_first_instance())->hwnd();
			mIsland			= new island();
			mIsland->xaml	= winrt::Windows::UI::Xaml::Hosting::DesktopWindowXamlSource{};
			auto	interop	= mIsland->xaml.as<IDesktopWindowXamlSourceNative>();
			interop->AttachToWindow(own);					// 親ウィンドウにアタッチ
			interop->get_WindowHandle(&mIsland->window);	// XamlIslandのウィンドウを取得する
		}
	}

	void close()
	{
		if (hwnd())
		{
			ShowWindow(hwnd(), SW_HIDE);
		}
	}


	HWND hwnd() const noexcept
	{
		return	mIsland ? mIsland->window : (HWND)nullptr;
	}

	DesktopWindowXamlSource& containor() noexcept
	{
		return	mIsland->xaml;
	}

private:
	struct	island : ref_object
	{
		DesktopWindowXamlSource	xaml{nullptr};
		HWND					window{nullptr};

		island()
		{
			sInstance = this;
		}

		~island()
		{
			if (xaml)
			{
				xaml.Close();
			}
			sInstance = nullptr;
		}

		static inline island*	sInstance;
	};
	ref<island>					mIsland;
};


template<class XamlT>
class	xamlcontrol
		: public xamlisland
{
public:
	using	contents_type	= XamlT;

	void initialize(HWND _owner = nullptr)
	{
		xamlisland::initialize(_owner);
		xamlisland::containor().Content(mContent = contents_type());
	}

	contents_type& content() noexcept
	{
		return	mContent;
	}

private:
	contents_type		mContent{nullptr};
};



class	xampldialog
		: public xamlcontrol<winrt::Windows::UI::Xaml::Controls::Canvas>
{
public:
	using	ContentDialog	= winrt::Windows::UI::Xaml::Controls::ContentDialog;
	using	StackPanel		= winrt::Windows::UI::Xaml::Controls::StackPanel;
	using	super_type		= os::win32::xamlcontrol<winrt::Windows::UI::Xaml::Controls::Canvas>;

	~xampldialog()
	{
		close();
	}

	void close()
	{
		if (mDialog)
		{
			mDialog.Hide();
			mDialog	= nullptr;
		}
		super_type::close();
	}

	template<class Callback>
	void initialize(std::vector<wchar_t const*> _menus, int _cancel, Callback&& _callback)
	{
		super_type::initialize();

		RECT	r;
		GetClientRect(GetParent(super_type::hwnd()), &r);
		SetWindowPos(super_type::hwnd(), HWND_TOP, 0, 0, r.right - r.left, r.bottom - r.top, SWP_SHOWWINDOW);

		mDialog	= ContentDialog();
		mDialog.Content(mPanel = StackPanel());

		int	id		= 0;
		int	bnum	= 0;
		for (auto const& t : _menus)
		{
			if (id == _cancel)
			{
				mDialog.CloseButtonText(winrt::hstring(t));
				mDialog.CloseButtonClick([=](auto sender, auto args)
				{
					_callback(id);
				});
			}
			else
			{
				if (bnum == 0)
				{
					mDialog.PrimaryButtonText(winrt::hstring(t));
					mDialog.PrimaryButtonClick([=](auto sender, auto args)
					{
						_callback(id);
					});
					++bnum;
				}
				else
				{
					mDialog.SecondaryButtonText(winrt::hstring(t));
					mDialog.SecondaryButtonClick([=](auto sender, auto args)
					{
						_callback(id);
					});
					++bnum;
				}
			}
			++id;
		}
	}

	ContentDialog& dialog()
	{
		return	mDialog;
	}

	StackPanel& panel()
	{
		return	mPanel;
	}

	void show()
	{
		mDialog.XamlRoot(super_type::content().XamlRoot());
		mDialog.ShowAsync();
	}

protected:
	ContentDialog		mDialog{nullptr};
	StackPanel			mPanel{nullptr};
};


AMTRS_OS_WIN32_NAMESPACE_END
#endif

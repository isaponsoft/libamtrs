/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__application__application_winrt__hpp
#define	__libamtrs__application__application_winrt__hpp
#include <windows.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Input.h>
AMTRS_NAMESPACE_BEGIN
namespace winrt {

using namespace ::winrt::Windows;
using namespace ::winrt::Windows::ApplicationModel::Core;
using namespace ::winrt::Windows::Foundation::Numerics;
using namespace ::winrt::Windows::UI;
using namespace ::winrt::Windows::UI::Core;
using namespace ::winrt::Windows::UI::Composition;

struct	App : ::winrt::implements<App, IFrameworkViewSource, IFrameworkView>
{
	App(application* _application)
		: mApplication(_application)
	{}

    IFrameworkView CreateView()
    {
		AMTRS_DEBUG_LOG("CreateView");
        return	*this;
    }

    void Initialize(CoreApplicationView const &)
    {
		AMTRS_DEBUG_LOG("Initialize");
    }

    void Load(::winrt::hstring const&)
    {
		AMTRS_DEBUG_LOG("Load");
		mApplication->on_created(0, nullptr);
		
    }

    void Uninitialize()
    {
		AMTRS_DEBUG_LOG("Uninitialize");
    }

    void Run()
    {
        CoreWindow window = CoreWindow::GetForCurrentThread();
        window.Activate();

        CoreDispatcher dispatcher = window.Dispatcher();
        dispatcher.ProcessEvents(CoreProcessEventsOption::ProcessUntilQuit);
    }

    void SetWindow(CoreWindow const & window)
    {
		AMTRS_DEBUG_LOG("SetWindow");

        Compositor compositor;
        ContainerVisual root = compositor.CreateContainerVisual();

		window.Activated([this](IInspectable const&, WindowActivatedEventArgs const& _args)
		{
			switch (_args.WindowActivationState())
			{
				case CoreWindowActivationState::CodeActivated :
				{
					AMTRS_DEBUG_LOG("CodeActivated");
					mApplication->on_event(application::system_event::resume);
					break;
				}

				case CoreWindowActivationState::Deactivated :
				{
					AMTRS_DEBUG_LOG("Deactivated");
					mApplication->on_event(application::system_event::pause);
					break;
				}

				case CoreWindowActivationState::PointerActivated :
				{
					AMTRS_DEBUG_LOG("PointerActivated");
					break;
				}
			}
		});

		window.PointerPressed([](IInspectable const &, PointerEventArgs const & args)
		{
			float2 const point = args.CurrentPoint().Position();
			AMTRS_DEBUG_LOG("Pressed %fx%f", point.x, point.y);
		});

        window.PointerMoved([](IInspectable const &, PointerEventArgs const & args)
		{
			float2 const point = args.CurrentPoint().Position();
			AMTRS_DEBUG_LOG("Moved %fx%f", point.x, point.y);
		});

        window.PointerReleased([&](auto && ...)
        {
        });

        window.SizeChanged([&](auto && ...)
        {
        });

        window.VisibilityChanged([&](auto && ...)
        {
        });
    }

private:
	application*	mApplication;
};

} // winrt



inline std::string application::cache_dir() const
{
	return	".";
}


inline std::string application::documents_dir() const
{
	return	{};
}



inline std::string application::files_dir(bool _external) const
{
	(void)_external;
	return	".";
}


inline void application::foreground()
{
}


inline int application::run(int _argc, char* _args[])
{
	this->on_created(_argc, _args);
    ::winrt::Windows::ApplicationModel::Core::CoreApplication::Run(::winrt::make<winrt::App>(this));
	return	0;
}



AMTRS_NAMESPACE_END
#endif

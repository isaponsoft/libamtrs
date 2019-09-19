/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
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
		AMTRS_TRACE_LOG("CreateView");
        return	*this;
    }

    void Initialize(CoreApplicationView const &)
    {
		AMTRS_TRACE_LOG("Initialize");
    }

    void Load(::winrt::hstring const&)
    {
		AMTRS_TRACE_LOG("Load");
		mApplication->on_created(0, nullptr);
		
    }

    void Uninitialize()
    {
		AMTRS_TRACE_LOG("Uninitialize");
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
		AMTRS_TRACE_LOG("SetWindow");

        Compositor compositor;
        ContainerVisual root = compositor.CreateContainerVisual();

		window.Activated([this](IInspectable const&, WindowActivatedEventArgs const& _args)
		{
			switch (_args.WindowActivationState())
			{
				case CoreWindowActivationState::CodeActivated :
				{
					AMTRS_TRACE_LOG("CodeActivated");
					mApplication->on_event(application::system_event::resume);
					break;
				}

				case CoreWindowActivationState::Deactivated :
				{
					AMTRS_TRACE_LOG("Deactivated");
					mApplication->on_event(application::system_event::pause);
					break;
				}

				case CoreWindowActivationState::PointerActivated :
				{
					AMTRS_TRACE_LOG("PointerActivated");
					break;
				}
			}
		});

		window.PointerPressed([](IInspectable const &, PointerEventArgs const & args)
		{
			float2 const point = args.CurrentPoint().Position();
			AMTRS_TRACE_LOG("Pressed %fx%f", point.x, point.y);
		});

        window.PointerMoved([](IInspectable const &, PointerEventArgs const & args)
		{
			float2 const point = args.CurrentPoint().Position();
			AMTRS_TRACE_LOG("Moved %fx%f", point.x, point.y);
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



inline filesystem::path application::cache_dir() const
{
	return	".";
}


inline filesystem::path application::documents_dir() const
{
	return	{};
}



inline filesystem::path application::files_dir(bool _external) const
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

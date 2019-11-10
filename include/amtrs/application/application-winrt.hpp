/******************************************************************************
Copyright (c) 2019, isaponsoft (Isao Shibuya)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the libamtrs project.
******************************************************************************/
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

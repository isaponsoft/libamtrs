/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <ctime>
#include <shellapi.h>
#include <amtrs/chrono/framerater.hpp>
#include <amtrs/gui/application.hpp>
#include <amtrs/gui/window.hpp>
#include <amtrs/string/convert.hpp>
#include <amtrs/system/sleep.hpp>


AMTRS_OS_NAMESPACE_BEGIN
class	application : public AMTRS_NAMESPACE::application
{
public:
	static inline application*	sThis	= nullptr;
	listener*		mListener	= nullptr;
	int				mExitCode	= 0;
	bool			mQuit		= false;
	ref<window>		mWindow;
	framerater		mFPS;

	static application& self() { return *sThis; }

	int start(HINSTANCE hInstance, HINSTANCE hPrevInstance, int _argc, char** _argv)
	{
		initialize(hInstance, hPrevInstance, _argc, _argv);
		if (is_quit())	return	mExitCode;
		mainloop();
		return	mExitCode;
	}

	void initialize(HINSTANCE hInstance, HINSTANCE hPrevInstance, int _argc, char** _argv)
	{
		createparams	cp;
		int				exitcode;
		sThis		= this;
		mListener	= create_listener();

		exitcode	= mListener->on_creating(cp, _argc, _argv);
		if (exitcode)
		{
			quit(exitcode);
			return;
		}

		mWindow		= window::create(cp.wcp);
		exitcode	= mListener->on_created(mWindow);
		if (exitcode)
		{
			quit(exitcode);
			return;
		}
	}

	void mainloop()
	{
		MSG		msg;
		HANDLE	msgReady	= CreateEvent(NULL, FALSE, FALSE, NULL);
		while (!is_quit())
		{
			while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
				{
					mListener->on_quit();
					return;
				}
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			double	wait	= mFPS.waittime();
			if (wait > 0)
			{
				system::sleep_ns(static_cast<int>(wait * 1000.0 * 1000.0));
			}

			int	e	= mListener->on_update(0);
			if (e)
			{
				quit(e);
			}
		}
	}

	void update_limit_fps(int _fps)
	{
		mFPS.fps(_fps);
	}

	void foreground()
	{
	}

	void quit(int _exitcode)
	{
		mExitCode	= _exitcode;
		mQuit		= true;
	}

	bool is_quit()
	{
		return	mQuit;
	}

};
AMTRS_OS_NAMESPACE_END AMTRS_NAMESPACE_BEGIN
void application::foreground() { os::application::self().foreground(); }
void application::quit(int _exitcode) { os::application::self().quit(_exitcode); }
bool application::is_quit() { return os::application::self().is_quit(); }
void application::update_limit_fps(int _fps) { return os::application::self().update_limit_fps(_fps); }
AMTRS_NAMESPACE_END AMTRS_OS_NAMESPACE_BEGIN


int win32_main(int _argc, char** _argv)
{
	application	app;
	return	app.start(GetModuleHandle(NULL), nullptr, _argc, _argv);
}

int win32_wmain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
	int			argc	= 0;
	wchar_t**	argv	= CommandLineToArgvW(GetCommandLineW(), &argc);

	simplearray<sstring>	args;
	for (int i = 0; i < argc; ++i)
	{
		args[i]	= wc_to_c8(argv[i]);
	}
	LocalFree(argv);
	application	app;
	return	app.start(hInstance, hPrevInstance, static_cast<int>(args.size()), reinterpret_cast<char**>(&*args.begin()));
}

AMTRS_OS_NAMESPACE_END

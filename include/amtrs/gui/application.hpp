/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__gui__application__hpp
#define	__libamtrs__gui__application__hpp
#include "../memory/ref_object.hpp"
#include "../string/sstring.hpp"
#include "../util/callback.hpp"
#include "window.hpp"
AMTRS_NAMESPACE_BEGIN
class	application
{
public:
	struct	createparams
	{
		sstring					title;			// アプリケーションのタイトルを指定します。主にメインウィンドウのタイトルバーに使用されます。
		window::createparams	wcp;			// メインウィンドウの作成情報を指定します。
	};

	class	listener
	{
	public:
		using	createparams	= application::createparams;

		// アプリケーションの初期化処理を開始する直前に呼ばれます。
		// _params に適切な値を設定してください。
		// return
		//		0    : success
		//		other: error exit code.
		virtual int	on_creating		(createparams& _params, int _argc, char** _argv) { return 0; }

		// アプリケーションの初期化処理が完了したときに呼ばれます。
		// return
		//		0    : success
		//		other: error exit code.
		virtual int	on_created		(ref<window> _window) { return 0; }

		virtual int on_update		(double _elapsed_ms) { return 0; }
		virtual int on_restart		() { return 0; }
		virtual int on_start		() { return 0; }
		virtual int on_resume		() { return 0; }
		virtual int on_pause		() { return 0; }
		virtual int on_stop			() { return 0; }
		virtual int on_quit			() { return 0; }
		virtual int on_destroyed	() { return 0; }
	};

	static void foreground();
	static void quit(int _exitcode);
	static bool is_quit();
	static void update_limit_fps(int _fps);

protected:
	static listener* create_listener();
};


#define	AMTRS_APPLICATIN_CLASS(_listener_classname)	\
	AMTRS_NAMESPACE_BEGIN\
	auto application::create_listener() -> listener*\
	{\
		return	new _listener_classname();\
	}\
	AMTRS_NAMESPACE_END\
	AMTRS_APPLICATIN_ENTRY(_listener_classname)



AMTRS_NAMESPACE_END
#include AMTRS_PLATFORM_INCLUDE(application.hxx)
#endif

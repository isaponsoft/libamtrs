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
#ifndef	__libamtrs__application__application__hpp
#define	__libamtrs__application__application__hpp
#include <algorithm>
#include <functional>
#include <list>
#include "../amtrs.hpp"
#include "../filesystem/@"
AMTRS_NAMESPACE_BEGIN

class	appcontroller;

// ============================================================================
//! GUIアプリケーションにおいて、アプリケーションのイベントを受け取るための基底クラス。
// ----------------------------------------------------------------------------
//! GUIアプリケーションを開発する場合は AMTRS_GUI_MAIN(クラス名) で、
//! このクラスを継承したクラス名を指定します。
// ----------------------------------------------------------------------------
class	application
		: public ref_object
{
public:
	enum class	system_event
	{
		restart,				//!< stop状態だったアプリケーションが再開したとき、startの直前に呼ばれる。
		start,					//!< アプリケーションが開始されたときに呼ばれる。

		// activate
		resume,					//!< 

		// deactivate
		pause,					//!< ほかのウィンドウがアクティブになったため一時停止を要求した
								//!< 動画や音楽プレイヤー、リアルタイム通信の必要なアプリ以外は
								//!< 更新処理を止めることが推奨される。


		stop,					//!< アプリケーションが停止状態になった。
								//!< 例えば、スマートホンにおけるバックグラウンドタスクへの移行などが該当する。

		quit,					//!< quit() が呼ばれた。
		destroyed,				//!< applicationが破棄された(applicationが無効になった)
	};

	//! イベント受け取り用のコールバックの型
	using	listener_type	= std::function<void(system_event)>;


	// ========================================================================
	//! アプリケーションを取得します。
	// ------------------------------------------------------------------------
	//! AMTRS_GUI_MAIN() で設定したクラスにstatic_cast出来ます。
	// ------------------------------------------------------------------------
	static application* get_application() noexcept { return sApplication; }


	application();
	~application();

	virtual void on_created(int /*_argc*/, char* _args[]) { (void)_args; }

	virtual void on_update() {}

	virtual void on_event(system_event _event);

	// ========================================================================
	//! フォアグラウンドへの遷移を試みます。
	// ------------------------------------------------------------------------
	//! プラットフォームによっては無視されます。
	// ------------------------------------------------------------------------
	void foreground();

	// ========================================================================
	//! アプリケーションの終了リクエストを発行します。
	// ------------------------------------------------------------------------
	void quit();


	// ========================================================================
	//! quit() が呼ばれた後かどうか取得します。
	// ------------------------------------------------------------------------
	bool is_quit() const noexcept { return mQuit; }


	// ========================================================================
	//! アプリケーションのイベントが発生した時のコールバックを設定します。
	// ------------------------------------------------------------------------
	//!	_key
	//!		remove_event_listener() でリスナを削除するためのキー。
	//!	_func
	//!		on_event() で呼び出したいリスナー関数。
	// ------------------------------------------------------------------------
	template<class FuncT>
	void add_event_listener(void* _key, FuncT&& _func);


	// ========================================================================
	//! add_event_listener() で登録したリスナを削除します。
	// ------------------------------------------------------------------------
	//!	_key
	//!		add_event_listener() で指定した _key
	// ------------------------------------------------------------------------
	//!	return
	//!		リスナが見つからなければ false。削除したなら true。
	// ------------------------------------------------------------------------
	bool remove_event_listener(void* _key);


	// ========================================================================
	//! キャッシュデータを保存すべきパスを取得します。
	// ------------------------------------------------------------------------
	filesystem::path cache_dir() const;


	// ========================================================================
	//! 永続化領域のパスを取得します。アンインストール時に削除されます。
	// ------------------------------------------------------------------------
	//! 通常は（OSによって保護されている場合は）ユーザーから隠された
	//! アプリケーションのプライベートなファイルになります。
	//! _external
	//!		true の時は利用可能なら外部ストレージを優先して使用するようにします。
	// ------------------------------------------------------------------------
	filesystem::path files_dir(bool _external = true) const;


	// ========================================================================
	//! ドキュメントフォルダを取得します。
	// ------------------------------------------------------------------------
	//! Androidなどの一部のOSではユーザーにアクセス許可を求める場合があります。
	//! その場合、アクセス許可が得られなかった場合は empty() を返します。
	//! また、アプリが終了した場合は例外をスローします。
	// ------------------------------------------------------------------------
	filesystem::path documents_dir() const;


	// ========================================================================
	//! アプリケーションを実行します。
	// ------------------------------------------------------------------------
	int run(int _argc, char* _args[]);
	int run() { return run(0, nullptr); }

private:
	struct	listener_data
	{
		void*			key;
		listener_type	listener;
		bool			removed		= false;
	};


	static inline application*		sApplication		= nullptr;
	bool							mQuit				= false;
	bool							mListenerRebuild	= false;
	std::list<listener_data>		mListenerList;

	friend	appcontroller;
};


inline application::application()
{
	//AMTRS_DEBUG_LOG("amtrs application created.");
	sApplication	= this;
}



inline application::~application()
{
	//AMTRS_DEBUG_LOG("amtrs application destroy.");
	on_event(system_event::destroyed);
	sApplication	= nullptr;
}




inline void application::on_event(system_event _event)
{
	for (auto& e : mListenerList)
	{
		if (!e.removed)
		{
			e.listener(_event);
		}
	}
	if (mListenerRebuild)
	{
		mListenerRebuild	= false;
		auto	it	= std::remove_if(mListenerList.begin(), mListenerList.end(), [](auto& _e) -> bool { return _e.removed; });
		if (mListenerList.end() != it)
		{
			mListenerList.erase(it, mListenerList.end());
		}
	}
}

inline void application::quit()
{
	if (!mQuit)
	{
		mQuit	= true;
		on_event(system_event::quit);
	}
}


template<class FuncT>
void application::add_event_listener(void* _key, FuncT&& _func)
{
	mListenerList.push_back({_key, std::forward<FuncT>(_func), false});
}


inline bool application::remove_event_listener(void* _key)
{
	bool	match	= false;
	for (auto& e : mListenerList)
	{
		if (e.key == _key)
		{
			mListenerRebuild	= true;
			match				= true;
			e.removed			= true;
		}
	}
	return	match;
}




AMTRS_NAMESPACE_END
#include AMTRS_MAKE_INCLUDE_PLATFORM(application)
#endif

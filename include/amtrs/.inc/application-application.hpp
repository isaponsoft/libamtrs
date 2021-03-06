﻿/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__application__application__hpp
#define	__libamtrs__application__application__hpp
#include <algorithm>
#include <functional>
#include <list>
#include "../amtrs.hpp"
#include "../filesystem.hpp"
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
	std::string cache_dir() const;


	// ========================================================================
	//! 永続化領域のパスを取得します。アンインストール時に削除されます。
	// ------------------------------------------------------------------------
	//! 通常は（OSによって保護されている場合は）ユーザーから隠された
	//! アプリケーションのプライベートなファイルになります。
	//! _external
	//!		true の時は利用可能なら外部ストレージを優先して使用するようにします。
	// ------------------------------------------------------------------------
	std::string files_dir(bool _external = true) const;


	// ========================================================================
	//! ドキュメントフォルダを取得します。
	// ------------------------------------------------------------------------
	//! Androidなどの一部のOSではユーザーにアクセス許可を求める場合があります。
	//! その場合、アクセス許可が得られなかった場合は empty() を返します。
	//! また、アプリが終了した場合は例外をスローします。
	// ------------------------------------------------------------------------
	std::string documents_dir() const;


	// ========================================================================
	//! 自動スリープの有効／無効を切り替えます。
	// ------------------------------------------------------------------------
	//! true を指定するとスリープをシステムのデフォルトにし、falseにすると
	//! スリープしないようにします。
	// ------------------------------------------------------------------------
	void set_sleep_enable(bool _enable);


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

	mutable std::string				mCacheDir;
	mutable std::string				mDocumentsDir;
	mutable std::string				mFilesDir;

	friend	appcontroller;
};


inline application::application()
{
	AMTRS_DEBUG_LOG("[%p] amtrs application created.", this);
	sApplication	= this;
}



inline application::~application()
{
	AMTRS_DEBUG_LOG("[%p] amtrs application destroy.", this);
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
#endif

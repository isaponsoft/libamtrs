/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <clocale>
#include <amtrs/application/@>
#include <amtrs/chrono/@>
#include <amtrs/g3d/@>
#include <amtrs/node/scene_node_stack.hpp>
#include AMTRS_DRIVER_INCLUDE(api/opengl.hpp)
#include <gframe/@>
#include <gframe/application.hpp>

#include <app-config.hpp>		// Application configuration.

using namespace	amtrs;
using namespace	gframe;

std::initializer_list<void*> const& initmodules();

GFRAME_NAMESPACE_BEGIN
using namespace	amtrs;

static amtrs::filesystem::fileloader*	gFileLoader	= nullptr;


auto get_fileloader() -> amtrs::filesystem::fileloader*
{
	return	gFileLoader;
}



class	appmain
		: public application
{
public:
	using	super_type	= application;


	appmain()
	{
		initmodules();
	}

	virtual void on_created(int _argc, char* _args[]) override
	{
		AMTRS_DEBUG_LOG("===================================================================");
		AMTRS_DEBUG_LOG("DEBUG_LOG_LEVEL=%d", (int)DEBUG_LOG_LEVEL);

		std::setlocale(LC_ALL, "ja_JP.UTF-8");
		std::setlocale(LC_NUMERIC, "ja_JP.UTF-8");
		std::setlocale(LC_TIME, "ja_JP.UTF-8");

		mLoader				= filesystem::fileloader::get_instance();
		gFileLoader			= mLoader;

		// ファイルシステムに対してバインド。
		#if		(AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_ANDROID) || (AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_IOS)
		mLoader->push_back(filesystem::assetvfs::get_instance());
		#endif
		mLoader->push_back(filesystem::stdvfs::get_instance());



		// ウィンドウサイズを調整する。
		{
			auto	disp	= display::get_display(0);
			auto	ws		= disp->size();
			#if	defined(SCREEN_WIDTH) && defined(SCREEN_HEIGHT)
			if (disp->is_window_mode_enable())
			{
				// ウィンドウモードで動かすので画面を調整する
				auto	ds		= ws * 0.8f;
				float	scale	= 1;
				ws.width	= SCREEN_WIDTH;
				ws.height	= SCREEN_HEIGHT;


				// Width Par Height(アスペクト比)
				if (ws.width * scale > ds.width)
				{
					scale	= (float)ds.width / (float)ws.width;
				}
				if (ws.height * scale > ds.height)
				{
					scale	= (float)ds.height / (float)ws.height;
				}
				ws	= ws * scale;

				AMTRS_SYSTRACE_LOG("Window mode: max window sizw = %d x %d, screen size = %d x %d ", (int)ds.width, (int)ds.height, (int)ws.width, (int)ws.height);
			}
			#endif
			mWindow		= g3d::window::create(ws);
			mG3DDevice	= mWindow->get_g3d_device();
		}

		mWindow->onInputEvent	= [this](auto* _input) -> bool
		{
			auto	scene	= mMainScene.current();
			if (!scene)
			{
				return	true;
			}

			// input_event の座標系はウィンドウ座標になっているので、
			// シーンの座標系にコンバートする。
			auto	ss	= size2<float>(scene->size());
			auto	ws	= size2<float>(mWindow->size());
			_input->scale_point(vec2<float>(ss)/vec2<float>(ws));
			return	mMainScene.input(_input);
		};

		mWindow->set_event_callback([this](window::event_type _msg, const window::event_data* _data)
		{
			switch (_msg)
			{
				case window::event_type::created :
					AMTRS_SYSTRACE_LOG("Created");
					break;

				case window::event_type::activate_in :
					AMTRS_SYSTRACE_LOG("Active IN");
					break;

				case window::event_type::activate_out :
					AMTRS_SYSTRACE_LOG("Active OUT");
					break;

				case window::event_type::focus_gain :
					AMTRS_SYSTRACE_LOG("Focus Gain");
					break;

				case window::event_type::focus_lost :
					AMTRS_SYSTRACE_LOG("Focus Lost");
					break;

				case window::event_type::size_changed :
				{
				#if	0
					auto	e	= static_cast<window::size_event const*>(_data);
					AMTRS_DEBUG_LOG("Size change %dx%d", e->size.width, e->size.height);
				#endif
					set_scene_size(mMainScene.current());
					break;
				}

				case window::event_type::repaint :
				{
				//	AMTRS_DEBUG_LOG("repaint event.");
					node::rerender_request();
					break;
				}
			}
		});

		application::get_application()->add_event_listener(this, [](application::system_event _event)
		{
			if (_event == application::system_event::resume)
			{
				gframe_application_resume();
			}
			if (_event == application::system_event::pause)
			{
				gframe_application_pause();
			}
			if (_event == application::system_event::stop)
			{
				gframe_application_stop();
			}
		});

		mFpsController	= chrono::framerator(60);
		mFpsController.reset();


		gframe_application_created();


		auto	startup	= module<scene>::create({"main", "default::main"});
		if (!startup.empty())
		{
	//		gCurrentScene	= startup.get();
			mMainScene.push(startup.get());
		}
		else
		{
			AMTRS_WARN_LOG("main scene nothing.");
		}

		mRenderRequest.store(0);
	}


	template<class Node>
	void set_scene_size(Node* _node)
	{
		if (_node)
		{
			auto	size	= size3<float>{(float)mWindow->size().width, (float)mWindow->size().height, 0};
			auto	aspect	= (float)mWindow->size().height / (float)mWindow->size().width;

			#if		defined(SCREEN_WIDTH)
			size.width			= (float)SCREEN_WIDTH;
			size.height			= (float)SCREEN_WIDTH * aspect;
			#elif	defined(SCREEN_HEIGHT)
//			mScreenScale		= (float)SCREEN_HEIGHT / (float)mWindow->size().height;
			#endif

			_node->set_size(size);
		}
	}


	struct	rqueuek : ref_object, node::renderer_argument
	{
	};
	ref<rqueuek>	mRQueue;


	int				mSkipupdate	= 0;
	float			mDelta		= 0.0f;


	virtual void on_update() override
	{
		mFpsController.update();
		set_scene_size(mMainScene.current());
		mDelta	+= (float)mFpsController.delta().count() / 1000.0f;
		if (mSkipupdate > 0 && !node::is_rerender_requested())
		{
			--mSkipupdate;
			return;
		}

		mMainScene.update(mDelta);
		mDelta	= 0.0f;

		// レンダリング要請があるかつキューが空っぽの時は新しくキューを作る
		if (mRQueue.empty())
		{
			// レンダリングの必要がないなら更新処理も少し減らす（バッテリー対策）
			if (!node::is_rerender_requested())
			{
				mSkipupdate	= 2;
				return;
			}
			node::reset_rerender_request();

			ref<rqueuek>	queue	= new rqueuek();

			size2<float>	canvas	= (size2<float>)mMainScene.current()->size();
			queue->device		= mG3DDevice;
			queue->projection	= mat44<float>::ortho(
									{float((int)canvas.width/2), float((int)canvas.height/2)},		// 左上を原点にする
									{canvas.width, -canvas.height},									// 仮想スクリーンの大きさ
									0, 1.0f
								);
			mMainScene.render(*queue);
			mRQueue	= std::move(queue);

			++mCall;
		}

		mWindow->repaint([this]()
		{
			if (!mRQueue) { return; }
			ref<rqueuek>	queue(std::move(mRQueue));

			glViewport(0, 0, mG3DDevice->size().width, mG3DDevice->size().height);
			glClearColor(0,0,0,255);
			glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

			node::renderer_dispatcher{}.draw(*queue);
			mG3DDevice->present();
		});
	}

	int		mCall	= 0;
	std::atomic<int>	mPaint;
protected:
	ref<filesystem::fileloader>		mLoader;
	ref<g3d::window>				mWindow;
	ref<g3d::device>				mG3DDevice;
	std::atomic<int>				mRenderRequest;
	chrono::framerator				mFpsController;

	scene_node_stack<node>			mMainScene;
};



amtrs::ref<amtrs::application> gframe_application()
{
	return	new appmain();
}



GFRAME_NAMESPACE_END

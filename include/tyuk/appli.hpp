#ifndef	__toyoukebime__appli__hpp
#define	__toyoukebime__appli__hpp
#include <amtrs/amtrs.hpp>
#include <amtrs/application/@>
#include <amtrs/chrono/@>
#include <amtrs/filesystem/@>
#include <amtrs/g3d/@>
#include <amtrs/graphics/@>
#include <amtrs/node/@>
#include "tyuk-def.hpp"
#include "display_scale.hpp"
#include "texture_manager.hpp"
TYUK_NAMESPACE_BEGIN


template<class StartupSceneT>
class	appli
		: public application
{
	using	_base_type	= application;
public:
	using	startup_scene_type	= StartupSceneT;
	using	node_type			= typename startup_scene_type::node_type;
	using	window_type			= g3d::window;
	using	g3d_device_type		= g3d::device;

	virtual void on_stop() {}
	virtual void on_activate_in() {}
	virtual void on_activate_out() {}
	virtual void on_focus_gain() {}
	virtual void on_focus_lost() {}
	virtual void on_size_changed(size2<unsigned int> _size) {}

	virtual void on_clear_screen()
	{
		glClearColor(0,0,0,255);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	}

protected:
	// ========================================================================
	//! _screenSize
	//!		アプリケーションが想定する画面の大きさ。
	//!		(0, 0) を指定した場合はディスプレイのサイズと同じであることを
	//!		指定したと想定します。
	// ------------------------------------------------------------------------
	appli(size2<unsigned int> _screenSize)
		: mRequestScreenSize(_screenSize)
	{}

	virtual void on_created(int _argc, char* _args[]) override
	{
		_base_type::on_created(_argc, _args);
		mFileLoader	= filesystem::fileloader::get_instance();

		// ファイルシステムに対してバインド。
		#if		(AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_ANDROID) || (AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_IOS)
		mFileLoader->push_back(filesystem::assetvfs::get_instance());
		#endif
		mFileLoader->push_back(filesystem::stdvfs::get_instance());

		mTextureManager	= texture_manager::get_instance();
		mDisplayScale	= display_scale::get_instance();

		// ウィンドウサイズを調整する。
		auto	disp		= display::get_display(0);
		auto	winSize		= disp->size();
		AMTRS_DEBUG_LOG("display size = %dx%d", (int)winSize.width, (int)winSize.height);

		#if	AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_WIN32
		// Windowsで動かしている場合、画面を縮小するためにウィンドウの大きさを小さくする。
		winSize	= size2<int>(mRequestScreenSize);
		while (winSize.height > disp->size().height * 0.9f)
		{
			winSize *= 0.9f;
		}
		#else
		// Windows 以外は必ずディスプレイサイズを使用する
		#endif

		mWindow		= window_type::create(size2<unsigned int>(winSize));
		mG3DDevice	= mWindow->get_g3d_device();

		mWindow->onInputEvent	= [this](auto* _input) -> bool
		{
			return	on_input(_input);
		};

		mWindow->set_event_callback([this](window::event_type _msg, const window::event_data* _data)
		{
			switch (_msg)
			{
				case window::event_type::activate_in :
					AMTRS_DEBUG_LOG("Active IN");
					on_activate_in();
					break;

				case window::event_type::activate_out :
					AMTRS_DEBUG_LOG("Active OUT");
					on_activate_out();
					break;

				case window::event_type::focus_gain :
					AMTRS_DEBUG_LOG("Focus Gain");
					on_focus_gain();
					break;

				case window::event_type::focus_lost :
					AMTRS_DEBUG_LOG("Focus Lost");
					on_focus_lost();
					break;

				case window::event_type::size_changed :
				{
					auto*	sz		= static_cast<const window::size_event*>(_data);
					on_size_changed(sz->size);
					set_scene_size(mSceneStack.current());
					node_type::rerender_request();
					break;
				}

				case window::event_type::repaint :
				{
					on_renderer();
					break;
				}
			}
		});

		application::get_application()->add_event_listener(this, [this](application::system_event _event)
		{
			if (_event == application::system_event::stop)
			{
				this->on_stop();
			}
		});

		auto*	s	= new startup_scene_type();
		mSceneStack.push(s);
		mRenderRequest.store(0);
		set_scene_size(s);
		mFpsController.reset();
	}

	void set_scene_size(node_type* _node)
	{
		auto	size	= size3<float>{(float)mWindow->size().width, (float)mWindow->size().height, 0};
		auto	sps		= vec3<float>((float)mRequestScreenSize.width, (float)mRequestScreenSize.height, 0) / vec3<float>(size);
		auto	scale	= std::min(sps.x, sps.y);
		mDisplayScale->set_scale(scale);
		size	= size * scale;
		_node->set_size(size);
	}


	virtual void on_update() override
	{
		mFpsController.wait();
		mFpsController.update();
		node_type::reset_rerender_request();
		mSceneStack.update((float)mFpsController.delta());
		if (/*node_type::is_rerender_requested() && */mRenderRequest.load() == 0)
		{
			node_type::reset_rerender_request();
			mSceneStack.render(mG3DDevice);
			mRenderRequest.store(1);
			mWindow->repaint();
		}
	}

	virtual bool on_renderer()
	{
		if (mG3DDevice->is_context_enable() && (mRenderRequest.load() == 1))
		{
			glViewport(0, 0, mG3DDevice->size().width, mG3DDevice->size().height);
			on_clear_screen();
			mSceneStack.draw_exec(mG3DDevice);
			mG3DDevice->present();
			mRenderRequest.store(0);
			return	true;
		}
		return	false;
	}


	virtual bool on_input(input_event* _event)
	{
		if (!mSceneStack.empty())
		{
			// input_event の座標系はウィンドウ座標になっているので、
			// シーンの座標系にコンバートする。
			auto	ss	= size2<float>(mSceneStack.current()->size());
			auto	ws	= size2<float>(mWindow->size());
			_event->scale_point(vec2<float>(ss)/vec2<float>(ws));
		}
		return	mSceneStack.input(_event);
	}


private:
	std::atomic<int>				mRenderRequest;
	size2<unsigned int>				mRequestScreenSize;
	chrono::fps						mFpsController;
	scene_node_stack<node_type>		mSceneStack;
	ref<window_type>				mWindow;
	ref<g3d_device_type>			mG3DDevice;
	ref<texture_manager>			mTextureManager;
	ref<display_scale>				mDisplayScale;
	ref<filesystem::fileloader>		mFileLoader;
};


TYUK_NAMESPACE_END
#endif

/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__google__firebase_admob_banner__hpp
#define	__libamtrs__google__firebase_admob_banner__hpp
#include "../window/display.hpp"
#include "firebase_admob.hpp"
AMTRS_GOOGLE_FIREBASE_NAMESPACE_BEGIN


class	admob::banner
		: public ref_object
		, fb::admob::BannerView::Listener
{
public:
	enum class last_command
	{
		initialize,
		load_ad,
		show,
		hide,
	};

	enum class position_mode
	{
		top_center		= ::firebase::admob::BannerView::kPositionTop,
		bottom_center	= ::firebase::admob::BannerView::kPositionBottom,
		top_left		= ::firebase::admob::BannerView::kPositionTopLeft,
		top_right		= ::firebase::admob::BannerView::kPositionTopRight,
		bottom_left		= ::firebase::admob::BannerView::kPositionBottomLeft,
		bottom_right	= ::firebase::admob::BannerView::kPositionBottomRight,
	};

	//! test mode 用のバナー広告ID
	static constexpr const char*	ADMOB_TEST_UNIT_ID	= "ca-app-pub-3940256099942544/6300978111";

	virtual void OnPresentationStateChanged(fb::admob::BannerView* banner_view, fb::admob::BannerView::PresentationState state) override
	{
	}

	virtual void OnBoundingBoxChanged(fb::admob::BannerView* banner_view, fb::admob::BoundingBox box) override
	{
	}

	static ref<banner> create(admob* _admob, window* _banner_parent, const char* _unit_id = ADMOB_TEST_UNIT_ID)
	{
		ref<banner>	thiz	= new banner();

		fb::admob::AdSize banner_ad_size;
		banner_ad_size.ad_size_type	= fb::admob::kAdSizeStandard;
		banner_ad_size.width		= 320;
		banner_ad_size.height		= 50;
		
		thiz->mParent			= _admob;
		thiz->mView				= new fb::admob::BannerView();
		thiz->mView->Initialize(reinterpret_cast<fb::admob::AdParent>(_banner_parent->get_context()), _unit_id, banner_ad_size);
		thiz->mLastCommand		= last_command::initialize;
		thiz->mListenerRequest	= true;

		// 画面サイズからバナーの大きさを予測する
		thiz->mBoundBox	= {0, 0, (int)banner_ad_size.width, (int)banner_ad_size.height};

#if	AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_ANDROID
		{
			auto	disp		= display::get_display();
			thiz->mBoundBox.size() *= static_cast<android_display*>(disp.get())->density();
		}
#endif
		return	thiz;
	}

	//! バナーの位置とサイズを物理ピクセルで返します。
	rect<int> content_size_with_pixel() const noexcept { return mBoundBox; }


	void set_position_mode(position_mode _pos)
	{
		mSetPosition		= _pos;
		mPositionRequest	= true;
	}

	fb::Future<void> get_last_result()
	{
		if (mLastCommand == last_command::initialize)
		{
			return	mView->InitializeLastResult();
		}
		if (mLastCommand == last_command::load_ad)
		{
			return	mView->LoadAdLastResult();
		}
		if (mLastCommand == last_command::show)
		{
			auto	res	= mView->ShowLastResult();
			if (!mShow && res.status() == fb::kFutureStatusComplete)
			{
				mShow	= true;
			}
			return	res;
		}
		if (mLastCommand == last_command::hide)
		{
			return	mView->ShowLastResult();
		}
		return	{};
	}

	void load_ad(adrequest* _request = nullptr, bool _show = true)
	{
		if (_request)
		{
			mRequest		= _request;
		}
		else
		{
			mRequest		= new adrequest();
		}
		mLoadRequest	= true;
		mShowRequest	= _show;
	}

	void update()
	{
		// コマンドはひとつづつ遅延実行されるので、最後に発行したコマンドまで待つ。
		if (get_last_result().status() != fb::kFutureStatusComplete)
		{
			return;
		}

		// 広告サイズの監視
		auto	bbox	= rect<int>(mView->bounding_box());
		if (!mInitializedBbox || mBoundBox != bbox)
		{
			mInitializedBbox	= true;
			//mBoundBox			= bbox;
			//AMTRS_DEBUG_LOG("Admob banner bounding box = (%d,%d) - (%d,%d)", bbox.x, bbox.y, bbox.width, bbox.height);
		}

		if (mInitializedBbox && mLoadRequest)
		{
			mLoadRequest	= false;
			mLastCommand	= last_command::load_ad;
			mView->LoadAd(*mRequest);
			return;
		}


		if (mListenerRequest)
		{
			mListenerRequest	= false;
			mView->SetListener(this);
		}

		if (mPositionRequest)
		{
			mPositionRequest	= false;
			mView->MoveTo((::firebase::admob::BannerView::Position)mSetPosition);
			return;
		}

		if (mShowRequest)
		{
			mShowRequest	= false;
			mLastCommand	= last_command::show;
			mView->Show();
			return;
		}
	}


	fb::admob::BannerView* get_context() const noexcept { return mView; }

protected:
	last_command				mLastCommand;
	ref<admob>					mParent;
	fb::admob::BannerView*		mView;
	ref<adrequest>				mRequest;
	bool						mInitializedBbox		= false;
	rect<int>					mBoundBox			= {0, 0, 0, 0};
	bool						mListenerRequest	= false;
	bool						mLoadRequest		= false;
	bool						mShowRequest		= false;
	bool						mShow				= false;

	bool						mPositionRequest	= false;
	position_mode				mSetPosition		= position_mode::top_center;
};


AMTRS_GOOGLE_FIREBASE_NAMESPACE_END
#endif

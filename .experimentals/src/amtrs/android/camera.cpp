/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <thread>
#include <amtrs/camera.hpp>
#include <amtrs/.driver/android-api-camera.hpp>
#include <amtrs/.driver/android-api-imagerender.hpp>
#include <amtrs/.driver/android-api-permissions.hpp>

// _funcで指定した関数を呼び出して OK 以外ならログを出力して return false する。
#define	__acam(...)	if (camera_status_t r = (__VA_ARGS__); r != ACAMERA_OK) { AMTRS_WARN_LOG("Error(%d): " #__VA_ARGS__, (int)r); return false; }
#define	__aimg(...)	if (media_status_t  r = (__VA_ARGS__); r != AMEDIA_OK ) { AMTRS_WARN_LOG("Error(%d): " #__VA_ARGS__, (int)r); return false; }

static ACameraManager*	gCameraManager	= nullptr;

AMTRS_NAMESPACE_BEGIN

class	camera_task
		: public ref_object
{
public:
	using	session_type	= os::android::camera::capture_session<camera_task>;
	using	render_type		= os::android::imagerender::imagerender;
	enum	status_types
	{
		_ready,
		_capturing,
		_stopping,
		_stopped,
	};


	~camera_task()
	{
		if (mDevice)
		{
			ACameraDevice_close(mDevice);
			mDevice	= nullptr;
		}
	}


	bool start(char const* _id, camera::format _format, std::function<void(camera::planedata const*, size_t)> _capture)
	{
		mCaptureListener	= std::move(_capture);

		// デバイスの初期化
		ACameraDevice_StateCallbacks	callbacks;
		callbacks.context			= this;
		callbacks.onDisconnected	= [](void* context, ACameraDevice* device)
		{
		};
		callbacks.onError			= [](void* context, ACameraDevice* device, int error)
		{
		};
		__acam(ACameraManager_openCamera(gCameraManager, _id, &callbacks, &mDevice))


		AIMAGE_FORMATS	fmt	= _format.type  == camera::format_types::yuv_420_888 ? AIMAGE_FORMAT_YUV_420_888
							: _format.type  == camera::format_types::jpeg        ? AIMAGE_FORMAT_JPEG
							: _format.type  != camera::format_types::bitmap      ? AIMAGE_FORMAT_PRIVATE
							: _format.color == color_format::rgba8  		     ? AIMAGE_FORMAT_RGBA_8888
							: _format.color == color_format::rgbx8  		     ? AIMAGE_FORMAT_RGBX_8888
							: _format.color == color_format::rgb8       		 ? AIMAGE_FORMAT_RGB_888
							:											   		   AIMAGE_FORMAT_PRIVATE;
		__aimg(mRender	.create(_format.size, fmt, 4));
		__acam(mOutput	.create(mRender.window()));
		__acam(mSession	.create(this, mDevice, mOutput));
		__acam(mTarget	.create(mRender.window()));
		__acam(mRequest	.create(mDevice, TEMPLATE_PREVIEW, mTarget));
		return	restart();
	}


	bool restart()
	{
		__aimg(mRender.set_listener(this));
		__acam(mSession.repeating(mRequest));
		mStatus	= _capturing;
		return	true;
	}

	bool pause()
	{
		return	stop();
	}

	bool stop()
	{
		if (mStatus != _capturing)
		{
			return	true;
		}

		mStatus	= _stopping;
		std::thread([thiz = ref<camera_task>(this)]()
		{
			thiz->mSession.stop_repeating();
			thiz->mStatus	= _stopped;
		}).detach();
		return	true;
	}



	// ==============================================================
	// imagerender callbacks
	void onImageAvailable(render_type const& _imagerender)
	{
		AImage*		image	= _imagerender.acquire_next_image();
		if (mStatus == _capturing)
		{
			int			width		= 0;
			int			height		= 0;
			AImage_getWidth(image, &width);
			AImage_getHeight(image, &height);

			camera::planedata	plans[10];

			int32_t		planCount;
			AImage_getNumberOfPlanes(image, &planCount);
			for (int i = 0; i < planCount; ++i)
			{
				uint8_t*	data	= nullptr;
				int			length	= 0;
				AImage_getPlaneData(image, i, &data, &length);
				plans[i].data(data);
				plans[i].length	= length;

				// 画像サイズ
				plans[i].size({width, height});
				if (i != 0)
				{
					plans[i].size(plans[i].size() / 2);
				}

				// stride
				int			stride_x	= 0;
				int			stride_y	= 0;
				AImage_getPlanePixelStride	(image, i, &stride_x);
				AImage_getPlaneRowStride	(image, i, &stride_y);
				plans[i].stride({stride_x, stride_y});
			}
			mCaptureListener(plans, planCount);
		}
		AImage_delete(image);
		mCaptureProcess	= false;
	}


	// ==============================================================
	// capture_session callbacks
	void onReady(session_type* _session)
	{
		AMTRS_SYSTRACE_LOG("onReady");
	}

	void onActive(session_type* _session)
	{
		AMTRS_SYSTRACE_LOG("onActive");
	}

	void onClosed(session_type* _session)
	{
		AMTRS_SYSTRACE_LOG("onClosed");
		mStatus	= _ready;
	}

	void onCaptureBufferLost(session_type* _session, ACaptureRequest* _request, ACameraWindowType* _window, int64_t _frameNumber)
	{
		AMTRS_SYSTRACE_LOG("onCaptureBufferLost");
	}

	// キャプチャ開始（1フレーム）
	void onCaptureStarted(session_type* _session, const ACaptureRequest* request, int64_t timestamp)
	{
		//AMTRS_SYSTRACE_LOG("onCaptureStarted");
		mCaptureProcess	= true;
	}

	// キャプチャ完了（1フレーム）
	void onCaptureCompleted(session_type* _session, ACaptureRequest* request, const ACameraMetadata* result)
	{
		//AMTRS_SYSTRACE_LOG("onCaptureCompleted");
	}

	void onCaptureFailed(session_type* _session, ACaptureRequest* request, ACameraCaptureFailure* failure)
	{
		AMTRS_SYSTRACE_LOG("onCaptureFailed");
	}

	void onCaptureProgressed(session_type* _session, ACaptureRequest* request, const ACameraMetadata* result)
	{
		AMTRS_SYSTRACE_LOG("onCaptureProgressed");
	}

	void onCaptureSequenceCompleted(session_type* _session, int sequenceId, int64_t frameNumber)
	{
		AMTRS_SYSTRACE_LOG("onCaptureSequenceCompleted");
		mStatus		= _stopped;
	}

	void onCaptureSequenceAborted(session_type* _session, int sequenceId)
	{
		AMTRS_SYSTRACE_LOG("onCaptureSequenceAborted");
	}

	status_types											mStatus			= _ready;
	bool													mCaptureProcess	= false;

	std::function<void(camera::planedata const*, size_t)>	mCaptureListener;

	ACameraDevice*											mDevice			= nullptr;
	render_type												mRender;
	os::android::camera::window_output_container			mOutput;
	session_type											mSession;

	os::android::camera::window_target						mTarget;
	os::android::camera::capture_request					mRequest;
};


class	camera_impl
		: public camera
{
public:
	camera_impl(std::string _id)
		: mId(std::move(_id))
	{
		ACameraMetadata*			meta;
		if (auto r = ACameraManager_getCameraCharacteristics(gCameraManager, mId.c_str(), &meta); r != ACAMERA_OK)
		{
			return;
		}
		// 向きを保存しておく
		auto	facing	= os::android::camera::facing(meta);
		mFacing	= facing == ACAMERA_LENS_FACING_FRONT ? camera::facing_types::front
				: facing == ACAMERA_LENS_FACING_BACK  ? camera::facing_types::back
				:									    camera::facing_types::external;
		ACameraMetadata_free(meta);

		mTask	= new camera_task();
	}

	~camera_impl()
	{
		stop();
	}


	virtual bool formats(std::function<void(format)> _callback) override
	{
		ACameraMetadata*				meta;
		__acam(ACameraManager_getCameraCharacteristics(gCameraManager, mId.c_str(), &meta))

		ACameraMetadata_const_entry		entry;
		__acam(ACameraMetadata_getConstEntry(meta, ACAMERA_SCALER_AVAILABLE_STREAM_CONFIGURATIONS, &entry))
		for (int i = 0; i < entry.count; i += 4)
		{
			bool	isOutput	= entry.data.i32[i + 3]
								? true			// output device
								: false;		// input device
			if (isOutput) { continue; }
			format	fmt;

			fmt.size.width	= entry.data.i32[i + 1];
			fmt.size.height	= entry.data.i32[i + 2];

			auto	imgf	= (AIMAGE_FORMATS)entry.data.i32[i + 0];;
			fmt.type	= imgf == AIMAGE_FORMAT_RGBA_8888	? format_types::bitmap
						: imgf == AIMAGE_FORMAT_RGBX_8888	? format_types::bitmap
						: imgf == AIMAGE_FORMAT_RGB_888		? format_types::bitmap
						: imgf == AIMAGE_FORMAT_YUV_420_888	? format_types::yuv_420_888
						: imgf == AIMAGE_FORMAT_JPEG		? format_types::jpeg
						:									  format_types::unkown;
			fmt.color	= imgf == AIMAGE_FORMAT_RGBA_8888	? color_format::rgba8
						: imgf == AIMAGE_FORMAT_RGBX_8888	? color_format::rgbx8
						: imgf == AIMAGE_FORMAT_RGB_888		? color_format::rgb8
						: imgf == AIMAGE_FORMAT_YUV_420_888	? color_format::gray8
						: imgf == AIMAGE_FORMAT_JPEG		? color_format::unkown
						:									  color_format::unkown;
			_callback(fmt);
		}
		ACameraMetadata_free(meta);
		return	true;
	}


	virtual bool start(format _format, std::function<void(image_type const*, size_t)> _capture) override
	{
		return	mTask->start(mId.c_str(), _format, std::move(_capture));
	}


	virtual bool restart() override
	{
		return	mTask->restart();
	}

	virtual bool pause() override
	{
		return	mTask->pause();
	}

	virtual bool stop() override
	{
		return	mTask->stop();
	}

	std::string			mId;
	facing_types		mFacing;
	ref<camera_task>	mTask;
};



void camera::permission(std::function<void(bool)> _callback)
{
	if (!gCameraManager)
	{
		gCameraManager	= ACameraManager_create();
	}

	auto	cameraPerm	= java::classes::android::Manifest::permission::find().get_CAMERA();
	os::android::permission({cameraPerm}, [=](bool _is_ok)
	{
		_callback(_is_ok);
	});
}


bool camera::enumlate(std::function<bool(ref<camera>)> _callback)
{
	return	ACAMERA_OK == os::android::camera::enumrate_ids(gCameraManager, [=](ACameraManager* m, int index, char const* id)
	{
		return	_callback(new camera_impl(std::string(id)));
	});
}

AMTRS_NAMESPACE_END

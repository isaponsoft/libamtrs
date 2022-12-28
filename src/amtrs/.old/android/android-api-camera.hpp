/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__driver__android__api__camera__hpp
#define	__libamtrs__driver__android__api__camera__hpp
#include <camera/NdkCameraCaptureSession.h>
#include <camera/NdkCameraDevice.h>
#include <camera/NdkCameraError.h>
#include <camera/NdkCameraManager.h>
#include <camera/NdkCameraMetadata.h>
#include <camera/NdkCameraMetadataTags.h>
#include <camera/NdkCameraWindowType.h>
#include <camera/NdkCaptureRequest.h>
#include <media/NdkImage.h>
#include "android-api-def.hpp"
#include "../media/types.hpp"
#include "../geometry.hpp"
AMTRS_OS_ANDROID_NAMESPACE_BEGIN namespace camera {


//! カメラIDをリストアップしてコールバックを呼び出します。
template<class Callback>
auto enumrate_ids(ACameraManager* _manager, Callback&& _callback) -> camera_status_t
{
	if (!_manager)
	{
		return	ACAMERA_ERROR_UNKNOWN;
	}
	ACameraIdList*	list;
	camera_status_t	r	= ACameraManager_getCameraIdList(_manager, &list);
	if (r != ACAMERA_OK)
	{
		return	r;
	}
	for (int i = 0; i < list->numCameras; ++i)
	{
		if (!_callback(_manager, i, list->cameraIds[i]))
		{
			break;
		}
	}
	ACameraManager_deleteCameraIdList(list);
	return	ACAMERA_OK;
}


//! レンズの向きを取得します。
inline auto facing(ACameraMetadata* _meta) -> acamera_metadata_enum_acamera_lens_facing
{
	ACameraMetadata_const_entry		entry;
	ACameraMetadata_getConstEntry(_meta, ACAMERA_LENS_FACING, &entry);
	return	(acamera_metadata_enum_acamera_lens_facing)entry.data.u8[0];
}

//! カメラの画質（ピクセル
inline auto pixel_size(ACameraMetadata* _meta) -> size2<int> 
{
	ACameraMetadata_const_entry		entry;
	ACameraMetadata_getConstEntry(_meta, ACAMERA_SENSOR_INFO_PIXEL_ARRAY_SIZE, &entry);
	return	size2<int>(entry.data.i32[0], entry.data.i32[1]);
}


//! サポートするストリームの情報を返します。
struct	stream_configration
{
	enum	stream_types
	{
		output_stream	= 0,
		input_stream	= 1,
	};

	stream_types	type;
	AIMAGE_FORMATS	format;
	int				width;
	int				height;

	bool is_output() const noexcept { return type == output_stream; }
	bool is_input() const noexcept { return type == input_stream; }

	template<class Callback>
	static auto enumrate_availavles(ACameraMetadata* _meta, Callback&& _callback) -> camera_status_t
	{
		ACameraMetadata_const_entry		entry;
		camera_status_t					r	= ACameraMetadata_getConstEntry(_meta, ACAMERA_SCALER_AVAILABLE_STREAM_CONFIGURATIONS, &entry);
		if (r != ACAMERA_OK)
		{
			return	r;
		}
		for (int i = 0; i < entry.count; i += 4)
		{
			stream_configration	data;
			data.format	= (AIMAGE_FORMATS)entry.data.i32[i + 0];
			data.width	= entry.data.i32[i + 1];
			data.height	= entry.data.i32[i + 2];
			data.type	= entry.data.i32[i + 3]
						? input_stream
						: output_stream;
			_callback(std::move(data));
		}
		return	ACAMERA_OK;
	}
};

/*
//! デバイスの情報を取得します。
inline auto get_info_data(amtrs::camera::info_data& _buff, ACameraManager* _manager, char const* _id) -> camera_status_t
{
	using namespace ::amtrs::camera;

	amtrs::camera::info_data	info;
	ACameraMetadata*			meta;
	if (auto r = ACameraManager_getCameraCharacteristics(_manager, _id, &meta); r != ACAMERA_OK)
	{
		return	r;
	}

	info.name	= std::string(_id);

	// レンズの方向
	auto	facing	= os::android::camera::facing(meta);
	info.facing	= facing == ACAMERA_LENS_FACING_FRONT ? facing_types::front
				: facing == ACAMERA_LENS_FACING_BACK  ? facing_types::back
				:									    facing_types::external;

	// カメラの画像サイズ
	info.size	= os::android::camera::pixel_size(meta);

	// 出力フォーマットを列挙する
	std::vector<image_info>	infos;
	os::android::camera::stream_configration::enumrate_availavles(meta, [&](auto conf)
	{
		if (!conf.is_output()) return;
		format_types	t	= conf.format == AIMAGE_FORMAT_RGBA_8888	? format_types::bitmap
							: conf.format == AIMAGE_FORMAT_RGBX_8888	? format_types::bitmap
							: conf.format == AIMAGE_FORMAT_RGB_888		? format_types::bitmap
							: conf.format == AIMAGE_FORMAT_YUV_420_888	? format_types::yuv_420_888
							: conf.format == AIMAGE_FORMAT_JPEG			? format_types::jpeg
							:											  format_types::unkown;
		color_format	c	= conf.format == AIMAGE_FORMAT_RGBA_8888	? color_format::rgba8
							: conf.format == AIMAGE_FORMAT_RGBX_8888	? color_format::rgbx8
							: conf.format == AIMAGE_FORMAT_RGB_888		? color_format::rgb8
							: conf.format == AIMAGE_FORMAT_YUV_420_888	? color_format::gray8
							: conf.format == AIMAGE_FORMAT_JPEG			? color_format::unkown
							:											  color_format::unkown;
		infos.push_back({t, c, size2<int>(conf.width, conf.height)});
	});
	info.streams	= shared_buffer<image_info>(infos.size());
	std::copy(infos.begin(), infos.end(), info.streams.begin());

	ACameraMetadata_free(meta);

	_buff	= std::move(info);
	return	ACAMERA_OK;
}
*/

class	window_output_container
{
public:
	auto create(ANativeWindow* _window) -> camera_status_t
	{
		if (camera_status_t r = ACaptureSessionOutputContainer_create(&mOoutputContainer); r != ACAMERA_OK)
		{
			return	r;
		}
		if (camera_status_t	r = ACaptureSessionOutput_create(_window, &mOutput); r != ACAMERA_OK)
		{
			return	r;
		}
		return	ACaptureSessionOutputContainer_add(mOoutputContainer, mOutput);
	}

	~window_output_container()
	{
		clear();
	}

	void clear()
	{
		if (mOoutputContainer)
		{
			ACaptureSessionOutputContainer_free(mOoutputContainer);
			mOoutputContainer	= nullptr;
		}
		if (mOutput)
		{
			ACaptureSessionOutput_free(mOutput);
			mOutput				 = nullptr;
		}
	}

	operator ACaptureSessionOutputContainer* () const noexcept { return mOoutputContainer; }

private:
	ACaptureSessionOutput*				mOutput				= nullptr;
	ACaptureSessionOutputContainer*		mOoutputContainer	= nullptr;
};

template<class Listener>
class	capture_session
{
public:
	using	listener_type	= Listener;

	auto create(listener_type* _listener, ACameraDevice* _device, ACaptureSessionOutputContainer * _output) -> camera_status_t
	{
		mListener	= _listener;

		ACameraCaptureSession_stateCallbacks	callbacks;
		callbacks.context	= this;
		callbacks.onReady	= [](void* context, ACameraCaptureSession* session) { auto thiz = reinterpret_cast<capture_session*>(context); thiz->mListener->onReady (thiz); };
		callbacks.onActive	= [](void* context, ACameraCaptureSession* session) { auto thiz = reinterpret_cast<capture_session*>(context); thiz->mListener->onActive(thiz); };
		callbacks.onClosed	= [](void* context, ACameraCaptureSession* session) { auto thiz = reinterpret_cast<capture_session*>(context); thiz->mListener->onClosed(thiz); };
		return	ACameraDevice_createCaptureSession(_device, _output, &callbacks, &mSession);
	}

	auto repeating(ACaptureRequest* _request) -> camera_status_t
	{
		ACameraCaptureSession_captureCallbacks	callbacks;
		callbacks.context						= this;

		callbacks.onCaptureBufferLost			= [](void *context, ACameraCaptureSession* session, ACaptureRequest* request, ACameraWindowType* window, int64_t frameNumber)
		{
			auto	thiz = reinterpret_cast<capture_session*>(context);
			thiz->mListener->onCaptureBufferLost(thiz, request, window, frameNumber);
		};

		callbacks.onCaptureCompleted			= [](void* context, ACameraCaptureSession* session, ACaptureRequest* request, const ACameraMetadata* result)
		{
			auto	thiz = reinterpret_cast<capture_session*>(context);
			thiz->mListener->onCaptureCompleted(thiz, request, result);
		};

		callbacks.onCaptureFailed				= [](void* context, ACameraCaptureSession* session, ACaptureRequest* request, ACameraCaptureFailure* failure)
		{
			auto	thiz = reinterpret_cast<capture_session*>(context);
			thiz->mListener->onCaptureFailed(thiz, request, failure);
		};

		callbacks.onCaptureProgressed			= [](void* context, ACameraCaptureSession* session, ACaptureRequest* request, const ACameraMetadata* result)
		{
			auto	thiz = reinterpret_cast<capture_session*>(context);
			thiz->mListener->onCaptureProgressed(thiz, request, result);
		};

		callbacks.onCaptureSequenceCompleted	= [](void* context, ACameraCaptureSession* session, int sequenceId, int64_t frameNumber)
		{
			auto	thiz = reinterpret_cast<capture_session*>(context);
			thiz->mListener->onCaptureSequenceCompleted(thiz, sequenceId, frameNumber);
		};

		callbacks.onCaptureSequenceAborted		= [](void* context, ACameraCaptureSession* session, int sequenceId)
		{
			auto	thiz = reinterpret_cast<capture_session*>(context);
			thiz->mListener->onCaptureSequenceAborted(thiz, sequenceId);
		};

		callbacks.onCaptureStarted				= [](void* context, ACameraCaptureSession* session, const ACaptureRequest* request, int64_t timestamp)
		{
			auto	thiz = reinterpret_cast<capture_session*>(context);
			thiz->mListener->onCaptureStarted(thiz, request, timestamp);
		};

		ACaptureRequest*	reqs[1]	= { _request };
		return	ACameraCaptureSession_setRepeatingRequest(mSession, &callbacks, 1, reqs, nullptr);
	}

	auto stop_repeating() -> camera_status_t
	{
		return	ACameraCaptureSession_stopRepeating(mSession);
	}

	~capture_session()
	{
		clear();
	}

	void clear()
	{
		if (mSession)
		{
			ACameraCaptureSession_close(mSession);
			mSession	= nullptr;
		}
	}

	operator ACameraCaptureSession* () const noexcept { return mSession; }


private:
	ACameraCaptureSession*		mSession	= nullptr;
	listener_type*				mListener	= nullptr;
};

class	window_target
{
public:
	auto create(ACameraWindowType* _window) -> camera_status_t 
	{
		return	ACameraOutputTarget_create(_window, &mTarget);
	}

	~window_target()
	{
		clear();
	}

	void clear()
	{
		if (mTarget)
		{
			ACameraOutputTarget_free(mTarget);
			mTarget	= nullptr;
		}
	}

	operator ACameraOutputTarget* () const noexcept { return mTarget; }
private:
	ACameraOutputTarget*		mTarget		= nullptr;
};


class	capture_request
{
public:
	auto create(ACameraDevice* _device, ACameraDevice_request_template _templateId, ACameraOutputTarget* _target) -> camera_status_t 
	{
		if (auto r = ACameraDevice_createCaptureRequest(_device, _templateId, &mRequest); r != ACAMERA_OK)
		{
			return	r;
		}
		return	ACaptureRequest_addTarget(mRequest, _target);
	}

	~capture_request()
	{
		clear();
	}

	void clear()
	{
		if (mRequest)
		{
			ACaptureRequest_free(mRequest);
			mRequest	= nullptr;
		}
	}

	operator ACaptureRequest* () const noexcept { return mRequest; }

private:
	ACaptureRequest*			mRequest	= nullptr;
};

} AMTRS_OS_ANDROID_NAMESPACE_END
#endif

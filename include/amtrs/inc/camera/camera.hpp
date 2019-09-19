/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__camera__camera__hpp
#define	__libamtrs__camera__camera__hpp
#include "../../java/jp/libamtrs/camera/Camera.hpp"
#include "def.hpp"
AMTRS_NAMESPACE_BEGIN
class	camera : public ref_object
{
public:
	enum class	camera_type
	{
		back_camera,			//!< 外側のカメラ
		inner_camera,			//!< 内向きのカメラ
	};


	static ref<camera> create(camera_type _type);

	void set_preview_callback(std::function<void(bitmap<rgba<uint8_t>>&&)>&& _callback)
	{
		mPreviewCallback	= std::move(_callback);
	}

	virtual size2<int> preview_size() const noexcept = 0;

//protected:
	std::function<void(bitmap<rgba<uint8_t>>&&)>		mPreviewCallback;
};



class	camera_android
		: public camera
{
public:
	using		JavaCamera		= java::classes::android::hardware::Camera;
	using		AmtrsCamera		= java::classes::jp::libamtrs::camera::Camera;
	

	virtual size2<int> preview_size() const noexcept
	{
		return	mPreviewSize;
	}


	template<class CO>
	void initialize(CO& _camera)
	{
		// NativeMethodを登録
		AMTRS_JAVA_JNIMETHOD_REGISTER(AmtrsCamera, camera_android, 
			AMTRS_JAVA_MAKE_JNIMETHOD(onPreviewFrame)
		);


		auto		clsCamera		= AmtrsCamera::find();
		auto		objAct			= os::android::get_activity();
		auto		objCamera		= clsCamera.init(reinterpret_cast<jlong>(this), objAct, _camera);
		auto		objPrevSize		= _camera.getParameters().getPreviewSize();
		auto		objPictSize		= _camera.getParameters().getPictureSize();
		mPreviewSize	= size2<int>(objPrevSize.get_width(), objPrevSize.get_height());
		mPictureSize	= size2<int>(objPictSize.get_width(), objPictSize.get_height());
		mCamera			= objCamera;

		AMTRS_TRACE_LOG("Camera preview size = %dx%d, picture size = %dx%d", mPreviewSize.width, mPreviewSize.height, mPictureSize.width, mPictureSize.height);
	}


	struct	CrCb
	{
		uint8_t	Cr;
		uint8_t	Cb;
	};

	void onPreviewFrame(jbyteArray _data, java::jobj<JavaCamera> _camera)
	{
		if (mPreviewCallback)
		{
			bitmap<rgba<uint8_t>>	bmp(size2<int>(mPreviewSize.width, mPreviewSize.height));

			jbyte*	elems	= java::get_jnienv()->GetByteArrayElements(_data, nullptr);
			auto	dest	= bmp.pixels().data();
#if	1
			int8_t*		y_src	= elems;
			CrCb*		crcb	= reinterpret_cast<CrCb*>(elems + mPreviewSize.width * mPreviewSize.height);
			for (int y = 0; y < mPreviewSize.height; ++y)
			{
				int		Cr			= 0;
				int		Cb			= 0;
				auto*	crcbline	= crcb;
				for (int x = 0; x < mPreviewSize.width; ++x)
				{

					#if	1
					int		Y	= *y_src++;
					if (!(x & 0x1))
					{
						Cb		= crcbline->Cr;
						Cr		= crcbline->Cb;
						++crcbline;
					}
					auto&	pixel	= *dest++;
					pixel.r = Y + 1.40200f * Cr;
					pixel.g = Y - 0.34414f * Cb - 0.71414f * Cr; 
					pixel.b = Y + 1.77200f * Cb;
					#else
					int		Y	= *y_src++;
					if (Y < 0) { Y += 255; }
					if (!(x & 0x1))
					{
						Cb		= crcbline->Cr;
						Cr		= crcbline->Cb;
						if (Cb < 0) { Cb += 127; } else { Cb -= 128; }
						if (Cr < 0) { Cr += 127; } else { Cr -= 128; }
						++crcbline;
					}
					auto&	pixel	= *dest++;
					pixel.r	= color_level_inbound(Y + Cr + (Cr >> 2) + (Cr >> 3) + (Cr >> 5));
					pixel.g	= color_level_inbound(Y - (Cb >> 2) + (Cb >> 4) + (Cb >> 5) - (Cr >> 1) + (Cr >> 3) + (Cr >> 4) + (Cr >> 5));
					pixel.b	= color_level_inbound(Y + Cb + (Cb >> 1) + (Cb >> 2) + (Cb >> 6));
					#endif
					pixel.a	= 255;
				}
				if (y&1)
				{
					crcb += mPreviewSize.width/2;
				}
			}
#endif
			java::get_jnienv()->ReleaseByteArrayElements(_data, elems, 0);

			mPreviewCallback(std::move(bmp));
		}
	}

	template<class T>
	T color_level_inbound(T _value)
	{
		return	  _value <   0 ?   0
				: _value > 255 ? 255
				: _value;
	}

	java::globalref<jobject>						mCamera;
	size2<int>										mPreviewSize;
	size2<int>										mPictureSize;
};

inline ref<camera> camera::create(camera_type _type)
{
	using		Camera		= java::classes::android::hardware::Camera;
	auto		clsCamera	= Camera::find();
	auto		clsInfo		= Camera::CameraInfo::find();

	// カメラのIDを取得する
	auto		numOfCamera	= clsCamera.getNumberOfCameras();
	auto		objInfo		= clsInfo.init();
	auto		requestType	= _type == camera_type::back_camera
							? clsInfo.get_CAMERA_FACING_BACK()
							: clsInfo.get_CAMERA_FACING_FRONT();
	int			cameraId	= -1;
	for (int i = 0; i < numOfCamera; ++i)
	{
		clsCamera.getCameraInfo(i, objInfo);
		if (objInfo.get_facing() == requestType)
		{
			cameraId	= i;
			break;
		}
	}

	ref<camera_android>	retval	= new camera_android();
	auto				objCam	= clsCamera.open(cameraId);
	retval->initialize(objCam);
	return	retval;
}


AMTRS_NAMESPACE_END
#endif

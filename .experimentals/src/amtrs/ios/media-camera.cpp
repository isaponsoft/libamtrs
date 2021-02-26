/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#import <UIKit/UIKit.h>
#import <AVFoundation/AVFoundation.h>
#include <thread>
#include <amtrs/media/camera.hpp>
#include <amtrs/.driver/ios/applekit/string.hpp>
AMTRS_MEDIA_NAMESPACE_BEGIN

class	camera_impl
		: public camera
{
public:
	camera_impl(AVCaptureDevice* _device)
		: mDevice(_device)
	{
		mName	= std::to_string([mDevice localizedName]);
		//mFace	= [mDevice position] == AVCaptureDevicePositionBack ? facing_types::back : facing_types::front;
	}

	virtual bool formats(std::function<void(format)> _callback) override
	{
		for (AVCaptureDeviceFormat* f in mDevice.formats)
		{
			if (CMFormatDescriptionGetMediaType(f.formatDescription) != kCMMediaType_Video)
			{
				continue;
			}

			CMVideoDimensions	dim		= CMVideoFormatDescriptionGetDimensions(f.formatDescription);

			format		fmt;

			fmt			= {};
			fmt.size	= size2<int>(dim.width, dim.height);
			fmt.type	= format_types::yuv_420_888;
			fmt.color	= color_format::gray8;
			_callback(fmt);

			fmt			= {};
			fmt.size	= size2<int>(dim.width, dim.height);
			fmt.type	= format_types::bitmap;
			fmt.color	= color_format::rgba8;
			_callback(fmt);
		}
		return	true;
	}


	virtual bool start(format _format, std::function<void(planedata const*, size_t)> _capture) override
	{
		mCaptureCallback	= std::move(_capture);
	
		// セッション作成
		mSession	= [[AVCaptureSession alloc] init];
		mInput		= [AVCaptureDeviceInput deviceInputWithDevice:mDevice error:nullptr];
		
		AVCaptureSessionPreset	preset	= AVCaptureSessionPresetHigh;
		if (_format.size.width <= 352)
		{
			preset	= AVCaptureSessionPreset352x288;
		}
		else if (_format.size.width <= 640)
		{
			preset	= AVCaptureSessionPreset640x480;
		}
		else if (_format.size.width <= 1280)
		{
			preset	= AVCaptureSessionPreset1280x720;
		}
		else if (_format.size.width <= 1920)
		{
			preset	= AVCaptureSessionPreset1920x1080;
		}
		else if (_format.size.width <= 3840)
		{
			preset	= AVCaptureSessionPreset3840x2160;
		}
		else if (_format.size.width <= 3840)
		{
			preset	= AVCaptureSessionPreset3840x2160;
		}
		[mSession setSessionPreset:AVCaptureSessionPreset640x480];
		[mSession addInput:mInput];

		// 出力パラメータ生成
		mOutput		= [[AVCaptureVideoDataOutput alloc] init];
		[mOutput setSampleBufferDelegate:create_delegate(this) queue:dispatch_queue_create("VideoQueue", DISPATCH_QUEUE_SERIAL)];
		mOutput.alwaysDiscardsLateVideoFrames = true;
		//mOutput.videoSettings	= @{(id)kCVPixelBufferPixelFormatTypeKey:[NSNumber numberWithInt:kCVPixelFormatType_420YpCbCr8BiPlanarFullRange]};
		mOutput.videoSettings	= @{(id)kCVPixelBufferPixelFormatTypeKey:[NSNumber numberWithInt:kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange]};
		//mOutput.videoSettings	= @{(id)kCVPixelBufferPixelFormatTypeKey:[NSNumber numberWithInt:kCVPixelFormatType_32BGRA]};
		[mSession addOutput:mOutput];

		[mSession commitConfiguration];

		mActive	= true;
		[mSession startRunning];
		return	true;
	}


	bool restart() override
	{
		mActive	= true;
		return	true;
	}

	bool pause() override
	{
		mActive	= false;
		return	true;
	}


	// ========================================================================
	//! キャプチャを停止します。
	// ------------------------------------------------------------------------
	bool stop() override
	{
		pause();
		if (mSession)
		{
			[mSession stopRunning];

			// キャプチャー処理中ならとりあえず待つ
			while (mProcessing.load())
			{
				std::this_thread::sleep_for(std::chrono::microseconds(10));
			}

			// 入力を終了
			[mSession removeInput:	mInput];

			// 出力を終了
			[mSession removeOutput:	mOutput];  
			[mOutput setSampleBufferDelegate:nil queue:NULL];

			// セッションを開放
			[mSession release];

			mSession	= nullptr;
			mInput		= nullptr;
			mOutput		= nullptr;
		}
		return	true;
	}

	// ========================================================================
	//! 画像処理を行います。
	// ------------------------------------------------------------------------
	void process(CMSampleBufferRef _smpleBuffer)
	{@autoreleasepool{
		// 念のために処理が間に合っていないときはスキップ
		if (!mActive || mProcessing.load())
		{
			return;
		}

		mProcessing.store(true);

		CVImageBufferRef	pixelBuffer	= CMSampleBufferGetImageBuffer(_smpleBuffer);
		CVPixelBufferLockBaseAddress(pixelBuffer, 0);

		int			width		= (int)CVPixelBufferGetWidthOfPlane(pixelBuffer, 0);
		int			height		= (int)CVPixelBufferGetHeightOfPlane(pixelBuffer, 0);
		int			widthUV		= (int)CVPixelBufferGetWidthOfPlane(pixelBuffer, 1);
		int			heightUV	= (int)CVPixelBufferGetHeightOfPlane(pixelBuffer, 1);
	 
		uint8_t*	buffY	= (unsigned char *)CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 0);
		uint8_t*	buffUV	= (unsigned char *)CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 1);

		image_type			planes[3];
		planes[0].data(buffY);
		planes[0].size({width, height});
		planes[0].length		= width * height;
		planes[0].stride({1, width});

		planes[1].data(buffUV);
		planes[1].size({widthUV, heightUV});
		planes[1].length		= widthUV * heightUV;
		planes[1].stride({2, widthUV * 2});

		planes[2].data(buffUV + 1);
		planes[2].size({widthUV, heightUV});
		planes[2].length		= widthUV * heightUV;
		planes[2].stride({2, widthUV * 2});

		mCaptureCallback(planes, 3);

		CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
		mProcessing.store(false);
	}}

	static UIViewController<AVCaptureVideoDataOutputSampleBufferDelegate>* create_delegate(camera_impl*);

	std::string										mName;
	std::atomic<bool>								mProcessing;
	AVCaptureDevice*								mDevice		= nullptr;
	AVCaptureSession*								mSession	= nullptr;
	AVCaptureDeviceInput*							mInput		= nullptr;
	AVCaptureVideoDataOutput*						mOutput		= nullptr;
	std::function<void(image_type const*, size_t)>	mCaptureCallback;
	bool											mActive		= false;
};




void camera::permission(std::function<void(bool)> _callback)
{
	AMTRS_FORCE_LOG("Permission %d ***************************************", (int)DEBUG_LOG_LEVEL);
	AMTRS_DEBUG_LOG("Permission Check");
	auto	status	= [AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeVideo];
	if (status == AVAuthorizationStatusAuthorized)
	{
		AMTRS_DEBUG_LOG("Permission YES");
		_callback(true);
		AMTRS_DEBUG_LOG("Permission YES finish");
		return;
	}
	if ((status == AVAuthorizationStatusDenied) || (status == AVAuthorizationStatusRestricted))
	{
		AMTRS_DEBUG_LOG("Permission NO");
		_callback(false);
		AMTRS_DEBUG_LOG("Permission NO finish");
		return;
	}

	[AVCaptureDevice requestAccessForMediaType:AVMediaTypeVideo completionHandler:^(BOOL granted) {
		if (granted) {
			// 許可された場合の処理
			dispatch_async(dispatch_get_main_queue(), ^{
				AMTRS_DEBUG_LOG("Permission YES");
				_callback(true);
				AMTRS_DEBUG_LOG("Permission YES finish");
			});
		} else {
			// 許可してもらえない場合
			dispatch_async(dispatch_get_main_queue(), ^{
				AMTRS_DEBUG_LOG("Permission NO");
				_callback(false);
				AMTRS_DEBUG_LOG("Permission NO finish");
			});
		}
	}];
	AMTRS_DEBUG_LOG("Permission unkown");
}


bool camera::enumlate(std::function<bool(ref<camera>)> _callback)
{
	auto*	discoverty	=
					[AVCaptureDeviceDiscoverySession
						discoverySessionWithDeviceTypes:@[AVCaptureDeviceTypeBuiltInWideAngleCamera] 
						mediaType:						AVMediaTypeVideo 
						position:						AVCaptureDevicePositionUnspecified
					];
	auto*	devices = [discoverty devices];
	for (AVCaptureDevice* device in devices)
	{
		if (![device hasMediaType: AVMediaTypeVideo])
		{
			continue;
		}
		ref<camera_impl>	cam	= new camera_impl(device);
		if (_callback(cam))
		{
			break;
		}
	}
	return	true;
}


AMTRS_MEDIA_NAMESPACE_END

// ****************************************************************************
//! AVCaptureVideoDataOutputからのコールバック
// ----------------------------------------------------------------------------
//! videostreamクラスへのバイパスを行う。
// ----------------------------------------------------------------------------
@interface VideoController : UIViewController<AVCaptureVideoDataOutputSampleBufferDelegate>
{
	@public amtrs::media::camera_impl*	instance;
}
@end
@implementation VideoController
// AVCaptureVideoDataOutputSampleBufferDelegate:captureOutput:didOutputSampleBuffer:fromConnection
- (void)
	captureOutput:			(AVCaptureOutput *)		captureOutput
	didOutputSampleBuffer:	(CMSampleBufferRef)		sampleBuffer
	fromConnection:			(AVCaptureConnection *)	connection
{
	// videostreamで処理をする
	instance->process(sampleBuffer);
}
@end


UIViewController<AVCaptureVideoDataOutputSampleBufferDelegate>* amtrs::media::camera_impl::create_delegate(camera_impl* _instance)
{
	VideoController*	controller	= [[VideoController alloc] init];
	controller->instance	= _instance;
	return	controller;
}

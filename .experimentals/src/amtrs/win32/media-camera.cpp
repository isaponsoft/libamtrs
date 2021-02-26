/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#if	0
#include <thread>
#include <amtrs/.driver/win32/.api-winrt-std.hpp>
#include <amtrs/.driver/win32/.api-media-source.hpp>

#include <winrt\Windows.System.Threading.h>
#include <winrt\Windows.Foundation.h>
#include <winrt\Windows.Foundation.Collections.h>
#include <winrt\Windows.Media.Capture.h>
#include <winrt\Windows.Media.Core.h>
#include <winrt\Windows.Media.Devices.h>
#include <winrt\Windows.Media.MediaProperties.h>
#include <winrt\Windows.Media.Capture.Frames.h>
#include <winrt\Windows.Devices.Enumeration.h>
#include <winrt\Windows.Graphics.Imaging.h>
#pragma comment(lib, "windowsapp")

#include <amtrs/media/camera.hpp>

using namespace winrt;
using namespace Windows::Media::Capture;
using namespace Windows::Media::MediaProperties;
using namespace Windows::Media::Capture::Frames;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Storage;
using namespace Windows::Graphics::Imaging;
using namespace Windows::Media::Core;


AMTRS_MEDIA_NAMESPACE_BEGIN

class	camera_impl
		: public camera
{
public:
	camera_impl(MediaFrameSourceGroup _group, MediaFrameSourceInfo _info)
		: mSourceGroup(_group)
		, mSourceInfo(_info)
	{
	}

	virtual bool formats(std::function<void(format)> _callback) override
	{
		for (MediaCaptureVideoProfileMediaDescription f : mSourceInfo.VideoProfileMediaDescription())
		{
			format		fmt;
			//double		fps	= f.FrameRate();
			//AMTRS_DEBUG_LOG("\"%4s\" %dx%d %3.2ffps", subtype.c_str(), (int)f.Width(), (int)f.Height(), fps);

			// サイズ
			fmt.size	= size2<int>((int)f.Width(), (int)f.Height());

			// フォーマット
			std::string	subtype	= amtrs::to_string(f.Subtype());
			if (subtype == "NV12")
			{
				fmt.type	= format_types::yuv_420_888;
				fmt.color	= color_format::gray8;
			}
		#if	0
			else if (subtype == "YUY2")
			{
				fmt.type	= format_types::yuv_420_888;
				fmt.color	= color_format::gray8;
			}
			else if (subtype == "MJPG")
			{
				fmt.type	= format_types::yuv_420_888;
				fmt.color	= color_format::gray8;
			}
			else if (subtype == "RGB24")
			{
				fmt.type	= format_types::yuv_420_888;
				fmt.color	= color_format::gray8;
			}
		#endif
			else
			{
				continue;
			}
			_callback(fmt);
		}
		return	true;
	}


	virtual bool start(format _format, std::function<void(image_type const*, size_t)> _capture) override
	{
		MediaCapture			mediaCapture		= create_capture();
		MediaFrameSource		colorFrameSource	= set_format(mediaCapture, _format);
		MediaFrameReader		mediaFrameReader	= mediaCapture.CreateFrameReaderAsync(colorFrameSource).get();
		mediaFrameReader.FrameArrived([=](MediaFrameReader _sender, MediaFrameArrivedEventArgs _args)
		{
			MediaFrameReference		mediaFrameReference = _sender.TryAcquireLatestFrame();
			if (mediaFrameReference == nullptr)
			{
				return;
			}
			VideoMediaFrame			videoMediaFrame		= mediaFrameReference.VideoMediaFrame();
			SoftwareBitmap			softwareBitmap		= videoMediaFrame.SoftwareBitmap();
			if (softwareBitmap == nullptr)
			{
				return;
			}
			SoftwareBitmap			frame(softwareBitmap);


			BitmapBuffer				bitmap	= frame.LockBuffer(BitmapBufferAccessMode::Read);
			IMemoryBufferReference	 	ref		= bitmap.CreateReference();
			BYTE*						data;
			UINT32						capacity;
			ref.as<IMemoryBufferByteAccess>()->GetBuffer(&data, &capacity);
			int						planeCount	= bitmap.GetPlaneCount();

			image_type				images[3];
			for (int planeIndex = 0; planeIndex < planeCount; ++planeIndex)
			{
				BitmapPlaneDescription	plane	= bitmap.GetPlaneDescription(planeIndex);

				int		startIndex	= plane.StartIndex;
				int		stride		= plane.Stride;
				int		width		= plane.Width;
				int		height		= plane.Height;

				images[planeIndex].data(data + startIndex);
				images[planeIndex].size({width, height});
				images[planeIndex].length	= width * height;
				images[planeIndex].stride({1, stride});
			}
			images[1].stride({2, images[1].stride().y});

			images[2]	= images[1];
			images[2].data(images[1].data() + 1);
			images[2].stride({2, images[2].stride().y});

			_capture(images, 3);
		});
		mediaFrameReader.StartAsync().get();


		mReader	= mediaFrameReader;
		return	true;
	}

	MediaCapture create_capture()
	{
		MediaCapture						mediaCapture;
		MediaCaptureInitializationSettings	settings;
		settings.SourceGroup(mSourceGroup);
		settings.SharingMode(MediaCaptureSharingMode::ExclusiveControl);			// 占有
		settings.MemoryPreference(MediaCaptureMemoryPreference::Cpu);				// CPUから使えるメモリ
		settings.StreamingCaptureMode(StreamingCaptureMode::Video);					// ビデオキャプチャ
		try
		{
			mediaCapture.InitializeAsync(settings).get();
			return	mediaCapture;
		}
		catch (winrt::hresult_error const& e)
		{
			AMTRS_DEBUG_LOG("Exception: %s", amtrs::to_string(e.message()).c_str());
			return	nullptr;
		}
	}

	MediaFrameSource set_format(MediaCapture _mediaCapture, format _format)
	{
		// キャプチャするフォーマットを取得する
		MediaFrameSource	colorFrameSource = _mediaCapture.FrameSources().Lookup(mSourceInfo.Id());
		MediaFrameFormat	preferredFormat(nullptr);
		for (MediaFrameFormat f : colorFrameSource.SupportedFormats())
		{
			if (f.VideoFormat().Width() == _format.size.width
			 && f.VideoFormat().Height() == _format.size.height
			 && amtrs::to_string(f.Subtype()) == "NV12")
			{
				preferredFormat	= f;
				break;
			}
		}
		if (preferredFormat == nullptr)
		{
			AMTRS_DEBUG_LOG("No such format");
			return	nullptr;
		}
		colorFrameSource.SetFormatAsync(preferredFormat).get();
		return	colorFrameSource;
	}

	virtual bool restart() override
	{
		return	false;
	}

	virtual bool pause() override
	{
		return	false;
	}

	virtual bool stop() override
	{
		return	false;
	}


	MediaFrameSourceGroup	mSourceGroup;
	MediaFrameSourceInfo	mSourceInfo;
	MediaFrameReader		mReader	= nullptr;
};


void camera::permission(std::function<void(bool)> _callback)
{
	_callback(true);
}


bool camera::enumlate(std::function<bool(ref<camera>)> _callback)
{
	// ビデオ録画（色情報サポート）を列挙する。
	auto	sources	= os::win32::enumrate_media_source([](auto group, auto info)
						{
							return	info.MediaStreamType() == MediaStreamType::VideoRecord
								 && info.SourceKind() == MediaFrameSourceKind::Color;
						});
	for (auto source : sources)
	{
		ref<camera_impl>	cam	= new camera_impl(std::get<0>(source), std::get<1>(source));
		if (_callback(cam))
		{
			break;
		}
	}
	return	true;
}

AMTRS_MEDIA_NAMESPACE_END
#endif

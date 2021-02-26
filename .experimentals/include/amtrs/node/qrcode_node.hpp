/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__qrcode_node__hpp
#define	__libamtrs__node__qrcode_node__hpp
#include "camera_node.hpp"
#include "fill_node.hpp"
#include "fillwrap_node.hpp"
AMTRS_NAMESPACE_BEGIN



template<class SuperT>
class	qrcode_node
		: public camera_node<SuperT>
{
public:
	using	super_type	= camera_node<SuperT>;

	qrcode_node(QRreader::create_params _params)
		: mParams(std::move(_params))
	{
		media::camera::permission([this](bool _perm)
		{
			AMTRS_DEBUG_LOG("permission %s", _perm ? "TRUE" : "FALSE");
			if (_perm)
			{
				auto [dev, img] = media::camera::choise(mParams.min_width, mParams.max_width);

				AMTRS_DEBUG_LOG("%dx%d", img.size.width, img.size.height);
				super_type::start(dev, img);
			}
			else
			{
				this->schedule_once([this]()
				{
					auto	black	= new fillwrap_node<fill_node<typename super_type::node_type>>();
					black->set_color({0,0,0,1});
					this->add_child(black);
				});
			}
		});
	}


	virtual std::string title() override
	{
		return	u8"QR";
	}

	void set_listener(std::function<void(std::string&)> _listener)
	{
		mListener	= std::move(_listener);
	}

protected:
	virtual void on_qr_decoded(std::string& _text)
	{
		if (mListener)
		{
			mListener(_text);
		}
	}

	virtual void on_capture(imagebuff<uint8_t> const& Y, imagebuff<uint8_t> const& U, imagebuff<uint8_t> const& V) override
	{
		super_type::on_capture(Y, U, V);
		if (mDetecting)
		{
			return;
		}

		mDetecting	= true;
		std::thread([thiz = ref<qrcode_node>(this), y = Y.share()]()
		{
			if (!thiz->capturing())
			{
				thiz->mDetecting	= false;
				return;
			}

			// カメラを覆い隠すなどして真っ暗な画像をDetectorに渡すと
			// cv::Exception が発生するので、try catch で囲っておく。
			try
			{
				cv::Mat					input_image(cv::Size(y.size().width, y.size().height), CV_8UC1);
				std::memcpy(input_image.data, y.data(), y.size().width * y.size().height);
				cv::QRCodeDetector		detector;

				// detectAndDecodeを呼ぶより、別々に呼び出したほうがパフォーマンスが良い
				std::vector<cv::Point>	keypoint;
				detector.detect(input_image, keypoint);
				if (thiz->capturing() && !keypoint.empty())
				{
					std::string		text		= detector.decode(input_image, keypoint);
					if (!text.empty())
					{
						thiz->schedule_once([thiz, tx = std::move(text)]()
						{
							if (!thiz->capturing()) { return; }
							thiz->update_text(tx);
						});
					}
				}
			}
			catch (...)
			{
				AMTRS_INFO_LOG("Exception");
			}
			thiz->mDetecting	= false;
		}).detach();
	}

	void update_text(std::string _text)
	{
		if (mText == _text)
		{
			return;
		}
		mText	= _text;
		AMTRS_DEBUG_LOG("decoded QR CODE: %s", mText.c_str());
		on_qr_decoded(mText);
	}

private:
	QRreader::create_params				mParams;
	std::string							mText;
	std::function<void(std::string&)>	mListener;
	bool								mDetecting	= false;
};
AMTRS_NAMESPACE_END
#endif

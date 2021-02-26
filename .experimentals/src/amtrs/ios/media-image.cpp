/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <thread>
#include <amtrs/media/image.hpp>
AMTRS_MEDIA_NAMESPACE_BEGIN


class	image_
		: public image
{
public:
	virtual void pickup(std::function<void(std::string)> _listener) override
	{
	}


	// ==============================================================
	//! 選択されたイメージを読み込んでビットマップを返します。
	// --------------------------------------------------------------
	virtual bitmap<rgba<uint8_t>> read(std::string const& _imageIdentify) override
	{
		return	{};
	}
};


ref<image> image::create()
{
	return	new image_();
}


AMTRS_MEDIA_NAMESPACE_END

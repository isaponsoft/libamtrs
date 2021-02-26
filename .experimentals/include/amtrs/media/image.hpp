/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__media__image__hpp
#define	__libamtrs__media__image__hpp
#include <functional>
#include "../graphics.hpp"
#include "types.hpp"
AMTRS_MEDIA_NAMESPACE_BEGIN

// ****************************************************************************
//! デバイスに保存されている画像を管理します。
// ----------------------------------------------------------------------------
//! OSの機能を用いて画像を読み取ります。
// ----------------------------------------------------------------------------
class	image
		: public ref_object
{
public:
	static ref<image> create();

	// ==============================================================
	//! メディア選択システムを呼び出し、選択されたメディアの識別子を返します。
	// --------------------------------------------------------------
	virtual void pickup(std::function<void(std::string)> _listener) = 0;

	// ==============================================================
	//! 選択されたイメージを読み込んでビットマップを返します。
	// --------------------------------------------------------------
	virtual bitmap<rgba<uint8_t>> read(std::string const& _imageIdentify) = 0;
};




AMTRS_MEDIA_NAMESPACE_END
#endif

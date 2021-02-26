/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__media__audio__hpp
#define	__libamtrs__media__audio__hpp
#include <functional>
#include "types.hpp"
AMTRS_MEDIA_NAMESPACE_BEGIN

//! デバイスに保存されているオーデォオを管理します。
class	audio
		: public ref_object
{
public:
	static ref<audio> create();

	// ==============================================================
	//! オーディオ選択システムを呼び出し、選択されたオーディオの識別子を返します。
	// --------------------------------------------------------------
	virtual void pickup(std::function<void(std::string)> _listener) = 0;

	// ==============================================================
	//! 選択されたオーディオを簡易再生します。
	// --------------------------------------------------------------
	//! 再生タイミングなど細かい調整が必要な場合は amtrs::sound を
	//! 使用してください。
	// --------------------------------------------------------------
	virtual void play(std::string const& _identify) = 0;

};




AMTRS_MEDIA_NAMESPACE_END
#endif

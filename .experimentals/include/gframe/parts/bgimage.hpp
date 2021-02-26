/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__gframe__parts__bgimage__hpp
#define	__gframe__parts__bgimage__hpp
#include <amtrs/g3d/texture.hpp>
#include "parts.hpp"
GFRAME_NAMESPACE_BEGIN

// ****************************************************************************
//! 背景画像を表示をするためのモジュール
// ----------------------------------------------------------------------------
class	bgimage
		: public parts
{
public:
	using	super_type	= parts;

	static constexpr int	default_priority	= -100;


	virtual void change_bg(std::string const& _bg_filename) = 0;

	virtual void change_bg(amtrs::g3d::texture* _bg) = 0;
};


GFRAME_NAMESPACE_END
#endif

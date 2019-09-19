/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__g3d__shader__hpp
#define	__libamtrs__g3d__shader__hpp
#include <string_view>
#include "def.hpp"
#include "attribute.hpp"
AMTRS_G3D_NAMESPACE_BEGIN

class	device;

// ============================================================================
//! シェーダーを抽象化したインターフェースです。
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
class	shader
		: public resorces::memory_base
{
public:
	//! シェーダーが有効な場合に true になります。
	//! デバイスがロストした場合など、シェーダーが無効になった場合は false を返します。
	virtual bool is_enable() const noexcept = 0;
};


AMTRS_G3D_NAMESPACE_END
#endif

/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef __libamtrs__type_utils__invalid_this__hpp
#define __libamtrs__type_utils__invalid_this__hpp
#include "def.hpp"
AMTRS_NAMESPACE_BEGIN

// ========================================================================
//!	@breif	呼び出そうとした機能が無効であることを示します。
// ------------------------------------------------------------------------
//!	テンプレート引数 V の型に関わらず、常に value = false となる構造体を
//!	定義します。
//!
//!	主に static_assert(sa::invalid_this<T>::value, "Message") のように
//!	static_assert() と組み合わせて利用します。
//!
//!	テンプレートの中には、特殊化を行った場合に無効となるメソッドなどが
//!	ある場合があります。そうした場合に、invalid_this を使用することで
//!	このメソッドは特殊化において無効であることが明示できます。
// ------------------------------------------------------------------------
template<class V>
struct	invalid_this
{
	static constexpr bool	value	= !std::is_same<V, V>::value;
};


AMTRS_NAMESPACE_END
#endif

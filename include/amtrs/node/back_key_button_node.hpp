/******************************************************************************
Copyright (c) 2019, isaponsoft (Isao Shibuya)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the libamtrs project.
******************************************************************************/
#ifndef	__libamtrs__node__back_key_button_node__hpp
#define	__libamtrs__node__back_key_button_node__hpp
#include "node.hpp"
#include "back_key_node.hpp"
#include "button_node.hpp"
AMTRS_NAMESPACE_BEGIN

// ****************************************************************************
//! Androidのバックキー、もしくはそれに相当するキーイベントがあったときにボタンを押したものとして処理します。
// ----------------------------------------------------------------------------
//! バックキーが押されたときに、コールバックを呼び出します。コールバックが
//! true を返した場合、バックキーをアプリが処理したものとしてOSに通知します。
//! それ以外は button_node と同じです。
// ****************************************************************************
template<class NodeT>
class	back_key_button_node
		: public back_key_node<button_node<NodeT>>
{
	using	_base_type	= back_key_node<button_node<NodeT>>;
public:
	using	_base_type::_base_type;

protected:
	virtual bool on_back() override
	{
		_base_type::on_back();
		_base_type::on_button_trigger({0,0,0});
		return	true;
	}

private:
	std::function<bool()>	mOnBackCallback;
};



AMTRS_NAMESPACE_END
#endif

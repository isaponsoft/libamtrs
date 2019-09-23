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
#ifndef	__libamtrs__node__adbanner_node__hpp
#define	__libamtrs__node__adbanner_node__hpp
#include "../google/firebase_admob.hpp"
#include "node.hpp"
AMTRS_NAMESPACE_BEGIN

//! 広告バナーをノード化を行い、update() をの呼び出しを自動化します。。
//! 広告バナーは、ノードシステムの座標による位置関係を無視し、常に画面サイズに対して動作します。
class	adbanner_node
		: public node
{
public:
	using	position_mode	= google::firebase::admob::banner::position_mode;

	static ref<adbanner_node> create()
	{
		ref<adbanner_node>	thiz	= new adbanner_node();
		auto	admob	= google::firebase::admob::create();
		thiz->mBanner	= google::firebase::admob::banner::create(admob, get_interfaces().window);
		return	thiz;
	}

	virtual void on_created() override
	{
		mBanner->load_ad(new google::firebase::admob::adrequest());
	}

	virtual void on_update(float _delta) override
	{
		node::on_update(_delta);
		mBanner->update();
	}


	rect<int> content_size_with_pixel() const noexcept { return mBanner->content_size_with_pixel(); }

	void set_position_mode(position_mode _pos) { mBanner->set_position_mode(_pos); }

private:
	adbanner_node(){}

	ref<google::firebase::admob::banner>	mBanner;
};


AMTRS_NAMESPACE_END
#endif

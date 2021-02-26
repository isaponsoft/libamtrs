/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
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

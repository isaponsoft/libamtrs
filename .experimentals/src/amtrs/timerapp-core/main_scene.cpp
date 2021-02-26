/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/application/@>
#include <amtrs/chrono/@>
#include <amtrs/g3d/@>
#include <amtrs/node/@>
#include <amtrs/node/anchor_node.hpp>
#include <amtrs/node/back_key_node.hpp>
#include <amtrs/node/clipping_node.hpp>
#include <amtrs/node/fill_node.hpp>
#include <amtrs/node/fillwrap_node.hpp>
#include <amtrs/node/flexbox_node.hpp>
#include <amtrs/node/input_untransparentable_node.hpp>
#include <amtrs/node/intersects_notification_node.hpp>
#include <amtrs/node/linerlayout_node.hpp>
#include <amtrs/node/paging_node.hpp>
#include <amtrs/node/stackpaging_node.hpp>
#include <amtrs/node/listener/remove_child.hpp>
#include <gframe/@>
#include <app-config.hpp>		// Application configuration.

#include <timerapp-core/application.hpp>
#include <timerapp-core/contents.hpp>
#include <timerapp-core/system.hpp>

using namespace	amtrs;
using namespace	gframe;

void system_intialize();
void contents_intialize();
void contents_stop();


enum	plane_priority
{
	pp_bg,
	pp_home,
//	pp_mainmenu,
	pp_contents,
	pp_loading,
	pp_dialog,
	pp_timer,
};



class	main_scene
		: public scene
{
public:
	using	super_type		= scene;
	using	contents_box	= fillwrap_node<paging_node<node>>;


	static inline	main_scene*				sMainScene			= nullptr;
	static inline	node*					sMainNode			= nullptr;
	static inline	node*					sHomeNode			= nullptr;

	main_scene()
	{
		system_intialize();
		sMainScene	= this;
	}

	virtual void on_enter() override
	{
		super_type::on_enter();
		this->schedule_once([this]()
		{
			// ローディングインジケーター
			this->add_child(contents::create_loading())
			->set_priority(pp_loading);

			// ダイアログ
			this->add_child(dialog::initialize())
			->set_priority(pp_dialog);


			// 壁紙
			this->add_child(wallpaper::initialize())
			->set_priority(pp_bg);

			// メインのコンテンツを入れておくノード
			sMainNode	= new fillwrap_node<node>();
			this->add_child(sMainNode)
			->set_priority(pp_contents);


			// ホーム画面を追加
			if (auto hc = get_application_config()->create_home; hc)
			{
				sHomeNode	= sMainNode->add_child(hc());
				sHomeNode->set_priority(pp_home);
			}

			// タイマー
			this->add_child(timer::initialize())
			->set_priority(pp_timer);
		});
	}


	virtual void on_invisible() override
	{
		super_type::on_invisible();
		contents_stop();
	}
};




namespace contents {



class	containor
		: public fillwrap_node<input_untransparentable_node<remove_child_node<contents_containor>>>
{
public:
	using	super_type	= fillwrap_node<input_untransparentable_node<remove_child_node<contents_containor>>>;

	static inline containor*	sThiz;


	containor()
	{
		sThiz		= this;

		mContents	= new stackpaging_node<back_key_node<node>>();
		mContents->set_page_vector({1, 0, 0});
		this->add_child(mContents);
		mContents->set_back_callback([this]()
		{
			pop(1);
			return	true;
		});

		if (get_application_config()->create_header)
		{
			mHeader	= get_application_config()->create_header([this](auto _msg)
			{
				pop(1);
			});
			this->add_child(mHeader);
			mHeader->set_priority(1000);
		}
	}

	~containor()
	{
		sThiz		= nullptr;
	}

	void update_title(std::string _title) override
	{
		if (mHeader)
		{
			mHeader->set_title(std::move(_title));
		}
	}


	void set_contents(amtrs::ref<contents_base> _node) override
	{
		if (mHeader)
		{
			mHeader->set_name(_node->name());
			mHeader->set_title(_node->title());
		}
		mContents->push(_node);
		_node->set_size(mContents->size());
		reposition();
		_node->on_containor(this, setup);
	}


	void pop(size_t _poppage) override
	{
		static_cast<contents_base*>(mContents->get_active_page())->on_containor(this, removing);
		while (_poppage > 0)
		{
			if (!mContents->stacking())
			{
				mContents->remove_from_parent();
			}
			else
			{
				mContents->pop();
				if (auto c = mContents->front(); c)
				{
					c->set_size(mContents->size());
					if (mHeader)
					{
						mHeader->set_title(static_cast<contents_base*>(c)->title());
					}
				}
			}
			--_poppage;
		}
	}

protected:
	virtual void on_size_change(typename super_type::size_type& _size) override
	{
		super_type::on_size_change(_size);
		reposition();
	}

	virtual void on_children_size_change(typename super_type::transform_component* _c) override
	{
		super_type::on_children_size_change(_c);
		if (_c == mHeader)
		{
			reposition();
		}
	}

	void reposition()
	{
		if (mContents)
		{
			float	y	= 0;
			if (mHeader)
			{
				y	= mHeader->size().height;
			}
			mContents->set_position({0, y, 0});
			mContents->set_size({this->size().width, this->size().height-y, this->size().depth});
			if (auto c = mContents->front(); c)
			{
				c->set_size(mContents->size());
			}
		}
	}

private:
	ref<contents_header>					mHeader;
	stackpaging_node<back_key_node<node>>*	mContents	= nullptr;
};



void set_main_contents(amtrs::ref<contents_base> _contents)
{
	if (containor::sThiz)
	{
		containor::sThiz->set_contents(_contents);
		return;
	}

	// ヘッダを制御するためのコンテナ
	auto		box	= new containor();
	box->set_contents(_contents);
	box->set_pivot({1, 0, 0});

	// メニューより前に表示する
	box->set_priority(pp_contents);
	main_scene::sMainNode->add_child(box);


	// 表示のための処理ピボットをいじってスクロールイン
	//mainmenu::show(false);
	box->run_action(new pivot_to_action<>({0,0,0}, 0.20f))
	->set_finish_callback([]()
	{
		if (main_scene::sHomeNode)
		{
			main_scene::sHomeNode->set_enable(false);
		}
	});


	// 隠すときの処理、ピボットをいじってスクロールアウト
	box->set_remove_child_listener([box](auto _node)
	{
		//mainmenu::show(true);
		if (main_scene::sHomeNode)
		{
			main_scene::sHomeNode->set_enable(true);
		}
		box->run_action(new pivot_to_action<>({1,0,0}, 0.20f))
		->set_finish_callback([box]()
		{
			node::rerender_request();
			box->remove_from_parent();
		});
		return	true;
	});
}


void palen_visible(bool _vis)
{
	main_scene::sMainNode	->set_visibility(_vis);
}


} // contents




module<scene>	_main_scene("main", []() -> ref<scene>
{
	return	new main_scene();
});

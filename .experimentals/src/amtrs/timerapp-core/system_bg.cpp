/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <gframe/@>
#include <amtrs/node/fill_node.hpp>
#include <amtrs/node/fillwrap_node.hpp>
#include <amtrs/media/image.hpp>
#include <timerapp-core/application.hpp>
#include <timerapp-core/contents.hpp>
#include <timerapp-core/system.hpp>

using namespace	amtrs;


// ============================================================================
//! 移動量を変更します。
// ----------------------------------------------------------------------------
//!	example)
//!		act = new move_by_action<node::action_type>(
//!			{1, 1, 1},	// to translate.
//!			1.0f		// duration time.
//!		);
// ----------------------------------------------------------------------------
template<class TweenT = waveform::linear<float>>
class	color_by_action
		: public basic_tween_action<TweenT>
{
public:
	using	super_type		= basic_tween_action<TweenT>;
	using	value_type		= rgba<float>;

	template<class... TweenArgs>
	color_by_action(value_type _to, float _duration, TweenArgs&&... _targs)
		: super_type(_duration, std::forward<TweenArgs>(_targs)...)
		, mEnd(_to)
	{}

	virtual void on_start() override
	{
		super_type::on_start();
		if (!mTarget)
		{
			mTarget	= static_cast<node*>(this->target());
		}
		mStart	= mTarget->get_color();
	}

	virtual void on_update(float _time) override
	{
		super_type::on_update(_time);
		mTarget->set_color(mEnd * _time + mStart * (1.0f - _time));
	}

protected:
	node*				mTarget	= nullptr;
	value_type			mStart;
	value_type			mEnd;
};



class	wapllaper_node
		: public fillwrap_node<amtrs::node>
{
public:
	using	super_type	= fillwrap_node<amtrs::node>;

	wapllaper_node()
	{
		sThis	= this;
		this->schedule_once([this]()
		{
			this->refresh();
		});
	}

	~wapllaper_node()
	{
		sThis		= nullptr;
	}

	void refresh()
	{
		auto&	looks	= save::storage::current()->setting.looks;
		if (looks.mode == save::setting_looks::bgmode_image)
		{
			mCurrent	= save::storage::current()->setting.looks.bgimage;
			std::thread([this]()
			{
				ref<g3d::texture>	tex;
				auto				it	= bg::find_item(mCurrent);
				if (it)
				{
					tex	= ::gframe::load_texture(mCurrent);
					if (!tex)
					{
						return;
					}
				}
				else
				{
					auto	bmp	= media::image::create()->read(mCurrent);
					if (!bmp.empty())
					{
						tex	= g3d::texture::create(std::move(bmp));
					}
				}
				if (!tex)
				{
					if (mImage)
					{
						mImage->remove_from_parent();
						mImage		= nullptr;
					}
					if (!mFill)
					{
						mFill	= new fill_node<node>();
						this->add_child(mFill);
					}
					auto&	looks	= save::storage::current()->setting.looks;
					mFill->set_color({(float)looks.color.r / 255.0f, (float)looks.color.g / 255.0f, (float)looks.color.b / 255.0f, 0.5f});
					return;
				}

				this->schedule_once([this, tex]()
				{
					if (mFill)
					{
						mFill->remove_from_parent();
						mFill		= nullptr;
					}
					if (!mImage)
					{
						mImage		= new sprite_node<node>();
						mImage->set_color({0,0,0,1});
						mImage->run_action(new color_by_action<>({1,1,1,1}, 0.3f));
						mImage->set_pivot({0.5f, 0, 0});
						this->add_child(mImage);
					}
					mImage->set_texture(tex);
					update_position();
				});
			}).detach();
		}
		if (mImage)
		{
			mImage->remove_from_parent();
			mImage		= nullptr;
		}
		if (!mFill)
		{
			mFill	= new fill_node<node>();
			this->add_child(mFill);
		}
		mFill->set_color({(float)looks.color.r / 255.0f, (float)looks.color.g / 255.0f, (float)looks.color.b / 255.0f, 0.5f});
	}


	virtual void on_size_change(typename super_type::size_type& _size) override
	{
		super_type::on_size_change(_size);
		update_position();
	}

	void update_position()
	{
		if (mImage && mImage->texture())
		{
			auto	s	= this->size();
			auto	t	= size2<float>(mImage->texture()->size());
			float	sx	= s.width  / t.width;
			float	sy	= s.height / t.height;
			float	sc	= std::max(sx, sy);
			mImage->set_scale({sc, sc, 0});
			mImage->set_position({s.width/2, 0, 0});
			return;
		}

		if (mFill)
		{
			mFill->set_position({0, 0, 0});
			mFill->set_size(this->size());
		}
	}

	sprite_node<node>*		mImage		= nullptr;
	fill_node<node>*		mFill		= nullptr;
	std::string				mCurrent;

	inline static wapllaper_node*	sThis;
};


amtrs::ref<amtrs::node> wallpaper::initialize()
{
	return	new wapllaper_node();
}



void wallpaper::update()
{
	wapllaper_node::sThis->refresh();
}


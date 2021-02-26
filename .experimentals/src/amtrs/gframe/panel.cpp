/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/application/@>
#include <amtrs/chrono/@>
#include <amtrs/g3d/@>
#include <amtrs/node/@>
#include <gframe/@>
#include <app-config.hpp>
GFRAME_NAMESPACE_BEGIN


auto compute_base_size(panel::side_type _type, panel* _panel)
{
	auto	ps	= _panel->parent()->size();
	auto	sz	= _panel->size();
	auto	tex	= _panel->texture();
	switch (_type)
	{
		case panel::fullsize :
			sz			= ps;
			break;

		case panel::top_side :
			sz.width	= ps.width;
			if (tex)
			{
				sz.height	= (float)tex->size().height;
			}
			break;

		case panel::bottom_side :
			sz.width	= ps.width;
			if (tex)
			{
				sz.height	= (float)tex->size().height;
			}
			break;

		case panel::left_side :
			sz.height	= ps.height;
			if (tex)
			{
				sz.width	= (float)tex->size().width;
			}
			break;

		case panel::right_side :
			sz.height	= ps.height;
			if (tex)
			{
				sz.width	= (float)tex->size().width;
			}
			break;
	}
	return	sz;
}



panel::panel()
{
}


panel::~panel()
{
}


void panel::hidden()
{
	if (mHidden)
	{
		return;
	}

	float		w	= this->size().width;
	float		h	= this->size().height;

	vec3<float>	to	= mSide == top_side		? vec3<float>( 0, -h, 0)
					: mSide == bottom_side	? vec3<float>( 0,  h, 0)
					: mSide == left_side	? vec3<float>(-w,  0, 0)
					: mSide == right_side	? vec3<float>( w,  0, 0)
					: vec3<float>(0, 0, 0);

	this->clear_action();
	this->run_action(new move_by_action<waveform::back_ease_out<float>>(this->children_transform(), to, visible_druction));
	mHidden	= true;
}


void panel::show()
{
	if (!mHidden)
	{
		return;
	}

	this->clear_action();
	this->run_action(new move_by_action<waveform::back_ease_out<float>>(this->children_transform(), {0, 0, 0}, visible_druction));
	mHidden	= false;
}


void panel::set_side(side_type _side)
{
	if (mSide != _side)
	{
		mSide	= _side;
		update_position();
	}
}


void panel::set_bg(std::string const& _imagename)
{
	if (!_imagename.empty())
	{
		auto	tex	= load_texture(_imagename);
		this->set_texture(tex);
	}
	else
	{
		this->set_texture(nullptr);
	}
}


void panel::update_position()
{
	if (mScheduleRepos)
	{
		return;
	}

	mScheduleRepos	= true;
	this->schedule_once([this](float)
	{
		auto	ps	= this->parent()->size();
		auto	sz	= this->size();
		auto	cs	= this->bounds(this->items().begin(), this->items().end());
		if (this->texture())
		{
			auto	tx	= this->texture()->size();
			switch (mSide)
			{
				case panel::fullsize :
					sz			= ps;
					break;

				case panel::top_side :
					sz.width	= ps.width;
					sz.height	= (float)tx.height;
					break;

				case panel::bottom_side :
					sz.width	= ps.width;
					sz.height	= (float)tx.height;
					break;

				case panel::left_side :
					sz.height	= ps.height;
					sz.width	= (float)tx.width;
					break;

				case panel::right_side :
					sz.height	= ps.height;
					sz.width	= (float)tx.width;
					break;
			}
		}
		else
		{
			switch (mSide)
			{
				case panel::fullsize :
					sz			= ps;
					break;

				case panel::top_side :
					sz.width	= ps.width;
					sz.height	= cs.height;
					break;

				case panel::bottom_side :
					sz.width	= ps.width;
					sz.height	= cs.height;
					break;

				case panel::left_side :
					sz.height	= ps.height;
					sz.width	= cs.width;
					break;

				case panel::right_side :
					sz.height	= ps.height;
					sz.width	= cs.width;
					break;
			}
		}
		this->set_size(sz);
		switch (mSide)
		{
			case fullsize :
				this->set_position({0, 0, 0});
				break;

			case top_side :
				this->set_position({0, 0, 0});
				break;

			case bottom_side :
				this->set_position({0, ps.height - cs.height, 0});
				break;

			case left_side :
				this->set_position({0, 0, 0});
				break;

			case right_side :
				this->set_position({ps.width - cs.width, 0, 0});
				break;
		}
		mScheduleRepos	= false;
	});
}



void panel::on_coordinate_modify()
{
	this->set_size(compute_base_size(mSide, this));
	super_type::on_coordinate_modify();
	update_position();
}


void panel::on_size_change(typename super_type::size_type& _size)
{
	_size	= compute_base_size(mSide, this);
	this->reposition(_size);
	auto	cs	= this->bounds(this->items().begin(), this->items().end());
	auto	tex	= this->texture();
	switch (mSide)
	{
		default:
			break;

		case panel::top_side :
			if (!tex)
			{
				_size.height	= cs.height;
			}
			break;

		case panel::bottom_side :
			if (!tex)
			{
				_size.height	= cs.height;
			}
			break;

		case panel::left_side :
			if (!tex)
			{
				_size.width	= cs.width;
			}
			break;

		case panel::right_side :
			if (!tex)
			{
				_size.width	= cs.width;
			}
			break;
	}
	super_type::on_size_change(_size);
}


void panel::on_child(typename super_type::node_type* _child, typename super_type::on_child_event _event)
{
	super_type::on_child(_child, _event);
	update_position();
}


void panel::on_enter()
{
	super_type::on_enter();
	this->set_size(compute_base_size(mSide, this));
	update_position();
}



class	footer
		: public panel
{
public:
	using	super_type	= panel;

	footer()
	{
		set_side(bottom_side);
	}
};


class	header
		: public panel
{
public:
	using	super_type	= panel;

	header()
	{
		set_side(top_side);
	}
};



GFRAME_NAMESPACE_END

gframe::module<gframe::panel>	__panel_default("default::panel", []() -> amtrs::ref<gframe::panel>
{
	return	new gframe::panel();
});

gframe::module<gframe::panel>	__panel_footer("panel::footer", []() -> amtrs::ref<gframe::panel>
{
	return	new gframe::footer();
});

gframe::module<gframe::panel>	__panel_header("panel::header", []() -> amtrs::ref<gframe::panel>
{
	return	new gframe::header();
});

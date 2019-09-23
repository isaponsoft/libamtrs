#ifndef	__libamtrs__node__flickable_pager_node__hpp
#define	__libamtrs__node__flickable_pager_node__hpp
#include "pager_node.hpp"
#include "ui_utils/scroll.hpp"
AMTRS_NAMESPACE_BEGIN


template<class NodeT>
class	flickable_pager_node
		: public updatable_node<pager_node<NodeT>>
{
	using	_base_type	= updatable_node<pager_node<NodeT>>;
public:
	using	node_type	= typename NodeT::node_type;

	flickable_pager_node(uint32_t _flickAsix)
		: mScroller(_flickAsix)
	{}

	void set_input_enable(bool _enable) noexcept { mInputEnable = _enable; }


protected:

	virtual void on_input_cancel() override
	{
		_base_type::on_input_cancel();
		mScroller.cancel(*this);
	}

	virtual node_type* on_input(typename _base_type::pointer_event* _e) override
	{
		auto	mat		= _base_type::world_matrix().inverse();
		if (auto* mouse = _e->event()->get_mouse_event(); mouse && mInputEnable)
		{
			auto	pos	= vec2<float>(vec3<float>(mouse->template get_point<float>(),0) * mat);
			if (mScroller.is_capturing())
			{
				if (mouse->type == input_event::mouse)
				{
					mScroller.input(*this, mouse, &pos);
				}
				if (mouse->state == input_event::status::release)
				{
					auto	spos	= mScroller.position();
					if (spos.x < -this->size().width/3)
					{
						mScroller.set_position({0, 0});
						this->set_page(this->page_index()+1);
					}
					if (spos.x >  this->size().width/3)
					{
						mScroller.set_position({0, 0});
						this->set_page(this->page_index()-1);
					}

					if (spos.y < -this->size().height/3)
					{
						mScroller.set_position({0, 0});
						this->set_page(this->page_index()+1);
					}
					if (spos.y >  this->size().height/3)
					{
						mScroller.set_position({0, 0});
						this->set_page(this->page_index()-1);
					}
				}
				return	this;
			}
			if (auto r = _base_type::on_input(_e); r)
			{
				return	r;
			}
			if (mouse->type == input_event::mouse)
			{
				mScroller.input(*this, mouse, &pos);
			}
			return	nullptr;
		}
		return	_base_type::on_input(_e);
	}

	


	virtual void on_update(float _delta) override
	{
		_base_type::on_update(_delta);
		mScroller.update(_delta);
		auto	now	= vec3<float>(mScroller.position());
		if (mScrollPrev != now)
		{
			mScrollPrev	= now;
			auto		pi	= this->page_index();
			vec3<float>	s	= -this->compute_page_position(pi);
			if (pi == _base_type::npage)
			{
				s	= {0, 0, 0};
			}
			s.x	= compute_scroll_position(s.x + mScrollPrev.x, { -(float)(_base_type::children_count() - 1) * this->size().width , 0 });
			s.y	= compute_scroll_position(s.y + mScrollPrev.y, { -(float)(_base_type::children_count() - 1) * this->size().height, 0 });
			this->set_pages_position(s);
		}
	}

private:
	float compute_scroll_position(float _pos, range<float> _renge)
	{
		float	v	= _pos;
		if (v < _renge.minimam)
		{
			v	= _renge.minimam;
		}
		else if (v > _renge.maximam)
		{
			v	= _renge.maximam;
		}
		return	v;
	}

	uiutils::scroll	mScroller;
	vec3<float>		mScrollPrev		= {0, 0, 0};
	bool			mInputEnable	= true;
};


AMTRS_NAMESPACE_END
#endif

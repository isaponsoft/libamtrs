#ifndef	__libamtrs__node__switch_node__hpp
#define	__libamtrs__node__switch_node__hpp
#include "circle_node.hpp"
#include "fill_node.hpp"
AMTRS_NAMESPACE_BEGIN


// ****************************************************************************
//! スイッチを描画します。
// ----------------------------------------------------------------------------
//! 描画のみを行うため、タッチやクリックによる操作が必要な場合は button_node 
//! などと組み合わせて使用してください。
// ----------------------------------------------------------------------------
template<class NodeT, class MarkerT = circle_node<typename NodeT::node_type>>
class	switch_node
		: public NodeT
{
	using	_base_type		= NodeT;
public:
	using	shader_type	= g3d::fill_shader;
	using	vertex		= typename shader_type::vertex;
	using	uniform		= typename shader_type::uniform;
	using	marker_type	= MarkerT;

	switch_node(bool _on_off = true)
		: mOnOff(_on_off)
	{
		mMarker	= new marker_type();
		mMarker->set_color({1,1,1,1});
		mMarker->set_pivot({0.5f, 0.5f, 0});
		this->add_child(mMarker);
	}


	bool is_on() const noexcept { return get_on_off() == true; }
	bool get_on_off() const noexcept { return mOnOff; }

	void set_on_off(bool _onOff)
	{
		if (mOnOff != _onOff)
		{
			on_change(_onOff);
		}
	}


	void set_div_count(std::size_t _count)
	{
		mDivCount	= _count;
	}

	rgba<float> get_bg_color() const noexcept { return mBgColor; }
	void set_bg_color(rgba<float> _color) noexcept { mBgColor = _color; mVertexModified = true; }

	rgba<float> get_marker_color() const noexcept { return mMarkerColor; }
	void set_marker_color(rgba<float> _color) noexcept { mMarkerColor = _color; mVertexModified = true; }


protected:
	virtual void on_change(bool& _onOff)
	{
		mOnOff = _onOff;
		mVertexModified	= true;
		change_marker_position();
	}

	virtual void on_size_change(typename _base_type::size_type& _size) override
	{
		_base_type::on_size_change(_size);
		mVertexModified	= true;
		change_marker_position();
	}


	virtual void on_render(typename _base_type::render_queue& _queue) override
	{
		mCommandHolder->uniform.perspective	= _queue.projection;
		mCommandHolder->uniform.coordinate	= this->world_matrix();
		if (mCommandHolder->shader.empty())
		{
			_queue.transfer(this, [this](auto device)
			{
				mCommandHolder->shader				= mShader.get_shader(device);
			});
			mCommandHolder->primitive			= g3d::primitive_type::triangle_strip;
		}


		if (mVertexModified)
		{
			mVertexModified	= false;
			update_vertex(_queue.device);
		}

		_queue.draw(mCommandHolder->make_draw_command());
		_base_type::on_render(_queue);
	}


protected:
	auto compute_marker_position() const noexcept -> float
	{
		auto	thisSize		= this->size();
		float	circleWidth		= thisSize.height / 2;
		return	is_on() ? thisSize.width - circleWidth
				        : circleWidth;
	}

	void change_marker_position()
	{
		float	circleWidth		= this->size().height / 2;
		auto	col	= mMarkerColor;
		if (!is_on())
		{
			col.r	*= 0.5f;
			col.g	*= 0.5f;
			col.b	*= 0.5f;
		}
		mMarker->set_color(col);
		mMarker->set_size({circleWidth, circleWidth, 0});
		mMarker->set_position({compute_marker_position(), this->size().height/2, 0});
	}

private:
	void update_vertex(g3d::device* _device)
	{
		std::size_t		count			= mDivCount;
		if (count == 0)
		{
			count	= (int)(this->size().height / 2);
		}
		std::size_t	vtxCount	= 3 * (2 + (count * 2));
		if (mCommandHolder->vertex_buffer.empty() || mCommandHolder->vertex_buffer->size() != vtxCount)
		{
			mCommandHolder->vertex_buffer		= new g3d::vertex_buffer<vertex>(vtxCount);
		}

		mCommandHolder->vertex_buffer->set_translate([this, count, vtxCount](auto* _mem)
		{
			auto			thisSize		= this->size();
			float			circleWidth		= thisSize.height / 2;
			std::vector<vertex>	vtx(vtxCount);
			rgba<float>	bgColor	= mBgColor;
			if (!is_on())
			{
				bgColor.r	*= 0.8f;
				bgColor.g	*= 0.8f;
				bgColor.b	*= 0.8f;
			}


			auto*			out				= vtx.data();
			// 真ん中の四角形
			*out++		= { { circleWidth, 0, 0, 1}, bgColor };
			*out++		= { { thisSize.width - circleWidth, thisSize.height, 0, 1 }, bgColor };
			*out++		= { { circleWidth, thisSize.height, 0, 1 }, bgColor };
			*out++		= { { circleWidth, 0, 0, 1 }, bgColor };
			*out++		= { { thisSize.width - circleWidth, 0, 0, 1 }, bgColor };
			*out++		= { { thisSize.width - circleWidth, thisSize.height, 0, 1 }, bgColor };

			// 半円の座標テーブル
			std::vector<vec4<float>>	posTable(count + 1);
			for (std::size_t c = 0; c <= count; ++c)
			{
				float	angle	= (float)c / (float)count;		// 角度
				float	x		= circleWidth * std::sin(pi * angle);
				float	y		= circleWidth * std::cos(pi * angle);
				posTable[c] = {x, y, 0, 0};
			}

			// 左と右の円
			vec4<float>	centerL	= {                 circleWidth, circleWidth, 0, 1};
			vec4<float>	centerR	= {thisSize.width - circleWidth, circleWidth, 0, 1};
			for (std::size_t c = 0; c < count; ++c)
			{
				// 左
				*out++		= { centerL                  , bgColor };
				*out++		= { centerL - posTable[c    ], bgColor };
				*out++		= { centerL - posTable[c + 1], bgColor };

				// 右
				*out++		= { centerR, bgColor };
				*out++		= { centerR + posTable[c    ], bgColor };
				*out++		= { centerR + posTable[c + 1], bgColor };
			}
			_mem->transfer(vtx.data());
		});
	}


	rgba<float>									mBgColor		= {0.5, 0.5, 0.5, 1};
	rgba<float>									mMarkerColor	= {1, 1, 1, 1};
	marker_type*								mMarker			= nullptr;
	shader_type									mShader;
	std::size_t									mDivCount		= 0;				//!< 半円部分の分割数
	bool										mOnOff;
	ref<g3d::draw_command_holder<shader_type>>	mCommandHolder	= new g3d::draw_command_holder<shader_type>();
	bool										mVertexModified	= true;
};

AMTRS_NAMESPACE_END
#endif

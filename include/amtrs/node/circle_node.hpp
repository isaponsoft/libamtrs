#ifndef	__libamtrs__node__circle_node__hpp
#define	__libamtrs__node__circle_node__hpp
#include "node.hpp"
AMTRS_NAMESPACE_BEGIN


// ============================================================================
//! 円ノード
// ----------------------------------------------------------------------------
template<class NodeT>
class	circle_node
		: public NodeT
{
	using	_base_type	= NodeT;
public:
	using	shader_type	= g3d::fill_shader;
	using	vertex		= typename shader_type::vertex;
	using	uniform		= typename shader_type::uniform;

	void set_div_count(std::size_t _count)
	{
		if (mDivCount != _count)
		{
			mDivCount		= _count;
			mVertexModified	= true;
		}
	}

	virtual void on_render(typename _base_type::render_queue& _queue) override
	{
		auto*	device	= _queue.device;
		mCommandHolder->uniform.perspective	= _queue.projection;
		mCommandHolder->uniform.coordinate	= this->world_matrix();
		if (mCommandHolder->shader.empty())
		{
			_queue.transfer(this, [this](auto device)
			{
				mCommandHolder->shader				= mShader.get_shader(device);
			});
			mCommandHolder->primitive			= g3d::primitive_type::triangle_fan;
		}
		if (mVertexModified)
		{
			mVertexModified	= false;
			update_vertex(device);
		}
		_queue.draw(mCommandHolder->make_draw_command());
		_base_type::on_render(_queue);
	}

protected:
	virtual void on_size_change(typename _base_type::size_type& _size) noexcept override
	{
		_base_type::on_size_change(_size);
		mVertexModified	= true;
	}

	virtual void on_chnage_color() noexcept override
	{
		_base_type::on_chnage_color();
		mVertexModified	= true;
	}

private:
	void update_vertex(g3d::device* _device)
	{
		std::size_t			vtxCount	= mDivCount + 2;
		if (mCommandHolder->vertex_buffer.empty() || mCommandHolder->vertex_buffer->size() != vtxCount)
		{
			mCommandHolder->vertex_buffer		= new g3d::vertex_buffer<vertex>(vtxCount);
		}

		mCommandHolder->vertex_buffer->set_translate([this, vtxCount](auto* _mem)
		{
			std::vector<vertex>	vtx(vtxCount);

			auto*	out	= vtx.data();
			auto	sz	= this->size();
			*out++		= {{sz.width/2, sz.height/2, 0, 1}, this->get_color()};
			for (std::size_t c = 0; c <= mDivCount; ++c)
			{
				float	r	= (float)c / (float)mDivCount;
				float	x	= sz.width /2 * std::cos(2.0f * pi * r);
				float	y	= sz.height/2 * std::sin(2.0f * pi * r);
				*out++		= {{sz.width/2 + x, sz.height/2 + y, 0, 1}, this->get_color()};
			}
			_mem->transfer(vtx.data());
		});
	}

	shader_type									mShader;
	ref<g3d::draw_command_holder<shader_type>>	mCommandHolder	= new g3d::draw_command_holder<shader_type>();
	bool										mVertexModified	= true;
	std::size_t									mDivCount		= 16;
};


AMTRS_NAMESPACE_END
#endif

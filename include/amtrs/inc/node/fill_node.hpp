#ifndef	__libamtrs__node__fill_node__hpp
#define	__libamtrs__node__fill_node__hpp
#include "../g3d/shader/fill_shader.hpp"
#include "node.hpp"
#include "action.hpp"
AMTRS_NAMESPACE_BEGIN

template<class BaseT>
class	fill_node
		: public BaseT
{
public:
	using	_base_type		= BaseT;
	using	shader_type		= g3d::fill_shader;
	using	vertex			= shader_type::vertex;
	using	uniform			= shader_type::uniform;

	using	_base_type::_base_type;

	virtual void on_render(typename _base_type::render_queue& _queue) override
	{
		draw(_queue);
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


	void draw(typename _base_type::render_queue& _queue)
	{
		auto*	device	= _queue.device;
		mCommandHolder->uniform.perspective	= _queue.projection;
		mCommandHolder->uniform.coordinate	= this->render_matrix();
		if (mCommandHolder->shader.empty())
		{
			_queue.transfer(this, [this](auto darg)
			{
				auto	device	= get_g3ddevice(darg);
				mCommandHolder->shader				= mShader.get_shader(device);
			});
			mCommandHolder->primitive			= g3d::primitive_type::triangle_strip;
		}
		if (mVertexModified)
		{
			mVertexModified	= false;
			update_vertex(device);
		}

		_queue.draw(mCommandHolder->make_draw_command());
	}


private:
	void update_vertex(g3d::device* _device)
	{
		if (mCommandHolder->vertex_buffer.empty() || mCommandHolder->vertex_buffer->count() != 4)
		{
			mCommandHolder->vertex_buffer		= new g3d::vertex_buffer<vertex>(4);
		}

		mCommandHolder->vertex_buffer->set_translate([this](auto* _mem)
		{
			auto	pos1	= vec2<float>(0, 0);
			auto	pos2	= pos1 + vec2<float>(size2<float>(this->size()));
			vertex	vtx[4] = 
			{
				{{pos1.x, pos1.y, 0, 1}, this->get_color() },
				{{pos2.x, pos1.y, 0, 1}, this->get_color() },
				{{pos1.x, pos2.y, 0, 1}, this->get_color() },
				{{pos2.x, pos2.y, 0, 1}, this->get_color() },
			};
			_mem->transfer(vtx);
		});
	}


	shader_type									mShader;
	ref<g3d::draw_command_holder<shader_type>>	mCommandHolder	= new g3d::draw_command_holder<shader_type>();
	bool										mVertexModified	= true;
};


AMTRS_NAMESPACE_END
#endif

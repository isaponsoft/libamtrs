#ifndef	__libamtrs__node__sprite_node__hpp
#define	__libamtrs__node__sprite_node__hpp
#include "node.hpp"
AMTRS_NAMESPACE_BEGIN


template<class NodeT>
class	sprite_node
		: public NodeT
{
	using	_base_type		= NodeT;
public:
	using	shader_type		= g3d::texture_shader;
	using	texture_type	= g3d::texture;
	using	vertex			= shader_type::vertex;
	using	uniform			= shader_type::uniform;


	sprite_node()
	{
		reset_draw_status();
	}

	sprite_node(ref<texture_type> _texture)
		: mTexture(std::move(_texture))
	{
		reset_draw_status();
		this->set_size(typename _base_type::size_type(mTexture->size()));
	}





	vec2<float> uv_scale() const noexcept { return mUVScale; }
	void set_uv_scale(vec2<float> _scale) noexcept { mUVScale = _scale; mUpdateVertex	= true; }

	void set_draw_status(const g3d::draw_status& _ds) noexcept { mCommandHolder->status = _ds; }
	g3d::draw_status get_draw_status() const noexcept { return mCommandHolder->status; }

	void reset_draw_status()
	{
		_base_type::rerender_request();
		mCommandHolder->status.set_alpha_blend(g3d::draw_status::blend_type::src_alpha, g3d::draw_status::blend_type::invrs_src_alpha);
		mCommandHolder->status.set_texture2d_filter
		(
			g3d::draw_status::texture2d_filter_type::liner,
			g3d::draw_status::texture2d_filter_type::liner
		);
	}

	texture_type* texture() const noexcept { return mTexture.get(); }

	void set_texture(ref<g3d::texture> _texture, bool _update_size = true)
	{
		_base_type::rerender_request();
		mUpdateVertex	= true;
		mTexture		= std::move(_texture);
		if (!mTexture.empty() && _update_size)
		{
			this->set_size(typename _base_type::size_type(mTexture->size()));
		}
	}

	void update()
	{
		_base_type::rerender_request();
		mUpdateVertex	= true;
	}


	virtual void on_render(typename _base_type::render_queue& _queue) override
	{
		if (!mTexture.empty())
		{
			g3d::device*	device	= _queue.device;
			bool			rebuild	= mCommandHolder->vertex_buffer.empty() || mUpdateVertex;
			mUpdateVertex	= false;

			mCommandHolder->uniform.color		= {1, 1, 1, 1};
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
			mCommandHolder->uniform.texture		= this->texture();
			if (rebuild)
			{
				update_vertex(device);
			}
			_queue.draw(mCommandHolder->make_draw_command());
		}

		_base_type::on_render(_queue);
	}

	virtual void on_size_change(typename _base_type::size_type& _size) noexcept override
	{
		_base_type::on_size_change(_size);
		update();
	}

	void update_vertex(g3d::device* _device)
	{
		if (mCommandHolder->vertex_buffer.empty() || mCommandHolder->vertex_buffer->count() != 4)
		{
			mCommandHolder->vertex_buffer		= new g3d::vertex_buffer<vertex>(4);
		}

		auto	pos1	= vec2<float>(0, 0);
		auto	pos2	= pos1 + vec2<float>(sprite_size());

		vertex	vtx[4] = 
		{
			{{pos1.x, pos1.y, 0, 1}, {0         , 0         }, this->get_color() },
			{{pos2.x, pos1.y, 0, 1}, {mUVScale.x, 0         }, this->get_color() },
			{{pos1.x, pos2.y, 0, 1}, {0         , mUVScale.y}, this->get_color() },
			{{pos2.x, pos2.y, 0, 1}, {mUVScale.x, mUVScale.y}, this->get_color() },
		};
		auto	tc	= this->texture()->coordinate();
		for (auto& v : vtx)
		{
			v.uv *= tc;
		}
		mCommandHolder->vertex_buffer->set_translate([v = std::vector<vertex>(std::begin(vtx), std::end(vtx))](auto _mem)
		{
			_mem->transfer(v.data());
		});
	}

protected:
	virtual size2<float> sprite_size() const noexcept
	{
		return	{ this->size().width, this->size().height };
	}

	bool										mUpdateVertex	= true;
	shader_type									mShader;
	ref<g3d::draw_command_holder<shader_type>>	mCommandHolder	= new g3d::draw_command_holder<shader_type>();
	vec2<float>									mUVScale		= {1, 1};
	ref<texture_type>							mTexture;
};

AMTRS_NAMESPACE_END
#endif

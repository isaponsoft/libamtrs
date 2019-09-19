#ifndef	__libamtrs__node__text_node__hpp
#define	__libamtrs__node__text_node__hpp
#include "node.hpp"
AMTRS_NAMESPACE_BEGIN


// ============================================================================
//! 長い文章を表示するためのノード。
// ----------------------------------------------------------------------------
//! 文章や、クレジット表記などを表示するのに向いている。
// ----------------------------------------------------------------------------
template<class NodeT>
class	text_node
		: public updatable_node<NodeT>
		, public g3d::texture_shader
{
	using	_base_type		= updatable_node<NodeT>;
	using	shader_type		= g3d::texture_shader;
public:
	enum mode : uint32_t
	{
		multiline	= 1<<0,			//!< 複数行サポート。
		linebreak	= 1<<1,			//!< ノードの大きさに合わせて折り返す。
		weblink		= 1<<2,			//!< WEBのリンクを有効にします。
		twitterlink	= 1<<3,			//!< Twitterのリンクを有効にします。
	};

	text_node() = default;
	text_node(font* _font)
		: mFont(_font)
	{}


	void set_text(std::string_view _text)
	{
		mText			= _text;
		mUpdateDirty	= true;
	}


	font* get_font()
	{
		if (!mFont)
		{
			set_font(font::create(nullptr, 30));
		}
		return	mFont;
	}


	void set_font(font* _font)
	{
		mFont			= _font;
		mUpdateDirty	= true;
	}


protected:
	virtual void on_size_change(typename _base_type::size_type& _size) noexcept override
	{
		_base_type::on_size_change(_size);
		mUpdateDirty	= true;
	}

	virtual void on_render(typename _base_type::render_queue& _queue) override
	{
		if (mUpdateDirty)
		{
			auto			f	= get_font();
			bitmap32		bmp(size2<int>((int)_base_type::size().width, (int)_base_type::size().height));
			f->draw_mb(bmp, mText, f->default_draw_info());
			if (!bmp.size().empty())
			{
				mCommandHolder->uniform.texture	= g3d::texture::create(std::move(bmp));
			}
			mCommandHolder->vertex_buffer.clear();
			mUpdateDirty	= false;
		}

		if (mCommandHolder->uniform.texture)
		{
			if (mCommandHolder->shader.empty())
			{
				_queue.transfer(this, [this](auto darg)
				{
					auto	device	= get_g3ddevice(darg);
					mCommandHolder->shader				= this->get_shader(device);
				});
				mCommandHolder->primitive			= g3d::primitive_type::triangle_strip;
			}

			if (mCommandHolder->vertex_buffer.empty())
			{
				mCommandHolder->vertex_buffer	= new g3d::vertex_buffer<vertex>(4);
				mCommandHolder->vertex_buffer->set_translate([this](auto* _mem)
				{
					auto	cs		= this->size();
					auto	pos1	= vec2<float>(0, 0);
					auto	pos2	= pos1 + vec2<float>(vec3<float>(cs));

					vertex	vtx[4] = 
					{
						{{pos1.x, pos1.y, 0, 1}, {0, 0}, {1,1,1,1} },
						{{pos2.x, pos1.y, 0, 1}, {1, 0}, {1,1,1,1} },
						{{pos1.x, pos2.y, 0, 1}, {0, 1}, {1,1,1,1} },
						{{pos2.x, pos2.y, 0, 1}, {1, 1}, {1,1,1,1} },
					};

					auto	tc	= mCommandHolder->uniform.texture->coordinate();
					for (auto& v : vtx)
					{
						v.uv *= tc;
					}
					_mem->transfer(vtx);
				});
			}

			mCommandHolder->uniform.color		= this->get_color();
			mCommandHolder->uniform.perspective	= _queue.projection;
			mCommandHolder->uniform.coordinate	= this->world_matrix();
			_queue.draw(mCommandHolder->make_draw_command());
		}

		_base_type::on_render(_queue);
	}



	size2<int>									mTextureSize	= { 128, 128 };	//!< このサイズ単位でテクスチャを生成します。
	ref<g3d::draw_command_holder<shader_type>>	mCommandHolder	= new g3d::draw_command_holder<shader_type>();
	ref<font>									mFont;
	std::string									mText;
	uint32_t									mMode			= mode::multiline | mode::linebreak | mode::weblink | mode::twitterlink;
	bool										mUpdateDirty	= false;
};


AMTRS_NAMESPACE_END
#endif

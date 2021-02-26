/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__text_node__hpp
#define	__libamtrs__node__text_node__hpp
#include "node.hpp"
#include "stringable_node.hpp"
#include "updatable_node.hpp"
#include "shader/opengles-sprite.hpp"
AMTRS_NAMESPACE_BEGIN


// ============================================================================
//! 長い文章を表示するためのノード。
// ----------------------------------------------------------------------------
//! 文章や、クレジット表記などを表示するのに向いている。
// ----------------------------------------------------------------------------
template<class NodeT>
class	text_node
		: public updatable_node<stringable_node<NodeT>>
{
public:
	using	super_type		= updatable_node<stringable_node<NodeT>>;
	using	shader_type		= opengles_sprite_shader;
	using	texture_type	= g3d::texture;
	using	vertex			= shader_type::vertex;
	using	uniform			= shader_type::uniform;

	enum mode : uint32_t
	{
		multiline		= 1<<0,		//!< 複数行サポート。
		linebreak		= 1<<1,		//!< ノードの大きさに合わせて折り返す。
		heightadjust	= 1<<2,		//!< ノードの高さを自動的に調整します。
	};

	using	super_type::super_type;

	//! 非同期更新を有効にします。
	//! 非同期化することで長い文字列を更新する際にブロックするのを防止します。
	void set_async_update(bool _async)
	{
		mUseAsync	= _async;
	}

	uint32_t get_mode() const noexcept { return mMode; }

	void set_mode(uint32_t _mode)
	{
		if (mMode != _mode)
		{
			mMode	= _mode;
			update_texture();
		}
	}

	//! この関数を呼び出すと横方向の大きさを自動調整するようになります。
	float set_width_adjust(float _max_width)
	{
		float	old	= mWidthAdjust;
		mWidthAdjust	= _max_width;
		return	old;
	}

	//! テ強制的にアップデートします。
	//! 通常、設定されたテキストはスケジューラによって後から更新されます。
	//! この関数を呼び出すとスケジューラを待たずに即座に更新を行います。
	void force_update()
	{
		if (!mUpdateTexture)
		{
			mUpdateTexture	= true;

			// テクスチャ生成中にパラメータが変わらないようにワークにコピー
			uint32_t		mode		= mMode;
			ref<font>		fnt			= super_type::get_font();
			size2<int>		texSize		= mTextureSize;
			size3<float>	bounds		= this->size();
			bool			widthAdjust	= false;
			if (mWidthAdjust > 0)
			{
				bounds.width	= mWidthAdjust;
				widthAdjust		= true;
			}


			mTextures		= {};
			create_textures(mode, super_type::get_string(), fnt, texSize, bounds, [&](auto tm)
			{
				mTextures.push_back(tm);
			});
			if (widthAdjust)
			{
				this->set_width(mTextures.back()->pos.x + mTextures.back()->siz.x);
			}
			if (mode & heightadjust)
			{
				if (mTextures.empty())
				{
					this->set_height(0);
				}
				else
				{
					this->set_height(mTextures.back()->pos.y + mTextures.back()->siz.y);
				}
			}
			super_type::rerender_request();
			mUpdateTexture	= false;
		}
	}

protected:
	virtual void on_size_change(typename super_type::size_type& _size) override
	{
		super_type::on_size_change(_size);
		update_texture();
	}

	virtual void on_stringable_state() override
	{
		super_type::on_stringable_state();
		update_texture();
	}


	virtual void on_render(typename super_type::renderer_argument& _queue) override
	{
		for (auto& tm : mTextures)
		{
			render(tm, _queue);
		}
		super_type::on_render(_queue);

	}


private:

	struct	texturemap : ref_object
	{
		ref<g3d::draw_command_holder<shader_type>>	commandHolder	= new g3d::draw_command_holder<shader_type>();
		ref<g3d::texture>							tex;
		vec2<float>									pos;
		vec2<float>									siz;
	};


	void update_texture()
	{
		if (mUpdateTexture)
		{
			return;
		}

		if (mUseAsync)
		{
			mUpdateTexture	= true;

			// テクスチャ生成中にパラメータが変わらないようにワークにコピー
			uint32_t		mode		= mMode;
			ref<font>		fnt			= super_type::get_font();
			size2<int>		texSize		= mTextureSize;
			size3<float>	bounds		= this->size();
			bool			widthAdjust	= false;
			if (mWidthAdjust > 0)
			{
				bounds.width	= mWidthAdjust;
				widthAdjust		= true;
			}

			this->async_schedule("textnode", [=]()
			{
				std::list<ref<texturemap>>	texs;
				create_textures(mode, super_type::get_string(), fnt, texSize, bounds, [&](auto tm)
				{
					texs.push_back(tm);
				});
				this->schedule_once([=]()
				{
					mTextures		= texs;
					mUpdateTexture	= false;
					if (widthAdjust)
					{
						this->set_width(mTextures.back()->pos.x + mTextures.back()->siz.x);
					}
					if (mode & heightadjust)
					{
						if (mTextures.empty())
						{
							this->set_height(0);
						}
						else
						{
							this->set_height(mTextures.back()->pos.y + mTextures.back()->siz.y);
						}
					}
					super_type::rerender_request();
				});
			});
		}
		else
		{
			mUpdateTexture	= true;
			this->schedule_once([=]()
			{
				// テクスチャ生成中にパラメータが変わらないようにワークにコピー
				uint32_t		mode		= mMode;
				ref<font>		fnt			= super_type::get_font();
				size2<int>		texSize		= mTextureSize;
				size3<float>	bounds		= this->size();
				bool			widthAdjust	= false;
				if (mWidthAdjust > 0)
				{
					bounds.width	= mWidthAdjust;
					widthAdjust		= true;
				}


				mTextures		= {};
				create_textures(mode, super_type::get_string(), fnt, texSize, bounds, [&](auto tm)
				{
					mTextures.push_back(tm);
				});
				if (widthAdjust)
				{
					this->set_width(mTextures.back()->pos.x + mTextures.back()->siz.x);
				}
				if (mode & heightadjust)
				{
					if (mTextures.empty())
					{
						this->set_height(0);
					}
					else
					{
						this->set_height(mTextures.back()->pos.y + mTextures.back()->siz.y);
					}
				}
				super_type::rerender_request();
				mUpdateTexture	= false;
			});
		}
	}


	static std::list<ref<texturemap>> create_textures(uint32_t _mode, std::string _txt, ref<font> _font, size2<int> _texsize, size3<float> _bounds, std::function<void(ref<texturemap>)> _callback)
	{
		std::list<ref<texturemap>>	maps;
		auto						text	= _txt;
		auto						bounds	= _bounds;
		auto						f		= _font;
		if (!text.empty() && bounds.width >= (float)f->size())
		{
			bitmap32		bmp;
			if (_mode & heightadjust)
			{
				font::draw_info	di	= f->default_draw_info();
				bmp	= f->create_bitmap_mb(text, di, (int)bounds.width);
			}
			else
			{
				if (bounds.height >= 1)
				{
					bmp	= bitmap32(size2<int>((int)bounds.width, (int)bounds.height));
					f->draw_mb(bmp, text, f->default_draw_info());
				}
			}
			if (!bmp.size().empty())
			{
				for (int y = 0; y < bmp.size().height; y += _texsize.height)
				{
					for (int x = 0; x < bmp.size().width; x += _texsize.width)
					{
						int			w	= std::min(x + _texsize.width , bmp.size().width ) - x;
						int			h	= std::min(y + _texsize.height, bmp.size().height) - y;
						bitmap32	sbmp(bmp.subimg({x, y, w, h}));

						ref<texturemap>	tm	= new texturemap();
						tm->tex		= g3d::texture::create(std::move(sbmp));
						tm->pos		= vec2<float>((float)x, (float)y);
						tm->siz		= vec2<float>((float)w, (float)h);
						_callback(tm);
					}
				}
			}
		}
		return	maps;
	}

	void render(ref<texturemap> _tm, typename super_type::renderer_argument& _queue)
	{
		if (_tm->commandHolder->shader.empty())
		{
			_tm->commandHolder->shader		= opengles_sprite_shader::create();
			_tm->commandHolder->primitive	= g3d::primitive_type::triangle_strip;
		}

		if (_tm->commandHolder->uniform.texture != _tm->tex)
		{

			_tm->commandHolder->uniform.texture	= _tm->tex;
			_tm->commandHolder->vertex_buffer	= new g3d::vertex_buffer<vertex>(4);

			auto	pos1	= _tm->pos;
			auto	pos2	= _tm->pos + _tm->siz;
			auto	tc		= _tm->commandHolder->uniform.texture->coordinate();
			_tm->commandHolder->vertex_buffer->set_translate([pos1, pos2, tc](auto* _mem)
			{
				vertex	vtx[4] = 
				{
					{{pos1.x, pos1.y, 0, 1}, {0, 0}, {1,1,1,1} },
					{{pos2.x, pos1.y, 0, 1}, {1, 0}, {1,1,1,1} },
					{{pos1.x, pos2.y, 0, 1}, {0, 1}, {1,1,1,1} },
					{{pos2.x, pos2.y, 0, 1}, {1, 1}, {1,1,1,1} },
				};

				for (auto& v : vtx)
				{
					v.uv *= tc;
				}
				_mem->transfer(vtx);
			});
		}
		if (_tm->commandHolder->shader && _tm->commandHolder->uniform.texture)
		{
			_tm->commandHolder->uniform.color		= this->get_color();
			_tm->commandHolder->uniform.perspective	= _queue.projection;
			_tm->commandHolder->uniform.coordinate	= this->world_matrix();
			_tm->commandHolder->uniform.coordinate.translate(_tm->commandHolder->uniform.coordinate.translate().round());
			_queue.draw(_tm->commandHolder->make_draw_command());
		}
	}


	bool										mUseAsync		= false;

	size2<int>									mTextureSize	= { 256, 256 };	//!< このサイズ単位でテクスチャを生成します。
	uint32_t									mMode			= mode::multiline | mode::linebreak | mode::heightadjust;
	bool										mUpdateTexture	= false;
	std::list<ref<texturemap>>					mTextures;

	float										mWidthAdjust	= -1;
};


AMTRS_NAMESPACE_END
#endif

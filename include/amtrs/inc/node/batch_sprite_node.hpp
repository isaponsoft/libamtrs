#if	0
#ifndef	__libamtrs__node__batch_sprite_node__hpp
#define	__libamtrs__node__batch_sprite_node__hpp
#include "node.hpp"
#include "sprite_node.hpp"
AMTRS_NAMESPACE_BEGIN
template<class NodeT>
class	batch_sprite_node
		: public texture_node<NodeT>
		, public g3d::texture_shader
{
	using	_base_type	= texture_node<NodeT>;
public:
	using	vertex		= texture_shader::vertex;
	using	uniform		= texture_shader::uniform;


	batch_sprite_node(size2<unsigned int> _divCount, g3d::texture* _texture)
		: _base_type(_texture, true)
		, mDivCount(_divCount)
	{}


	//! 
	template<class... Args>
	void set_chars(Args&&... _args)
	{
		mChars.clear();
		mChars.assign(std::forward<Args>(_args)...);
		build_vertex();
	}


	void set_chars(std::initializer_list<int> _chars)
	{
		mChars.clear();
		mChars.assign(_chars);
		build_vertex();
	}

	size2<unsigned int> char_size()
	{
		auto	tex		= this->texture();
		return	size2<unsigned int>(tex->size() / vec2<unsigned int>(mDivCount));
	}

	g3d::texture* texture(int _i)
	{
		return	mTextures.at(_i);
	}

	std::size_t texture_count() const noexcept { return mTextures.size(); }


	virtual void on_render(typename NodeT::render_status& _rs) override
	{
		if (!mVertex.empty())
		{
			update_textures();
			auto*	shader			= static_cast<texture_shader*>(this);

			uniform	un;
			un.perspective	= _rs.projection;
			un.coordinate	= NodeT::render_matrix();
			un.texture		= this->texture();
			_rs.device->set_uniform(&un);
			_rs.device->draw(g3d::device::triangles, mVertex.data(), mVertex.size());
		}
		NodeT::on_render(_rs);
	}

	void update_textures(bool _force = false)
	{
/*
		if (mModifiedTexture || _force)
		{
			mModifiedTexture	= false;

			auto	charSize	= size2<unsigned int>(this->size() / vec2<unsigned int>(mDivCount));
			auto	texCount	= mDivCount.width * mDivCount.height;
			mTextures.clear();
			mTextures.reserve(texCount);
			auto	pos			= vec2<unsigned int>(0, 0);
			for (unsigned int y = 0; y < mDivCount.height; ++y)
			{
				for (unsigned int x = 0; x < mDivCount.width; ++x)
				{
					mTextures.push_back(new g3d::sub_texture(tex, pos, charSize));
					pos.x += charSize.width;
				}
				pos.x	= 0;
				pos.y	+= charSize.height;
			}
		}
*/
	}

protected:

	void build_vertex()
	{
		update_textures();
		auto	pos		= vec2<float>(0, 0);
		mVertex.clear();
		mVertex.reserve(mChars.size() * 6);
		for (auto& c : mChars)
		{
			auto	st	= mTextures[c].get();
			auto	tc	= st->coordinate();
			auto	cs	= size2<float>(st->size() / 2);
			mVertex.push_back({{pos.x,            pos.y            , 0, 1}, vec2<float>{0, 0} * tc, this->get_color() });
			mVertex.push_back({{pos.x + cs.width, pos.y            , 0, 1}, vec2<float>{1, 0} * tc, this->get_color() });
			mVertex.push_back({{pos.x,            pos.y + cs.height, 0, 1}, vec2<float>{0, 1} * tc, this->get_color() });
			mVertex.push_back({{pos.x + cs.width, pos.y            , 0, 1}, vec2<float>{1, 0} * tc, this->get_color() });
			mVertex.push_back({{pos.x,            pos.y + cs.height, 0, 1}, vec2<float>{0, 1} * tc, this->get_color() });
			mVertex.push_back({{pos.x + cs.width, pos.y + cs.height, 0, 1}, vec2<float>{1, 1} * tc, this->get_color() });
			pos.x += cs.width ;
		}
	}


	bool							mModifiedTexture	= true;
	bool							mModifiedVertex		= true;
	size2<unsigned int>				mDivCount;
	std::vector<ref<g3d::texture>>	mTextures;
	std::vector<int>				mChars;
	std::vector<vertex>				mVertex;
};

AMTRS_NAMESPACE_END
#endif
#endif

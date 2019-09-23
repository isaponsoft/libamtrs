#ifndef	__libamtrs__node__string_node__hpp
#define	__libamtrs__node__string_node__hpp
#include "node.hpp"
AMTRS_NAMESPACE_BEGIN


// ============================================================================
//! 簡単なテキスト表示を行います。
// ----------------------------------------------------------------------------
//! ノードのサイズは常にテキストを描画するサイズと一致します。
// ----------------------------------------------------------------------------
template<class NodeT>
class	string_node
		: public sprite_node<NodeT>
{
	using	_base_type		= sprite_node<NodeT>;
public:
	string_node() = default;
	string_node(font* _font)
		: mFont(_font)
	{}


	string_node(std::string_view _text)
	{
		set_text(_text);
	}

	string_node(std::string_view _text, font* _font)
		: mFont(_font)
	{
		set_text(_text);
	}

	template<class Text>
	void set_text(Text&& _text)
	{
		if (mText != _text)
		{
			mText			= std::forward<Text>(_text);
			update();
		}
	}


	font* get_font()
	{
		return	mFont;
	}


	void set_font(font* _font)
	{
		if (mFont != _font)
		{
			mFont			= _font;
			update();
		}
	}

	void update()
	{
		if (mFont.empty() || mText.empty())
		{
			this->set_texture(nullptr);
		}
		else
		{
			on_update_texture(mText, mFont);
		}
		this->rerender_request();
		_base_type::update();
	}

protected:
	virtual void on_render(typename _base_type::render_queue& _queue) override
	{
		_base_type::on_render(_queue);
	}

	//! テクスチャを変更する際に呼ばれます。
	//! テクスチャをカスタマイズしたい場合はオーバーライドして、このクラスの代わりに
	//! テクスチャを生成して返してください。
	virtual void on_update_texture(const std::string& _text, font* _font)
	{
		bitmap<rgba<uint8_t>>	bmp	= get_font()->create_bitmap(mText);
		ref<g3d::texture>		tex	= g3d::texture::create(std::move(bmp));
		_base_type::set_texture(tex, true);
	}

private:
	std::string		mText;
	ref<font>		mFont;
};


AMTRS_NAMESPACE_END
#endif

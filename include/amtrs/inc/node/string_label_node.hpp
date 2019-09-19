#ifndef	__libamtrs__node__string_label_node__hpp
#define	__libamtrs__node__string_label_node__hpp
#include "string_node.hpp"

AMTRS_NAMESPACE_BEGIN
template<class BaseT>
class	string_label_node
		: public string_node<BaseT>
{
	using	_base_type	= string_node<BaseT>;

public:
	using	size_type	= typename _base_type::size_type;

	using	_base_type::_base_type;

	//! 自動改行を有効化します。
	void set_auto_linebreak(bool _enable) noexcept
	{
		if (mAutoLineBreak != _enable)
		{
			mAutoLineBreak	= _enable;
			_base_type::update();
		}
	}

	//! フォントサイズの自動調整を有効化します。
	void set_adjust_fontsize(bool _enable) noexcept
	{
		if (mAdjustFontSize != _enable)
		{
			mAdjustFontSize	= _enable;
			_base_type::update();
		}
	}

	//! サイズが変わったときに自動的に再描画を行うようにします。
	void set_auto_resize(bool _enable) noexcept
	{
		if (mAutoResize != _enable)
		{
			mAutoResize	= _enable;
			_base_type::update();
		}
	}

	template<class T>
	void set_text(T _text)
	{
		_base_type::set_text(std::forward<T>(_text));
	}

	void set_text(const std::string& _text, size_type _size)
	{
		set_text(_text, _size, _base_type::get_font());
	}

	void set_text(const std::string& _text, size_type _size, font* _font)
	{
		ref<g3d::texture>	retval;
		font::draw_info		di	= _font->default_draw_info();
		auto				bmp	= _font->create_bitmap_adjust_size(nullptr, _text, di, {(int)_size.width, (int)_size.height});
		if (bmp.empty())
		{
			_base_type::set_texture(nullptr, false);
		}
		else
		{
			auto				tex	= g3d::texture::create(std::move(bmp));
			_base_type::set_texture(tex);
		}
	}

protected:

	virtual void on_update_texture(const std::string& _text, font* _font) override
	{
		set_text(_text, this->size(), _font);
	}


	virtual void on_size_change(typename _base_type::size_type& _size) noexcept override
	{
		_base_type::on_size_change(_size);
		if (mAutoResize)
		{
			auto	tex	= _base_type::texture();
			if (tex)
			{
				auto	s1	= size2<int>(tex->size());
				auto	s2	= size2<int>((int)_size.width, (int)_size.height);
				if (s1 != s2)
				{
					_base_type::update();
				}
			}
			else
			{
				_base_type::update();
			}
		}
	}


	bool	mAutoLineBreak	= true;
	bool	mAdjustFontSize	= true;
	bool	mAutoResize		= true;
};
AMTRS_NAMESPACE_END

#endif

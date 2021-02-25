/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__component__color_component__hpp
#define	__libamtrs__node__component__color_component__hpp
AMTRS_NAMESPACE_BEGIN namespace component {

template<class NodeT>
class	color_component
{
public:
	using	color_type		= rgba<float>;

	color_type const& get_color() const noexcept
	{
		if (!is_enabled(cf_cached))
		{
			set_flags(cf_cached);
			mColorCache	= mColor;
			if (auto p = static_cast<NodeT const*>(this)->parent(); p)
			{
				auto	pc	= p->get_color();
				if (p->is_enabled(cf_cascade_color))
				{
					mColorCache.r	*= pc.r;
					mColorCache.g	*= pc.g;
					mColorCache.b	*= pc.b;
				}
				if (p->is_enabled(cf_cascade_alpha))
				{
					mColorCache.a	*= pc.a;
				}
			}
		}
		return	mColorCache;
	}

	void set_alpha(float _alpha)
	{
		if (mColor.a != _alpha)
		{
			mColor.a	= _alpha;
			clear_color_cache();
			changed();
		}
	}

	void set_color(color_type _color)
	{
		if (mColor != _color)
		{
			mColor	= _color;
			clear_color_cache();
			changed();
		}
	}

	//! アルファのカスケードを有効にする
	void set_cascade_alpha(bool _enable)
	{
		uint8_t	f	= mFlags;
		if (_enable)
		{
			set_flags(cf_cascade_alpha);
		}
		else
		{
			clear_flags(cf_cascade_alpha);
		}
		if (mFlags != f)
		{
			changed();
		}
	}

	//! 色のカスケードを有効にする
	void set_cascade_color(bool _enable)
	{
		uint8_t	f	= mFlags;
		if (_enable)
		{
			set_flags(cf_cascade_color);
		}
		else
		{
			clear_flags(cf_cascade_color);
		}
		if (mFlags != f)
		{
			changed();
		}
	}

protected:
	void clear_color_cache() { clear_flags(cf_cached); }

private:
	static constexpr uint8_t	cf_cascade_alpha	= 1<<0;		//!< 子へのカスケードを行う
	static constexpr uint8_t	cf_cascade_color	= 1<<1;		//!< 子へのカスケードを行う
	static constexpr uint8_t	cf_cached			= 1<<2;		//!< 色をキャッシュ済み

	bool is_enabled(uint8_t _flags) const noexcept { return mFlags & _flags; }
	void set_flags(uint8_t _flags) const noexcept { mFlags |= _flags; }
	void clear_flags(uint8_t _flags) const noexcept { mFlags &= ~_flags; }

	void changed()
	{
		auto*	thiz	= static_cast<NodeT*>(this);
		if (is_enabled(cf_cascade_alpha|cf_cascade_color))
		{
			thiz->visit_children([=](auto c)
			{
				c->clear_color_cache();
				return	false;
			}, false);
		}
		thiz->on_change_render_status();
		thiz->rerender_request();
	}

	color_type				mColor		= {1, 1, 1, 1};
	mutable color_type		mColorCache;
	mutable uint8_t			mFlags		= 0;
};


} AMTRS_NAMESPACE_END
#endif

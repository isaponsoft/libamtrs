/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__graphics__rect__hpp
#define	__libamtrs__graphics__rect__hpp
AMTRS_NAMESPACE_BEGIN

template<class T>
struct	rect
		: vec2<T>
		, size2<T>
{
	using	value_type		= T;
	using	position_type	= vec2<value_type>;
	using	size_type		= size2<value_type>;
	using	split_type		= std::tuple<rect, rect>;

	constexpr rect() = default;
	constexpr rect(const rect& _r) noexcept = default;
	constexpr rect& operator = (const rect& _r) noexcept = default;

	constexpr rect(value_type _x, value_type _y, value_type _width, value_type _height) noexcept
		: position_type(_x, _y), size_type(_width, _height)
	{}

	constexpr rect(position_type _pos, size_type _size) noexcept
		: position_type(_pos), size_type(_size)
	{}

	template<class R>
	constexpr explicit rect(const R& _r) noexcept
		: position_type(value_type(_r.x), value_type(_r.y)), size_type(value_type(_r.width), value_type(_r.height))
	{}


	bool empty() const noexcept { return size().empty(); }

	//! width や height が負の値の時、正の値になるように調整します。
	constexpr rect absolute() const noexcept
	{
		return
		{
			size_type::width  >= 0 ? position_type::x : position_type::x + size_type::width,
			size_type::height >= 0 ? position_type::y : position_type::y + size_type::height,
			size_type::width  >= 0 ? size_type::width  : -size_type::width,
			size_type::height >= 0 ? size_type::height : -size_type::height
		};
	}

	constexpr bool operator == (const rect& _r) const noexcept { return position() == _r.position() && size() == _r.size(); }
	constexpr bool operator != (const rect& _r) const noexcept { return !(*this == _r); }

	constexpr rect operator + (position_type _pos) const noexcept
	{
		return	{ position() + _pos, size() };
	}

	constexpr rect operator - (position_type _pos) const noexcept
	{
		return	{ position() - _pos, size() };
	}

	constexpr rect operator + (size_type _size) const noexcept
	{
		return	{ position(), size() + _size};
	}

	constexpr rect operator - (size_type _size) const noexcept
	{
		return	{ position(), size() - _size};
	}

	constexpr rect& operator += (position_type _pos) noexcept
	{
		set_position(position() + _pos);
		return	*this;
	}

	constexpr rect& operator -= (position_type _pos) noexcept
	{
		set_position(position() - _pos);
		return	*this;
	}

	constexpr rect& operator += (size_type _size) noexcept
	{
		set_size(size() + _size);
		return	*this;
	}

	constexpr rect& operator -= (size_type _size) noexcept
	{
		set_size(size() - _size);
		return	*this;
	}

	constexpr       position_type& position()       noexcept { return *this; }
	constexpr const position_type& position() const noexcept { return *this; }
	constexpr position_type set_position(position_type _pos) noexcept { static_cast<position_type&>(*this) = _pos; return position(); }
	constexpr       size_type& size()       noexcept { return *this; }
	constexpr const size_type& size() const noexcept { return *this; }
	constexpr size_type set_size(size_type _size) noexcept { static_cast<size_type&>(*this) = _size; return size(); }

	constexpr value_type left() const noexcept { return position_type::x; }
	constexpr value_type top() const noexcept { return position_type::y; }
	constexpr value_type right() const noexcept { return position_type::x + size_type::width; }
	constexpr value_type bottom() const noexcept { return position_type::y + size_type::height; }
	constexpr value_type right(value_type _r) noexcept { size_type::width = _r - position_type::x; return _r; }
	constexpr value_type bottom(value_type _b) noexcept { size_type::height = _b - position_type::y; return _b; }

	//! Left Top
	constexpr vec2<float> lt() const noexcept { return {position().x, position().y}; }

	//! Left Bottom
	constexpr vec2<float> lb() const noexcept { return {position().x, position().y + size().height}; }

	//! Right Top
	constexpr vec2<float> rt() const noexcept { return {position().x + size().width, position().y}; }

	//! Right Bottom
	constexpr vec2<float> rb() const noexcept { return {position().x + size().width, position().y + size().height}; }


	//! 指定する座標が内側にあるか調べます。
	constexpr bool is_inner(position_type _pos) const noexcept
	{
		return	(left() < _pos.x) && (_pos.x < right()) && (top() < _pos.y) && (_pos.y < bottom());
	}

	//! 指定する矩形が同じか内側にあるか調べます。
	constexpr bool is_inner(const rect& _in) const noexcept
	{
		return	(left() <= _in.left()) && (top() <= _in.top()) && (right() >= _in.right()) && (bottom() >= _in.bottom());
	}

	//! 指定する矩形が衝突しているか調べます。
	constexpr bool is_collision(const rect& _other) const noexcept
	{
		return	((_other.x >= this->x && _other.x < this->right() ) || (this->x >= _other.x && this->x < _other.right() ))
			&&	((_other.y >= this->y && _other.y < this->bottom()) || (this->y >= _other.y && this->y < _other.bottom()));
	}

	//! 二つの矩形が含まれる矩形を生成します。
	constexpr rect concat(const rect& _b) const noexcept
	{
		auto	a		= absolute();
		auto	b		= _b.absolute();
		float	x		= a.left() < b.left() ? a.left() : b.left();
		float	y		= a.top()  < b.top()  ? a.top()  : b.top();
		float	right	= a.right()  > b.right()  ? a.right()  : b.right();
		float	bottom	= a.bottom() > b.bottom() ? a.bottom() : b.bottom();
		return	{ x, y, right - x, bottom - y };
	}

	//! 指定位置を中心に拡大・縮小します。
	constexpr rect scale(position_type _pivot, position_type _scale) const noexcept
	{
		return	{ (position() - _pivot) * _scale + position(), size() * _scale };
	}

	//! 上下左右をトリミングした矩形を作成します。
	//! 負の値を指定すると余白を持たせるのと同義になります。
	constexpr rect trim(value_type _left, value_type _top, value_type _right, value_type _bottom) const noexcept
	{
		return	{ position() + position_type(_left, _top), size() - size_type(_left, _top) - size_type(_right, _bottom) };
	}

	//! 上下左右に指定した大きさのマージンを持たせた矩形を作成します。
	constexpr rect margin(value_type _left, value_type _top, value_type _right, value_type _bottom) const noexcept
	{
		return	trim(-_left, -_top, -_right, -_bottom);
	}

	//! 左から指定した縦幅の矩形を作成します。
	constexpr rect subrect_from_left(value_type _width) const noexcept
	{
		return	{position_type::x, position_type::y, size_type::width - _width, size_type::height};
	}

	//! 上から指定した縦幅の矩形を作成します。
	constexpr rect subrect_from_top(value_type _height) const noexcept
	{
		return	{position_type::x, position_type::y, size_type::width, size_type::height - _height};
	}

	//! 右から指定した縦幅の矩形を作成します。
	constexpr rect subrect_from_right(value_type _width) const noexcept
	{
		return	{position_type::x + _width, position_type::y, _width, size_type::height};
	}

	//! 下から指定した縦幅の矩形を作成します。
	constexpr rect subrect_from_bottom(value_type _height) const noexcept
	{
		return	{position_type::x, position_type::y + _height, size_type::width, _height};
	}

	//! 左から指定した縦幅の矩形を作成し、自身の大きさを減らします。
	constexpr rect allocate_from_left(value_type _width) noexcept
	{
		auto	retval	= subrect_from_left();
		position_type::x	+= _width;
		size_type::width	-= _width;
		return	retval;
	}

	//! 上から指定した縦幅の矩形を作成し、自身の大きさを減らします。
	constexpr rect allocate_from_top(value_type _height) noexcept
	{
		auto	retval	= subrect_from_top();
		position_type::y	+= _height;
		size_type::height	-= _height;
		return	retval;
	}

	//! 右から指定した縦幅の矩形を作成し、自身の大きさを減らします。
	constexpr rect allocate_from_right(value_type _width) noexcept
	{
		auto	retval	= subrect_from_right();
		size_type::width	-= _width;
		return	retval;
	}

	//! 下から指定した縦幅の矩形を作成し、自身の大きさを減らします。
	constexpr rect allocate_from_bottom(value_type _height) noexcept
	{
		auto	retval	= subrect_from_bottom();
		size_type::height	-= _height;
		return	retval;
	}

	//! 左から指定した縦幅の位置で分割し、二つの矩形を生成します。
	constexpr split_type split_from_left(value_type _width) const noexcept
	{
		return
		{
			{position_type::x         , position_type::y, _width                   , size_type::height},
			{position_type::x + _width, position_type::y, size_type::width - _width, size_type::height}
		};
	}

	//! 上から指定した縦幅の位置で分割し、二つの矩形を生成します。
	constexpr split_type split_from_top(value_type _height) const noexcept
	{
		return
		{
			{position_type::x, position_type::y          , size_type::width, _height                    },
			{position_type::x, position_type::y + _height, size_type::width, size_type::height - _height}
		};
	}


	//! 指定するサイズの矩形を取り出し、３つの領域を生成します。
	constexpr std::tuple<rect, rect, rect> split_from_topleft(const size_type& _allocate_size) const noexcept
	{
		if (_allocate_size.height < _allocate_size.width)
		{
			// ┏━━━━┳━━━┓
			// ┃  use   ┃  f2  ┃
			// ┣━━━━┫      ┃
			// ┃   f1   ┃　　　┃
			// ┗━━━━┻━━━┛
			auto	[ tmp, f2 ]	= split_from_left(_allocate_size.width);
			auto	[ use, f1 ]	= tmp.split_from_top(_allocate_size.height);
			return	{ use, f1, f2 };
		}
		// ┏━━━┳━━━━┓
		// ┃ use  ┃   f1   ┃
		// ┣━━━┻━━━━┫
		// ┃       f2       ┃
		// ┗━━━━━━━━┛
		auto	[ tmp, f2 ]	= split_from_top(_allocate_size.height);
		auto	[ use, f1 ]	= tmp.split_from_left(_allocate_size.width);
		return	{ use, f1,  f2 };
	}
};

AMTRS_NAMESPACE_END
#endif

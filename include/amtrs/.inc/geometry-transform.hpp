/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__graphics__transform__hpp
#define	__libamtrs__graphics__transform__hpp
AMTRS_NAMESPACE_BEGIN


// ============================================================================
//! 変形情報を保持します。
// ----------------------------------------------------------------------------
//! このクラスは親子関係の管理は行いません。親子関係が必要な場合は node などを
//! 使用してください。
// ----------------------------------------------------------------------------
template<class T>
class	transform
{
public:
	using	value_type		= float;

	using	position_type	= vec3<value_type>;
	using	scale_type		= vec3<value_type>;
	using	rotate_type		= quaternion<value_type>;
	using	size_type		= size3<value_type>;
	using	matrix_type		= mat44<value_type>;
	using	box_type		= box<value_type>;


	auto pivot() const noexcept -> const scale_type& { return mPivot; }
	bool set_pivot(scale_type _position) noexcept { if (mPivot != _position) {mPivot = _position; update_transform(); return true; } return false; }

	auto position() const noexcept -> position_type     { return mPosition; }
	bool set_position(position_type _position) noexcept { if (mPosition   != _position) { mPosition   = _position; update_transform(); return true; } return false; }
	bool set_position_x(value_type _position) noexcept  { if (mPosition.x != _position) { mPosition.x = _position; update_transform(); return true; } return false; }
	bool set_position_y(value_type _position) noexcept  { if (mPosition.y != _position) { mPosition.y = _position; update_transform(); return true; } return false; }
	bool set_position_z(value_type _position) noexcept  { if (mPosition.z != _position) { mPosition.z = _position; update_transform(); return true; } return false; }


	auto scale() const noexcept -> const scale_type& { return mScale; }
	bool set_scale(scale_type _scale) noexcept { if (mScale != _scale) { mScale = _scale; on_size_change(mSize); update_transform(); return true; } return false; }

	auto rotate() const noexcept -> const rotate_type& { return mRotate; }
	bool set_rotate(rotate_type _rotate) noexcept { if (mRotate != _rotate) { mRotate = _rotate; update_transform(); return true; } return false; }

	auto size() const noexcept -> size_type { return mSize; }
	bool set_size(size_type _size) noexcept;
	bool set_width(value_type _width) noexcept;
	bool set_height(value_type _height) noexcept;

	auto content_size() const noexcept -> box_type { return { position(), size() }; }
	void set_content_size(box_type _box) noexcept { mPosition = _box.position(); mSize = _box.size(); update_transform(); }

	auto scaled_size() const noexcept -> size_type { return mSize * mScale; }

	// ========================================================================
	//! 自身のトランスフォーム情報をマトリックスに変換します。
	// ------------------------------------------------------------------------
	auto to_matrix() const noexcept -> const matrix_type&;


protected:
	virtual void on_coordinate_modify()
	{
		mModified = true;
	}

	virtual void on_size_change(size_type& _size)
	{
		if (mSize != _size)
		{
			mSize = _size;
			update_transform();
		}
	}

private:
	void update_transform()
	{
		on_coordinate_modify();
	}

	scale_type				mPivot			= {0,0,0};
	position_type			mPosition		= {0,0,0};
	scale_type				mScale			= {1,1,1};
	rotate_type				mRotate			= rotate_type::identify();
	size_type				mSize			= {0,0,0};

	mutable bool			mModified		= false;
	mutable matrix_type		mMatrixCache	= matrix_type::identify();
};


template<class T>
auto transform<T>::to_matrix() const noexcept -> const matrix_type&
{
	if (mModified)
	{
		mModified		= false;
		mMatrixCache	= matrix_type::translate(-vec3<float>(size() * pivot()))
						* matrix_type::scale(scale().x, scale().y, scale().z, 1)
						* matrix_type(rotate().to_matrix())
						* matrix_type::translate(position().x, position().y, position().z);
	}
	return	mMatrixCache;
}


template<class T>
bool transform<T>::set_size(size_type _size) noexcept
{
	if (mSize == _size)
	{
		return	false;
	}
	on_size_change(_size);
	return	true;
}


template<class T>
bool transform<T>::set_width(value_type _width) noexcept
{
	size_type	s	= size();
	s.width	= _width;
	return	set_size(s);
}


template<class T>
bool transform<T>::set_height(value_type _height) noexcept
{
	size_type	s	= size();
	s.height	= _height;
	bool	isChanged	= set_size(s);
	return	isChanged;
}



AMTRS_NAMESPACE_END
#endif

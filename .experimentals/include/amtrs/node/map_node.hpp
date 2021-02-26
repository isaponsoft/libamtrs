/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__map_node__hpp
#define	__libamtrs__node__map_node__hpp
#include "node.hpp"
#include "scroll_node.hpp"
AMTRS_NAMESPACE_BEGIN



// ============================================================================
//! ノードに対してスクロール機能を持たせます。
// ----------------------------------------------------------------------------
//! 
// ----------------------------------------------------------------------------
template<class NodeT>
class	map_node
		: public updatable_node<NodeT>
{
	using	_base_type		= updatable_node<NodeT>;
public:
	using	node_type		= typename NodeT::node_type;
	using	matrix_type		= typename NodeT::matrix_type;
	using	position_type	= typename NodeT::position_type;

	using	_base_type::_base_type;


	//! マップビューの状態をリセットします。
	void reset_scroll() { mScrollMatrix = mat44<float>::identify(); mTranslate = {0,0,0}; mAngle = 0; mScale = 1; on_coordinate_modify(); }

	//! マップビュー内の位置を指定します。
	void set_scroll_position(vec3<float> _pos) noexcept { mTranslate = _pos; on_coordinate_modify(); }

	//! マップビュー内の角度を指定します。
	void set_scroll_angle(float _angle) noexcept { mAngle = _angle; on_coordinate_modify(); }

	//! マップビュー内のスケールを指定します。
	void set_scroll_scale(float _scale) noexcept { mScale = _scale; on_coordinate_modify(); }


	//! デッドゾーンサイズを取得します。
	constexpr float deadzone_size() const noexcept
	{
		return	15.0f;	// MEMO: そのうちDPIをベースに決定する
	}

	bool is_angle_enable() const noexcept { return mAngleEnable; }
	void set_angle_enable(bool _enable) noexcept { mAngleEnable = _enable; } 
	bool is_scale_enable() const noexcept { return mScaleEnable; }
	void set_scale_enable(bool _enable) noexcept { mScaleEnable = _enable; } 


	virtual node_type* on_input(typename _base_type::pointer_event* _e) override
	{
		auto	mat	= _base_type::world_matrix().inverse();

		// --------------------------------------------------------------------
		// ホイールイベント
		// --------------------------------------------------------------------
		if (auto wheel = _e->event()->get_mouse_wheel_event(); wheel)
		{
			// 先に上のノードから処理する。
			if (auto r = _base_type::on_input(_e); r)
			{
				return	r;
			}

			vec2<float>		pos	= vec2<float>(vec3<float>(wheel->template get_point<float>(), 0) * mat);
			if (_base_type::size().is_inner(pos))
			{
				if (wheel->axis == mouse_wheel_event::axis_y)
				{
					float			scale	= wheel->delta;
					if (scale < 0)
					{
						scale	= -scale / 1.2f;
					}
					else
					{
						scale	=  scale * 1.2f;
					}
					mScrollMatrix	= mScrollMatrix
									* mat44<float>::translate(vec3<float>(-pos, 0))
									* mat44<float>::scale({scale, scale, scale, 1})
									* mat44<float>::translate(vec3<float>(pos, 0));
					mScale			= 1;
					on_coordinate_modify();
					return	this;
				}
			}
			return	nullptr;
		}


		// --------------------------------------------------------------------
		// ポインターイベント
		// --------------------------------------------------------------------
		if (auto mouse = _e->event()->get_mouse_event(); mouse)
		{
			// 離された場合は、上のノードの有無にかかわらず状態を終了する。
			if (mouse->state == input_event::release)
			{
				mButtonMask		&= ~mouse->buttons;
				if (mMode != mode::none && mMode != mode::pressing)
				{
					// 位置を確定する
					mScrollMatrix	= mScrollMatrix
									* mat44<float>::translate(vec3<float>(-mBeginPos1, 0))
									* mat44<float>(mat33<float>::rotate_z(mAngle))
									* mat44<float>::scale({mScale, mScale, mScale, 1})
									* mat44<float>::translate(mTranslate + vec3<float>(mBeginPos1));
					mScale			= 1;
					mAngle			= 0;
					mTranslate		= {0, 0, 0};
					if (mButtonMask)
					{
						mMode		=  mode::scrolling;
						if (mButtonMask & 0x01)
						{
							mBeginPos1	= mLastPos[0];
							mBeginPos2	= mLastPos[0];
						}
						if (mButtonMask & 0x02)
						{
							mBeginPos1	= mLastPos[1];
							mBeginPos2	= mLastPos[1];
						}
					}
					else
					{
						mMode			=  mode::none;
					}
					on_coordinate_modify();

					// スクロール処理中だったので、上のノードには伝えない
					return	0;
				}
				mMode			=  mode::none;
			}

			// 先に上のノードから処理する。
			if (auto r = _base_type::on_input(_e); r)
			{
				return	r;
			}

			vec2<float>		pos	= vec2<float>(vec3<float>(mouse->template get_point<float>(), 0) * mat);
			if (mouse->buttons & 0x03)
			{
				if (mouse->buttons & 0x01)
				{
					mLastPos[0]	= pos;
				}
				if (mouse->buttons & 0x02)
				{
					mLastPos[1]	= pos;
				}

				if (mouse->state == input_event::press)
				{
					if (!_base_type::size().is_inner(pos))
					{
						return	_base_type::on_input(_e);
					}
					mButtonMask	|= mouse->buttons;
					if ((mButtonMask&3) != 3)
					{
						mMode		= mode::pressing;
						mBeginPos1	= pos;
						mBeginPos2	= pos;
					}
					else
					{
						mMode			= mode::pinhing;
						if (mouse->buttons == 1<<0)
						{
							mBeginPos1		= pos;
						}
						else if (mouse->buttons == 1<<1)
						{
							mBeginPos2		= pos;
						}
						mBeginLength	= (mLastPos[0] - mLastPos[1]).length();
						mBeginAngle		= (mLastPos[0] - mLastPos[1]).atan2();
					}
				}


				if (mButtonMask)
				{
					if (mode::pressing && mouse->state == input_event::drag)
					{
						if (mMode == mode::pinhing)
						{
							if (mouse->buttons == 1)
							{
								mTranslate	= vec3<float>(pos - mBeginPos1, 0);
							}
							if (mScaleEnable)
							{
								mScale	= (mLastPos[0] - mLastPos[1]).length() / mBeginLength;
							}
							if (mAngleEnable)
							{
								mAngle	= mBeginAngle - (mLastPos[0] - mLastPos[1]).atan2();
							}
							on_coordinate_modify();
						}
						else if (mMode == mode::scrolling)
						{
							mTranslate	= vec3<float>(pos - mBeginPos1, 0);
							on_coordinate_modify();
						}
						else if ((pos - mBeginPos1).length() >= deadzone_size())
						{
							mMode		= mode::scrolling;
						}
					}
				}
			}

			// スクロールかピンチを行っているなら自身でイベントをキャプチャする。
			if (mMode == mode::scrolling || mMode == mode::pinhing)
			{
				return	this;
			}
		}

		// --------------------------------------------------------------------
		// 自分には関係のないイベント
		// --------------------------------------------------------------------
		return	_base_type::on_input(_e);
	}

	void update_range()
	{
		mUpdateStatus	= true;
	}

	virtual void on_update(float _delta) override
	{
		_base_type::on_update(_delta);
		if (mUpdateStatus)
		{
			mUpdateStatus	= false;

			// 子の大きさに合わせてスクロール幅を調整する
			rect<float>	range	= {0, 0, 0, 0};
			_base_type::visit_children([&](auto* child) -> bool
			{
				range	= range.concat(rect<float>(child->content_size()));
				return	0;
			});
			mScrollRange	= range;
		}
	}

	virtual const matrix_type& world_matrix() const noexcept override
	{
		if (mModifiedMatrix)
		{
			mModifiedMatrix	= false;
			mMatrix = _base_type::world_matrix()
					* mScrollMatrix
					* mat44<float>::translate(vec3<float>(-mBeginPos1, 0))
					* mat44<float>(mat33<float>::rotate_z(mAngle))
					* mat44<float>::scale({mScale, mScale, mScale, 1})
					* mat44<float>::translate(mTranslate + vec3<float>(mBeginPos1))
			;
		}
		return	mMatrix;
	}

protected:
	virtual void on_coordinate_modify() noexcept override
	{
		_base_type::on_coordinate_modify();
		mModifiedMatrix	= true;
	}


	enum mode
	{
		none,
		pressing,		//!< 押したが、まだ何もしていない
		scrolling,		//!< スクロール
		pinhing,		//!< ピンチ
	};

	bool				mScaleEnable	= true;
	bool				mAngleEnable	= true;

	mode				mMode		= mode::none;
	uint32_t			mButtonMask	= 0;
	vec2<float>			mBeginPos1;
	vec2<float>			mBeginPos2;

	vec2<float>			mLastPos[2];
	float				mBeginLength;
	float				mBeginAngle;

	vec3<float>			mTranslate		= {0, 0, 0};
	float				mScale			= 1;
	float				mAngle			= 0;

	bool				mUpdateStatus	= false;
	rect<float>			mScrollRange	= {0, 0, 0, 0};
	mat44<float>		mScrollMatrix	= mat44<float>::identify();
	mutable bool		mModifiedMatrix	= true;
	mutable matrix_type	mMatrix;
};


AMTRS_NAMESPACE_END
#endif

/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__amtrs_utils__ui_utils__scroll__hpp
#define	__libamtrs__amtrs_utils__ui_utils__scroll__hpp
#include "def.hpp"
AMTRS_UIUTILS_NAMESPACE_BEGIN


// ============================================================================
//! ボタンとしての機能を持たせるためのコンポーネント
// ----------------------------------------------------------------------------
struct	scroll
{
public:
	static constexpr double	moment_speed	= 0.98f;

	enum scroll_axis
	{
		axis_x		= 1<<0,
		axis_y		= 1<<1,
	};

	constexpr scroll(uint32_t _axis_mask = axis_x | axis_y)
		: mAxis(_axis_mask)
	{}

	void set_multi_scroll(bool _enable)
	{
		mMultiScroll	= _enable;
	}

	//! マウスキャプチャが必要な状態なら true
	constexpr bool is_capturing() const noexcept
	{
		return	mCapturing;
	}

	//! マージンサイズを取得します。
	constexpr float margin_size() const noexcept
	{
		return	80.0f;	// MEMO: そのうちDPIをベースに決定する
	}

	void set_scroll(vec2<float> _pos)
	{
		mScroll		= _pos;
		mPrevious	= vec2<int>(_pos);
		mScrollVec	= { 0, 0 };
	}

	//! ボタンが離された処理を行う
	void release()
	{
		this->mPressed		= false;
		this->mCapturing	= false;
		this->mAxisLock		= 0;
	}

	template<class NodeT>
	void cancel(NodeT& _node) noexcept
	{
		release();
		mScrollMoment		= {0, 0};
	}


	template<class NodeT>
	constexpr void input(NodeT& _node, input_event* _e, vec2<float>* _pos = nullptr) noexcept
	{
		vec2<float>	pos		= _pos
							? *_pos
							: vec2<float>(vec3<float>(_e->get_point<float>(), 0) * _node.world_matrix().inverse());
		auto*		event	= _e->get_mouse_event();
		if (event)
		{
			if (!this->mPressed)
			{
				if ((event->buttons & mouse_event::left) && event->state == input_event::press)
				{
					if (_node.size().is_inner(pos))
					{
						// 押された
						this->mPressed		= true;
						this->mCapturing	= false;
						mBegin				= 
						mPrevious			= event->get_point<int>();
						mScrollVec			= {0, 0};
						mScrollMoment		= {0, 0};
						return;
					}
				}
			}
			else
			{
				if ((event->buttons & mouse_event::left) && event->state == input_event::release)
				{
					release();
				}
				else if ((event->buttons & mouse_event::left) && event->state == input_event::drag)
				{
					auto	pos	= event->get_point<int>();
					if (!this->mCapturing)
					{
						if ((mAxis & axis_x) && (std::abs(pos.x - (float)mBegin.x) >= (margin_size() * 0.7f)))
						{
							this->mCapturing	= true;
						}
						if ((mAxis & axis_y) && (std::abs(pos.y - (float)mBegin.y) >= margin_size()))
						{
							this->mCapturing	= true;
						}
						if (!this->mCapturing)
						{
							mScrollMoment	= vec2<float>(event->get_point<int>()) - vec2<float>(mPrevious);
							mPrevious		= event->get_point<int>();
							return;
						}
					}

					auto	v	= vec2<float>(pos - mPrevious);

					if (mMultiScroll)
					{
						if (!(mAxis & axis_x))
						{
							v.x	= 0;
						}
						if (!(mAxis & axis_y))
						{
							v.y	= 0;
						}
						axis_filter(v);
						mScroll 		+= v;
						mScrollVec		+= v;
					}
					else
					{
						if (this->mAxisLock)
						{
							if (!(mAxis & axis_x) || (this->mAxisLock == axis_y))
							{
								v.x	= 0;
							}
							if (!(mAxis & axis_y) || (this->mAxisLock == axis_x))
							{
								v.y	= 0;
							}
						}
						axis_filter(v);
						mScroll 		+= v;
						mScrollVec		+= v;
						if (!this->mAxisLock)
						{
							if (std::abs(mScrollVec.x) > 16 || std::abs(mScrollVec.y) > 16)
							{
								if (std::abs(mScrollVec.x) > std::abs(mScrollVec.y))
								{
									this->mAxisLock	= axis_x;
								}
								else
								{
									this->mAxisLock	= axis_y;
								}
							}
						}
					}
					mScrollMoment	= vec2<float>(event->get_point<int>()) - vec2<float>(mPrevious);
					mPrevious		= event->get_point<int>();
				}
			}
		}
		axis_filter(mScroll);
		axis_filter(mScrollVec);
	}


	//! スクロールの定期更新を行います。
	bool update(float _delta)
	{
		if (!mPressed)
		{
			mScroll.x	+= mScrollMoment.x;	mScrollMoment.x = (float)((float)mScrollMoment.x * moment_speed * _delta / (1.0f/60.0f));
			mScroll.y	+= mScrollMoment.y;	mScrollMoment.y = (float)((float)mScrollMoment.y * moment_speed * _delta / (1.0f/60.0f));
			if (std::abs(mScrollMoment.x) < 1) { mScrollMoment.x = 0; }
			if (std::abs(mScrollMoment.y) < 1) { mScrollMoment.y = 0; }
		}
		if (mRangeEnable && !mCapturing)
		{
			//float	speed	= mOverfllowSpeed * _delta * 60.0f;
			for (int i = 0; i < mScrollRange.col_size; ++i)
			{
				auto	out	= (int)mScrollRange[i].out_of(mScroll[i]);
				if (out < 0)
				{
					mScroll[i]			+= (mScrollRange[i].minimam - mScroll[i]) * 0.5f;
					mScrollMoment[i]	=  0;
				}
				if (out > 0)
				{
					mScroll[i]			+= (mScrollRange[i].maximam - mScroll[i]) * 0.5f;
					mScrollMoment[i]	=  0;
				}
				axis_filter(mScroll);
			}
		}
		if (mScrollPrev != mScroll)
		{
			mScrollPrev	= mScroll;
			return	true;
		}

		return	false;
	}

	//! 範囲外にはみ出しているかどうか
	bool is_over() const noexcept
	{
		for (int i = 0; i < mScrollRange.col_size; ++i)
		{
			auto	out	= (int)mScrollRange[i].out_of(mScroll[i]);
			if (out < 0)
			{
				return	true;
			}
			if (out > 0)
			{
				return	true;
			}
		}
		return	false;
	}


	vec2<float> position() const noexcept
	{
		return	mScroll;
	}

	const vec2<float>& set_position(vec2<float> _pos) noexcept
	{
		mScroll	= _pos;
		axis_filter(mScroll);
		return	mScroll;
	}

	const range<float>& range_x() const noexcept { return mScrollRange.x; }
	const range<float>& range_y() const noexcept { return mScrollRange.y; }

	void set_range_x(range<float> _x) noexcept
	{
		mScrollRange.x = _x;
	}

	void set_range_y(range<float> _y) noexcept
	{
		mScrollRange.y = _y;
	}

private:


	vec2<float>& axis_filter(vec2<float>& _pos)
	{
		if (this->mAxisLock)
		{
			if (!(mAxis & axis_x) || (this->mAxisLock == axis_y))
			{
				_pos.x	= 0;
			}
			if (!(mAxis & axis_y) || (this->mAxisLock == axis_x))
			{
				_pos.y	= 0;
			}
		}
		else
		{
			if (mAxis & axis_x)
			{
				_pos.y	= 0;
			}
			if (mAxis & axis_y)
			{
				_pos.x	= 0;
			}
		}
		_pos.x	= (float)((std::intmax_t)(_pos.x));
		_pos.y	= (float)((std::intmax_t)(_pos.y));
		return	_pos;
	}

	uint32_t			mAxis			= 0;
	uint32_t			mAxisLock		= 0;
	vec2<int>			mBegin			= {0, 0};		//!< 謳歌された場所
	vec2<int>			mPrevious		= {0, 0};
	vec2<float>			mScroll			= {0, 0};
	vec2<float>			mScrollPrev		= {0, 0};		//!< update() で使用される。前回のフレームの時の値。

	bool				mPressed		= false;		//!< 押された状態なら true
	bool				mCapturing		= false;		//!< イベントキャプチャ中なら true

	//float				mOverfllowSpeed	= 1.0f;			//!< オーバーフロー時のスクロール速度(１秒でどのくらい減衰するかを示すα値)

	bool				mMultiScroll	= false;		//!< true の時はXY同時にスクロールできる


	bool				mRangeEnable	= true;			//!< true の時、mScrollMin, mScrollMax によるリミット処理を行う。
	vec2<range<float>>	mScrollRange	= {{0, 0}, {0,0}};
	vec2<float>			mScrollVec		= {0, 0};		//!< スクロール方向を決定するためのテンポラリ
	vec2<float>			mScrollMoment	= {0, 0};		//!< スクロールの勢い
};


AMTRS_UIUTILS_NAMESPACE_END
#endif

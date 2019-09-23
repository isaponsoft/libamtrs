/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__graphics__waveform__hpp
#define	__libamtrs__graphics__waveform__hpp
#include <cmath>
#include "def.hpp"
// ****************************************************************************
// 波形を生成するためのユーティリティ
// ----------------------------------------------------------------------------
AMTRS_NAMESPACE_BEGIN namespace waveform {



// ============================================================================
//! 線形変形
// ----------------------------------------------------------------------------
template<class T>
struct	linear
{
	constexpr T operator () (T _alpha)
	{
		return	_alpha;
	}
};


// ============================================================================
//! 反転した波形
// ----------------------------------------------------------------------------
template<class T>
struct	reverse
{
	constexpr T operator () (T _alpha)
	{
		return	1 - _alpha;
	}
};


// ============================================================================
//! のこぎり波
// ----------------------------------------------------------------------------
//! コンストラクタの引数で周期を指定できます。
// ----------------------------------------------------------------------------
template<class T>
struct	sawtooth
{
	sawtooth(T _cycle = 2)
		: mCycle(_cycle)
	{}

	constexpr T operator () (T _alpha)
	{
		_alpha	= _alpha * mCycle;
		return	_alpha - floor(_alpha);
	}

	const T	mCycle;
};


// ============================================================================
//! 正弦波
// ----------------------------------------------------------------------------
//! コンストラクタの引数で周期を指定できます。
// ----------------------------------------------------------------------------
template<class T>
struct	sine
{
	sine(T _cycle = 2)
		: mCycle(_cycle)
	{}

	constexpr T operator () (T _alpha)
	{
		return	sin(_alpha * mCycle);
	}

	const T	mCycle;
};



// ============================================================================
//! 三角派
// ----------------------------------------------------------------------------
//! コンストラクタの引数で周期を指定できます。
// ----------------------------------------------------------------------------
template<class T>
struct	triangle_in
{
	triangle_in(T _cycle = 1)
		: mCycle(_cycle * pi * 2)
	{}

	constexpr T operator () (T _alpha)
	{
		auto	x	= _alpha;
		return	acos(sin((0.25 * pi * 2) + (x * mCycle))) / (pi * 2);
	}

	const T	mCycle;
};




// ============================================================================
//! パスフィルター
// ----------------------------------------------------------------------------
template<class T, class Filter, class Source>
struct	pass_filter
{
	constexpr pass_filter(Filter _filter = {}, Source _source = {})
		: mFilter(_filter)
		, mSource(_source)
	{}

	constexpr T operator () (T _x)
	{
		return	mFilter(mSource(_x));
	}

	Filter	mFilter;
	Source	mSource;
};





// ****************************************************************************
// 0.0 ～ 1.0 のアルファ値に対して補正を行う関数群
// ----------------------------------------------------------------------------
// cocos2d-x 互換の波形関数群
// ----------------------------------------------------------------------------



// ============================================================================
//! 開始時に勢いをつけて
// ----------------------------------------------------------------------------
template<class T>
struct	back_ease_in
{
	back_ease_in(T _overshoot = 1.70158)
		: mOvershoot(_overshoot)
	{}

	constexpr T operator () (T _alpha)
	{
		return	_alpha * _alpha * ((mOvershoot + 1) * _alpha - mOvershoot);
	}

	T	mOvershoot;
};


// ============================================================================
//! 終了時に勢いをつける
// ----------------------------------------------------------------------------
template<class T>
struct	back_ease_out
{
	back_ease_out(T _overshoot = 1.70158)
		: mOvershoot(_overshoot)
	{}

	constexpr T operator () (T _alpha)
	{
		_alpha	= _alpha - 1;
		return	_alpha * _alpha * ((mOvershoot + 1) * _alpha + mOvershoot) + 1;
	}

	T	mOvershoot;
};


AMTRS_IMPLEMENTS_BEGIN(bounce)
template<class T, int BoundCount>
T bounce(float _alpha)
{
	static constexpr T	bounds_width	= 1 / static_cast<T>(BoundCount);

	if (_alpha < 1 / 2.75)
	{
		return 7.5625 * _alpha * _alpha;
	}
	else if (_alpha < 2 / 2.75)
	{
		_alpha -= 1.5 / 2.75;
		return	7.5625 * _alpha * _alpha + 0.75;
	}
	else if (_alpha < 2.5 / 2.75)
	{
		_alpha -= 2.25 / 2.75;
		return	7.5625 * _alpha * _alpha + 0.9375;
	}
	else
	{
		_alpha -= 2.625 / 2.75;
		return	7.5625 * _alpha * _alpha + 0.984375;
	}
}
AMTRS_IMPLEMENTS_END(bounce)



// ============================================================================
//! 徐々に大きくなるようにバウンドします
// ----------------------------------------------------------------------------
template<class T, int BoundCount = 4>
struct	bounce_ease_in
{
	constexpr T operator () (T _alpha)
	{
		return	1 - AMTRS_IMPLEMENTS(bounce)::bounce<T, BoundCount>(1 - _alpha);
	}
};


// ============================================================================
//! 重力を逆にしたようにバウンドします
// ----------------------------------------------------------------------------
template<class T, int BoundCount = 4>
struct	bounce_ease_out
{
	constexpr T operator () (T _alpha)
	{
		return	AMTRS_IMPLEMENTS(bounce)::bounce<T, BoundCount>(_alpha);
	}
};


template<class T, int BoundCount = 4>
struct	bounce_ease_in_out
{
	constexpr T operator () (T _alpha)
	{
		if (_alpha < 0.5f)
		{
			_alpha = _alpha * 2;
			return	(1 - AMTRS_IMPLEMENTS(bounce)::bounce<T, BoundCount>(1 - _alpha    )) * 0.5f;
		}
		else
		{
			return	     AMTRS_IMPLEMENTS(bounce)::bounce<T, BoundCount>(_alpha * 2 - 1)  * 0.5f + 0.5f;
		}
	}
};




template<class T>
struct	circ_ease_in
{
	constexpr T operator () (T _alpha)
	{
		return	-1 * (sqrt(1 - _alpha * _alpha) - 1);
	}
};

template<class T>
struct	circ_ease_out
{
	constexpr T operator () (T _alpha)
	{
		_alpha = _alpha - 1;
		return	sqrt(1 - _alpha * _alpha);
	}
};

template<class T>
struct	circ_ease_in_out
{
	constexpr T operator () (T _alpha)
	{
		_alpha = _alpha * 2;
		if (_alpha < 1)
		{
			return	-0.5f * (sqrt(1 - _alpha * _alpha) - 1);
		}
		else
		{
			_alpha	-= 2;
			return	0.5f * (sqrt(1 - _alpha * _alpha) + 1);
		}
	}
};




template<class T>
struct	elastic_ease_in
{
	elastic_ease_in(T _period = 0.3)
		: mPeriod(_period)
	{}

	constexpr T operator () (T _alpha)
	{
		if ((_alpha > 0) && (_alpha < 1))
		{
			float	s	= mPeriod / 4;
			_alpha	= _alpha - 1;
			_alpha	= -pow(2, 10 * _alpha) * sinf((_alpha - s) * (pi * 2) / mPeriod);
		}
		return	_alpha;
	}

	T	mPeriod;
};



template<class T>
struct	elastic_ease_out
{
	elastic_ease_out(T _period = 0.3)
		: mPeriod(_period)
	{}

	constexpr T operator () (T _alpha)
	{
		if (_alpha == 0 || _alpha == 1)
		{
			return	_alpha;
		}
		else
		{
			float s = mPeriod / 4;
			return	powf(2, -10 * _alpha) * sinf((_alpha - s) * pi*2 / mPeriod) + 1;
		}
	}

	T	mPeriod;
};



template<class T>
struct	elastic_ease_in_out
{
	elastic_ease_in_out(T _period = 0.3)
		: mPeriod(_period ? _period :  0.3f * 1.5f)
	{}

	constexpr T operator () (T _alpha)
	{
		if (_alpha == 0 || _alpha == 1)
		{
			return	_alpha;
		}
		else
		{
			float s = mPeriod / 4;
			_alpha = _alpha * 2 - 1;
			if (_alpha < 0)
			{
				return	-0.5f * pow(2, 10 * _alpha) * sin((_alpha -s) * pi*2 / mPeriod);
			}
			else
			{
				return	powf(2, -10 * _alpha) * sinf((_alpha - s) * pi*2 / mPeriod) * 0.5f + 1;
			}
		}
	}

	T	mPeriod;
};




template<class T>
struct	expo_ease_in
{
	constexpr T operator () (T _alpha)
	{
		return	_alpha == 0
				? 0
				: powf(2, 10 * (_alpha/1 - 1)) - 1 * 0.001f;
	}
};


template<class T>
struct	expo_ease_out
{
	constexpr T operator () (T _alpha)
	{
		return	_alpha == 1
				? 1
				: (-pow(2, -10 * _alpha / 1) + 1);
	}
};


template<class T>
struct	expo_ease_in_out
{
	constexpr T operator () (T _alpha)
	{
		if (_alpha == 0 || _alpha == 1)
		{
			return _alpha;
		}
		if (_alpha < 0.5f)
		{
			return	0.5f * pow(2, 10 * (_alpha * 2 - 1));
		}
		return	0.5f * (-pow(2, -10 * (_alpha * 2 - 1)) + 2);
	}
};


template<class T>
struct	 quad_ease_in
{
	constexpr T operator () (T _alpha)
	{
		return	_alpha * _alpha;
	}
};

template<class T>
struct	 quad_ease_out
{
	constexpr T operator () (T _alpha)
	{
		return	-1 * _alpha * (_alpha - 2);
	}
};

template<class T>
struct	 quad_ease_in_out
{
	constexpr T operator () (T _alpha)
	{
		_alpha	= _alpha * 2;
		if (_alpha < 1)
		{
			return	0.5f * _alpha * _alpha;
		}
		--_alpha;
		return	-0.5f * (_alpha * (_alpha - 2) - 1);
	}
};



template<class T> using quadratic_ease_in		= quad_ease_in<T>;
template<class T> using quadratic_ease_out		= quad_ease_out<T>;
template<class T> using quadratic_ease_in_out	= quad_ease_in_out<T>;



// ============================================================================
//! 矩形波
// ----------------------------------------------------------------------------
//! コンストラクタの引数で周期を指定できます。
// ----------------------------------------------------------------------------
template<class T>
struct	square_in
{
	square_in(T _cycle = 2)
		: mCycle(_cycle * pi * 2)
	{}

	constexpr T operator () (T _alpha)
	{
		auto	a	= sin(_alpha * mCycle);
		// sgn(a)
		return	  a < 0 ? -1
				: a > 0 ?  1
				: 0;
	}

	const T	mCycle;
};


// ============================================================================
//! 矩形波
// ----------------------------------------------------------------------------
//! コンストラクタの引数で周期を指定できます。
// ----------------------------------------------------------------------------
template<class T>
struct	square_out
{
	square_out(T _cycle = 2)
		: mCycle(_cycle * pi * 2)
	{}

	constexpr T operator () (T _alpha)
	{
		auto	a	= -sin(_alpha * mCycle);
		// sgn(a)
		return	  a < 0 ? -1
				: a > 0 ?  1
				: 0;
	}

	const T	mCycle;
};




template<class T>
struct	sine_ease_in
{
	constexpr T operator () (T _alpha)
	{
		return	-1 * cos(_alpha * (float)(pi * 2)) + 1;
	}
};

template<class T>
struct	sine_ease_out
{
	constexpr T operator () (T _alpha)
	{
		return	sin(_alpha * (float)(pi * 2));
	}
};

template<class T>
struct	sine_ease_in_out
{
	constexpr T operator () (T _alpha)
	{
		return	-0.5f * (cos(_alpha * (float)pi) - 1);
	}
};




} // namespace alpha
AMTRS_NAMESPACE_END
#endif

/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__action__hpp
#define	__libamtrs__node__action__hpp
AMTRS_NAMESPACE_BEGIN



// ============================================================================
//! アクションの基本動作を提供します。
// ----------------------------------------------------------------------------
//! このテンプレートから派生してアクションを作ることができます。
// ----------------------------------------------------------------------------
class	basic_action
		: public ref_object
{
public:
	using	action_type	= basic_action;

	static constexpr float	unlimit	= -1;	//!< 再生時間を持たないアクションの場合に指定します。

	basic_action(float _duration)
		: mDuration(_duration)
	{}

	virtual void update(float _delta)
	{
		if (!mStarted)
		{
			on_start();
		}
		if (!mFinish)
		{
			mElpased	+= _delta;
			on_update(mDuration != unlimit ? time() : _delta);
			if ((mDuration != unlimit) && (mElpased >= mDuration))
			{
				on_finish();
			}
		}
	}

	bool is_running() const noexcept { return !mFinish; }

	void set_target(transform<float>* _owner) noexcept { mOwner = _owner; }

	template<class FuncT>
	basic_action* set_finish_callback(FuncT&& _func) { mCallback = std::forward<FuncT>(_func); return this; }

	float elpased() const noexcept { return mElpased; }

	//! 再生時間を取得します。
	float duration() const noexcept { return mDuration; }

	//! 0～1のローカル時間に変換します。
	virtual float time() const noexcept
	{
		float	t	= mElpased / mDuration;
		return	  t < 0.0f ? 0.0f
				: t > 1.0f ? 1.0f
				: t;
	}

	void finish() noexcept
	{
		if (!mFinish)
		{
			mFinish = true;
			if (mCallback)
			{
				mCallback();
			}
		}
	}

	transform<float>* target() const noexcept { return mOwner; }



	//! イテレータで指定するアクションの再生時間の合計を取得します。
	template<class It>
	static float duration(It _first, It _last)
	{
		float	d	= 0;
		for (auto& i : make_bucket(std::forward<It>(_first), std::forward<It>(_last)))
		{
			d += i->duration();
		}
		return	d;
	}

	//! イテレータで指定するアクションの再生時間のうち最も時間の長いものを取得します。
	template<class It>
	static float max_duration(It _first, It _last)
	{
		float	d	= 0;
		for (auto& i : make_bucket(std::forward<It>(_first), std::forward<It>(_last)))
		{
			float	t	= i->duration();
			if (d < t)
			{
				d	= t;
			}
		}
		return	d;
	}


	//! アクション開始時に呼ばれる
	virtual void on_start()
	{
		mStarted	= true;
		mElpased	= 0;
	}

	//! アクションを更新する
	virtual void on_update(float _delta) {}

	//! アクションが終了したときに呼ばれる。
	virtual void on_finish()
	{
		finish();
	}


	// ====================================================================
	//! リピートなどが行われた時の挙動を記述します。
	// --------------------------------------------------------------------
	//! このメソッドを呼ばれたら最初から再生できるようしなくてはなりません。
	//! また、on_reset() の後に、on_start() が呼ばれます。
	// --------------------------------------------------------------------
	virtual void on_reset()
	{
		mStarted	= false;
		mFinish		= false;
	}


protected:
	bool					mStarted	= false;
	float					mDuration	= 0;
	float					mElpased	= 0;
	bool					mFinish		= false;

	transform<float>*		mOwner		= nullptr;
	std::function<void()>	mCallback;
};


// ============================================================================
//! アクションのアニメーション時間の変形を行います。
// ----------------------------------------------------------------------------
template<class TweenT>
class	basic_tween_action
		: public basic_action
{
	using	_base_type	= basic_action;

public:
	using	action_type	= typename _base_type::action_type;

	basic_tween_action(float _duration)
		: _base_type(_duration)
	{}

	template<class... TweenArgs>
	basic_tween_action(float _duration, TweenArgs&&... _targs)
		: _base_type(_duration)
		, mTween(std::forward<TweenArgs>(_targs)...)
	{}


	virtual float time() const noexcept override
	{
		return	mTween(_base_type::time());
	}


private:
	mutable TweenT	mTween;
};


// ============================================================================
//! 何もせずに一定時間待ちます。
// ----------------------------------------------------------------------------
//!	example)
//!		act = new wait_action<node::action_type>(
//!			1.0f		// duration time.
//!		);
// ----------------------------------------------------------------------------
class	wait_action
		: public basic_action
{
	using	_base_type	= basic_action;
public:
	template<class... TweenArgs>
	wait_action(float _duration, TweenArgs&&... _targs)
		: _base_type(_duration, std::forward<TweenArgs>(_targs)...)
	{}

	virtual void on_update(float _time) override
	{
		_base_type::on_update(_time);
	}
};



// ============================================================================
//! 複数のアクションを順に実行します。
// ----------------------------------------------------------------------------
//!	example)
//!		act = new sequence_action<node::action_type>(
//!			new actionA(),
//!			new actionA()
//!		);
// ----------------------------------------------------------------------------
template<class TweenT = waveform::linear<float>>
class	sequence_action
		: public basic_tween_action<TweenT>
{
	using	_base_type	= basic_tween_action<TweenT>;
public:
	using	action_type	= typename _base_type::action_type;

	sequence_action(std::initializer_list<ref<action_type>> _actions)
		: _base_type(_base_type::duration(_actions.begin(), _actions.end()))
		, mActions(_actions.begin(), _actions.end())
	{
		float	st	= 0;
		for (auto& a : mActions)
		{
			a.startTime = st;
			st += a.action->duration();
		}
	}

	virtual void on_start() override
	{
		_base_type::on_start();
		mCurrent		= nullptr;
		mPosition		= 0;
		mPrevElpased	= 0;
	}

	virtual void on_update(float _delta) override
	{
		_base_type::on_update(_delta);
		if (!mCurrent)
		{
			// 最後まで再生している場合は何もしない
			if (mPosition >= mActions.size())
			{
				_base_type::finish();
				return;
			}
			mCurrent	= &mActions[mPosition];
			mCurrent->action->set_target(this->target());
			++mPosition;
		}
		auto		elp		= _base_type::elpased();
		mCurrent->action->update((elp - mCurrent->startTime) - (mPrevElpased - mCurrent->startTime));
		mPrevElpased	= elp;
		if (!mCurrent->action->is_running())
		{
			mCurrent	= nullptr;
			if (mPosition >= mActions.size())
			{
				_base_type::finish();
				return;
			}
		}
	}


	virtual void on_finish() override
	{
	}

protected:
	struct	subaction
	{
		subaction(action_type* _act)
			: action(_act)
		{}

		ref<action_type>	action;
		float				startTime;
	};


	std::vector<subaction>	mActions;
	subaction*				mCurrent		= nullptr;
	std::size_t				mPosition		= 0;
	float					mPrevElpased	= 0;
};


// ============================================================================
//! 指定したアクションをリピートします。
// ----------------------------------------------------------------------------
//!	example)
//!		act = new repeat_action<node::action_type>(
//!			new actionA(),
//!			new actionB()
//!		);
// ----------------------------------------------------------------------------
class	repeat_sequence_action
		: public sequence_action<waveform::linear<float>>
{
	using	_base_type	= sequence_action<waveform::linear<float>>;
public:
	using	action_type	= typename _base_type::action_type;

	repeat_sequence_action(std::initializer_list<ref<action_type>> _actions)
		: _base_type(_actions)
	{}


	//! アクションが終了したときに呼ばれる。
	virtual void on_finish()
	{
		_base_type::on_reset();
		for (auto& act : _base_type::mActions)
		{
			act.action->on_reset();
		}
	}
};


// ============================================================================
//! 複数のアクションを並列に実行します。
// ----------------------------------------------------------------------------
//!	example)
//!		act = new parallel_action<node::action_type>(
//!			new actionA(),
//!			new actionA()
//!		);
// ----------------------------------------------------------------------------
template<class TweenT = waveform::linear<float>>
class	parallel_action
		: public basic_tween_action<TweenT>
{
	using	_base_type	= basic_tween_action<TweenT>;
public:
	using	action_type	= typename _base_type::action_type;

	parallel_action(std::initializer_list<ref<action_type>> _actions)
		: _base_type(_base_type::unlimit)
		, mActions(_actions.begin(), _actions.end())
	{}

	virtual void on_update(float _delta) override
	{
		_base_type::on_update(_delta);
		for (auto it = mActions.begin(); it != mActions.end(); )
		{
			action_type*	act	= (*it);
			act->set_target(this->target());
			act->update(_delta);
			if (act->is_running())
			{
				++it;
			}
			else
			{
				it = mActions.erase(it);
			}
		}
		if (mActions.empty())
		{
			this->finish();
		}
	}

protected:
	std::vector<ref<action_type>>	mActions;
};



// ============================================================================
//! スケールを変更します。
// ----------------------------------------------------------------------------
//!	example)
//!		act = new scale_to_action<node::action_type>(
//!			{1, 1, 1},	// to scale.
//!			1.0f		// duration time.
//!		);
// ----------------------------------------------------------------------------
template<class TweenT = waveform::linear<float>>
class	scale_to_action
		: public basic_tween_action<TweenT>
{
	using	_base_type	= basic_tween_action<TweenT>;
public:
	using	scale_type	= vec3<float>;

	template<class... TweenArgs>
	scale_to_action(transform<float>* _target, scale_type _scale, float _duration, TweenArgs&&... _targs)
		: _base_type(_duration, std::forward<TweenArgs>(_targs)...)
		, mTarget(_target)
		, mScale(_scale)
	{}

	template<class... TweenArgs>
	scale_to_action(scale_type _scale, float _duration, TweenArgs&&... _targs)
		: _base_type(_duration, std::forward<TweenArgs>(_targs)...)
		, mScale(_scale)
	{}

	virtual void on_start() override
	{
		_base_type::on_start();
		if (!mTarget)
		{
			mTarget	= this->target();
		}
		mStart	= mTarget->scale();
	}


	virtual void on_update(float _time) override
	{
		_base_type::on_update(_time);
		auto	scl		= (mStart * (1.0f - _time)) + (mScale * _time);
		mTarget->set_scale(scl);
	}

protected:
	transform<float>*	mTarget	= nullptr;
	scale_type			mStart;
	scale_type			mScale;
};

template<class TweenT = waveform::linear<float>>
using	scale_by_action	= scale_to_action<TweenT>;




// ============================================================================
//! スケールを変更します。
// ----------------------------------------------------------------------------
//!	example)
//!		act = new scale_to_action<node::action_type>(
//!			{1, 1, 1},	// to scale.
//!			1.0f		// duration time.
//!		);
// ----------------------------------------------------------------------------
template<class TweenT = waveform::linear<float>>
class	pivot_to_action
		: public basic_tween_action<TweenT>
{
	using	_base_type	= basic_tween_action<TweenT>;
public:
	using	position_type	= vec3<float>;

	template<class... TweenArgs>
	pivot_to_action(transform<float>* _target, position_type _pivot, float _duration, TweenArgs&&... _targs)
		: _base_type(_duration, std::forward<TweenArgs>(_targs)...)
		, mTarget(_target)
		, mTo(_pivot)
	{}

	template<class... TweenArgs>
	pivot_to_action(position_type _pivot, float _duration, TweenArgs&&... _targs)
		: _base_type(_duration, std::forward<TweenArgs>(_targs)...)
		, mTo(_pivot)
	{}

	virtual void on_start() override
	{
		_base_type::on_start();
		if (!mTarget)
		{
			mTarget	= this->target();
		}
		mStart	= mTarget->pivot();
	}


	virtual void on_update(float _time) override
	{
		_base_type::on_update(_time);
		auto	scl		= (mStart * (1.0f - _time)) + (mTo * _time);
		mTarget->set_pivot(scl);
	}

protected:
	transform<float>*	mTarget	= nullptr;
	position_type		mStart;
	position_type		mTo;
};

template<class TweenT = waveform::linear<float>>
using	pivot_by_action	= pivot_to_action<TweenT>;

// ============================================================================
//! 移動量を変更します。
// ----------------------------------------------------------------------------
//!	example)
//!		act = new move_by_action<node::action_type>(
//!			{1, 1, 1},	// to translate.
//!			1.0f		// duration time.
//!		);
// ----------------------------------------------------------------------------
template<class TweenT = waveform::linear<float>>
class	move_by_action
		: public basic_tween_action<TweenT>
{
	using	_base_type	= basic_tween_action<TweenT>;
public:
	using	position_type	= vec3<float>;

	template<class... TweenArgs>
	move_by_action(transform<float>* _target, position_type _pos, float _duration, TweenArgs&&... _targs)
		: _base_type(_duration, std::forward<TweenArgs>(_targs)...)
		, mTarget(_target)
		, mEnd(_pos)
	{}

	template<class... TweenArgs>
	move_by_action(position_type _pos, float _duration, TweenArgs&&... _targs)
		: _base_type(_duration, std::forward<TweenArgs>(_targs)...)
		, mEnd(_pos)
	{}

	virtual void on_start() override
	{
		_base_type::on_start();
		if (!mTarget)
		{
			mTarget	= this->target();
		}
		mStart	= mTarget->position();
	}

	virtual void on_update(float _time) override
	{
		_base_type::on_update(_time);
		mTarget->set_position(mEnd * _time + mStart * (1.0f - _time));
	}

protected:
	transform<float>*	mTarget	= nullptr;
	position_type		mStart;
	position_type		mEnd;
};


// ============================================================================
//! 位置とサイズを変更します。
// ----------------------------------------------------------------------------
//!	example)
//!		act = new content_size_by_action<node::action_type>(
//!			{{x, y, z},	{w, h, d}},		// position and size
//!			1.0f						// duration time.
//!		);
// ----------------------------------------------------------------------------
template<class TweenT = waveform::linear<float>>
class	content_size_by_action
		: public basic_tween_action<TweenT>
{
	using	_base_type	= basic_tween_action<TweenT>;
public:
	using	value_type	= box<float>;

	template<class... TweenArgs>
	content_size_by_action(transform<float>* _target, value_type _end, float _duration, TweenArgs&&... _targs)
		: _base_type(_duration, std::forward<TweenArgs>(_targs)...)
		, mTarget(_target)
		, mEnd(_end)
	{}

	template<class... TweenArgs>
	content_size_by_action(value_type _end, float _duration, TweenArgs&&... _targs)
		: _base_type(_duration, std::forward<TweenArgs>(_targs)...)
		, mEnd(_end)
	{}

	virtual void on_start() override
	{
		_base_type::on_start();
		if (!mTarget)
		{
			mTarget	= this->target();
		}
		mStart	= mTarget->content_size();
	}

	virtual void on_update(float _time) override
	{
		_base_type::on_update(_time);
		mTarget->set_content_size(mEnd * _time + mStart * (1.0f - _time));
	}

protected:
	transform<float>*	mTarget	= nullptr;
	value_type			mStart;
	value_type			mEnd;
};



// ============================================================================
//! 角度を四元素で指定して変更します。
// ----------------------------------------------------------------------------
//!	example)
//!		act = new rotate_by_action<node::action_type>(
//!			{1, 1, 1},	// to translate.
//!			1.0f		// duration time.
//!		);
// ----------------------------------------------------------------------------
template<class TweenT = waveform::linear<float>>
class	rotate_by_action
		: public basic_tween_action<TweenT>
{
	using	_base_type	= basic_tween_action<TweenT>;
public:
	using	value_type	= quaternion<float>;

	template<class... TweenArgs>
	rotate_by_action(transform<float>* _target, value_type _end, float _duration, TweenArgs&&... _targs)
		: _base_type(_duration, std::forward<TweenArgs>(_targs)...)
		, mTarget(_target)
		, mEnd(_end)
	{}

	template<class... TweenArgs>
	rotate_by_action(value_type _end, float _duration, TweenArgs&&... _targs)
		: _base_type(_duration, std::forward<TweenArgs>(_targs)...)
		, mEnd(_end)
	{}

	virtual void on_start() override
	{
		_base_type::on_start();
		if (!mTarget)
		{
			mTarget	= this->target();
		}
		mStart	= mTarget->rotate();
	}

	virtual void on_update(float _time) override
	{
		_base_type::on_update(_time);
		mTarget->set_rotate(mEnd * _time + mStart * (1.0f - _time));
	}

protected:
	transform<float>*	mTarget	= nullptr;
	value_type			mStart;
	value_type			mEnd;
};


// ============================================================================
//! 角度をオイラー角で指定して変更します。
// ----------------------------------------------------------------------------
//!	example)
//!		act = new rotate_by_action<node::action_type>(
//!			{1, 1, 1},	// to translate.
//!			1.0f		// duration time.
//!		);
// ----------------------------------------------------------------------------
template<class TweenT = waveform::linear<float>>
class	rotate_euler_by_action
		: public basic_tween_action<TweenT>
{
	using	_base_type	= basic_tween_action<TweenT>;
public:
	using	rotate_type	= vec3<float>;

	template<class... TweenArgs>
	rotate_euler_by_action(rotate_type _rot, float _duration, TweenArgs&&... _targs)
		: _base_type(_duration, std::forward<TweenArgs>(_targs)...)
		, mRotate(_rot)
	{}

	virtual void on_update(float _time) override
	{
		_base_type::on_update(_time);
		auto	rot		= quaternion<float>(mat33<float>::rotate_xzy(mRotate * _time));
		this->target()->set_rotate(rot);
	}

protected:
	rotate_type		mRotate;
};



AMTRS_IMPLEMENTS_BEGIN(make_action)
template<class Callback>
class	custom_action
		: public basic_action
{
	using	_base_type	= basic_action;
public:
	template<class FuncT>
	custom_action(float _duration, FuncT _callback)
		: _base_type(_duration)
		, mCallback(_callback)
	{}

	virtual void on_start() override
	{
		_base_type::on_start();
		mCallback(this, 0);
	}


	virtual void on_update(float _time) override
	{
		_base_type::on_update(_time);
		mCallback(this, _time);
	}

protected:
	Callback	mCallback;
};
AMTRS_IMPLEMENTS_END(make_action)



// ============================================================================
//! コールバックを使用したアクションを設定します。
// ----------------------------------------------------------------------------
//!	example)
//!		act = make_action<node::action_type>(
//!			[](auto* _act, float _time)
//!			{
//!				your action.
//!			}
//!		);
// ----------------------------------------------------------------------------
template<class Callback>
static auto* make_action(Callback&& _callback)
{
	return	new AMTRS_IMPLEMENTS(make_action)::custom_action<Callback>(basic_action::unlimit, _callback);
}


// ============================================================================
//! コールバックを使用したアクションを設定します。
// ----------------------------------------------------------------------------
//!	example)
//!		act = make_action<node::action_type>(
//!			time_of_duration,
//!			[](auto* _act, float _time)
//!			{
//!				your action.
//!			}
//!		);
// ----------------------------------------------------------------------------
template<class Callback>
static auto* make_action(float _duration, Callback&& _callback)
{
	return	new AMTRS_IMPLEMENTS(make_action)::custom_action<Callback>(_duration, _callback);
}


AMTRS_NAMESPACE_END
#endif

#ifndef	__libamtrs__message__hpp
#define	__libamtrs__message__hpp
#include "../amtrs.hpp"
AMTRS_NAMESPACE_BEGIN
template<class T>
class	message
{
public:
	using	value_type	= T;

	bool sync() noexcept
	{
		update();
		return	!empty();
	}

	bool empty() const noexcept { return !mTop; }

	T* top() const noexcept { return mTop ? &mTop->value : nullptr; }

	template<class S>
	void push(S _value)
	{
		msg*	m	= new msg(std::forward<S>(_value));
		while (!mPending.compare_exchange_strong(m->next, m))
		{
			;
		}
	}

	void pop()
	{
		if (mTop)
		{
			msg*	m	= mTop;
			mTop	= m->next;
			delete	m;
		}
	}


private:
	struct	msg
	{
		template<class S>
		msg(S _value)
			: value(std::forward<S>(_value))
		{}

		T		value;
		msg*	next	= nullptr;
	};


	void update()
	{
		msg*	expected	= nullptr;
		for (;;)
		{
			if (!mPending.compare_exchange_strong(expected, nullptr))
			{
				continue;
			}
			if (!expected)
			{
				break;
			}
			msg**	tail	= &mTop;
			if (*tail)
			{
				while ((*tail)->next)
				{
					tail	= &(*tail)->next;
				}
			}
			pending_move(tail, expected);
			expected->next	= nullptr;
			expected		= nullptr;
		}
	}

	msg** pending_move(msg** _msg, msg* _from)
	{
		if (_from->next)
		{
			_msg	= pending_move(_msg, _from->next);
		}
		*_msg	= _from;
		return	&(*_msg)->next;
	}


	msg*					mTop			= nullptr;
	std::atomic<msg*>		mPending		= nullptr;
};
AMTRS_NAMESPACE_END
#endif

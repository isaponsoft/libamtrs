/*
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__android__def__hpp
#define	__libamtrs__android__def__hpp
#include <list>
#include <android_native_app_glue.h>
#include "../def.hpp"
#define	AMTRS_OS_ANDROID_NAMESPACE_BEGIN	AMTRS_OS_NAMESPACE_BEGIN namespace android {
#define	AMTRS_OS_ANDROID_NAMESPACE_END		} AMTRS_OS_NAMESPACE_END
AMTRS_OS_ANDROID_NAMESPACE_BEGIN

enum class	lifecycle
{
	create,
	start,
	restart,
	resume,
	pause,
	stop,
	destroy,
};


template<class...>
class	event_listener;

template<class K, class... Args>
class	event_listener<K, void(Args...)>
{
public:
	using	key_type		= K;
	using	listener_type	= void(Args...);

	// ========================================================================
	//! ライフサイクルコールバックを登録します。
	// ------------------------------------------------------------------------
	void register_listener(const key_type& _key, std::function<listener_type>&& _listener)
	{
		mListenerList.push_back({_listener, _key});
	}


	// ========================================================================
	//! ライフサイクルコールバックを解除します。
	// ------------------------------------------------------------------------
	void unregister_listener(const key_type& _key)
	{
		mListenerList.remove_if([_key](auto& _state) -> bool
		{
			return	_state.key == _key;
		});
	}


	template<class... Value>
	void fire_event(Value&&... _value)
	{
		for (auto& st : mListenerList)
		{
			st.listener(std::forward<Value>(_value)...);
		}
	}

private:
	struct	listener_info
	{
		std::function<listener_type>	listener;
		key_type						key;
	};

	std::list<listener_info>			mListenerList;
};

AMTRS_OS_ANDROID_NAMESPACE_END
#endif

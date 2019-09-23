/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef __libamtrs__mutex__spinlock__hpp
#define __libamtrs__mutex__spinlock__hpp
#include <atomic>
#include "../amtrs.hpp"
AMTRS_NAMESPACE_BEGIN

// ==================================================================
// 単純なスピンロック
// ------------------------------------------------------------------
// ロックがほとんど起こらず、かつロックされる時間がごくわずかな場合に
// 適しています。
// 同一スレッドで呼び出すとデッドロックするので注意してください。
// ------------------------------------------------------------------
class	spinlock
{
public:
	spinlock() = default;
	spinlock(const spinlock&) = delete;
	spinlock(spinlock&&) = delete;
	spinlock& operator = (const spinlock&) = delete;

	inline void lock() noexcept
	{
		while (m_lockFlag.test_and_set(std::memory_order_acquire))
		{
			;
		}
	}

	inline bool try_lock() noexcept
	{
		if (m_lockFlag.test_and_set(std::memory_order_acquire))
		{
			return	false;
		}
		return	true;
	}

	inline void unlock() noexcept
	{
		m_lockFlag.clear(std::memory_order_release);
	}

private:
	std::atomic_flag	m_lockFlag	= ATOMIC_FLAG_INIT;
};

AMTRS_NAMESPACE_END
#endif

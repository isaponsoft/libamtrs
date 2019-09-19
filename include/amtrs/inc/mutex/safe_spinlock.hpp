/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef __libamtrs__mutex__safe_spinlock__hpp
#define __libamtrs__mutex__safe_spinlock__hpp
#include <atomic>
#include <thread>
#include "../../amtrs.hpp"
AMTRS_NAMESPACE_BEGIN

// ==================================================================
// 安全ななスピンロック
// ------------------------------------------------------------------
// ロックがほとんど起こらず、かつロックされる時間がごくわずかな場合に
// 適しています。
// spinlockと異なり、同一スレッドでロックしてもデッドロックしません。
// ------------------------------------------------------------------
class	safe_spinlock
{
	using	value_type	= decltype(std::thread::id());
public:
	safe_spinlock() : m_lockCount(0) { m_locking.store(std::thread::id()); }
	safe_spinlock(const safe_spinlock&) = delete;
	safe_spinlock(safe_spinlock&&) = delete;
	safe_spinlock& operator = (const safe_spinlock&) = delete;

	inline void lock() noexcept
	{
		value_type	tid			= std::this_thread::get_id();
		value_type	expected	= m_locking.load();

		// 自スレッドがロックしているならunlock用のカウントアップするのみ
		if (expected == tid)
		{
			++m_lockCount;
			return;
		}
		for (;;)
		{
			expected	= m_locking.load(std::memory_order_acquire);
			if (expected == std::thread::id())
			{
				if (m_locking.compare_exchange_strong(expected, tid, std::memory_order_acquire))
				{
					++m_lockCount;
					return;
				}
			}
		}
	}

	inline bool try_lock() noexcept
	{
		value_type	tid			= std::this_thread::get_id();
		value_type	expected	= m_locking.load();

		// 自スレッドがロックしているならunlock用のカウントアップするのみ
		if (expected == tid)
		{
			// 自スレッドでロック済み
			++m_lockCount;
			return	true;
		}
		if (expected == std::thread::id())
		{
			if (m_locking.compare_exchange_strong(expected, tid, std::memory_order_acquire))
			{
				// ロック獲得に成功
				++m_lockCount;
				return	true;
			}
		}
		return	false;
	}

	inline void unlock() noexcept
	{
		if (m_locking.load() == std::this_thread::get_id())
		{
			if (--m_lockCount == 0)
			{
				m_locking.store(std::thread::id(), std::memory_order_release);
			}
		}
	}

private:
	std::atomic<value_type>	m_locking;
	unsigned int			m_lockCount;
};

AMTRS_NAMESPACE_END
#endif

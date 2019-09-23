/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__g3d__memory_hpp
#define	__libamtrs__g3d__memory_hpp
#include <functional>
#include "def.hpp"
AMTRS_G3D_NAMESPACE_BEGIN
template<class OffsetType, class SizeType>
class	memory_mapped;
class	resorces;


class	resorces
{
public:
	class	memory_base
			: public ref_object
	{
	public:
		~memory_base() { if (mManager) { mManager->remove(this); } }

		virtual void on_gain() = 0;
		virtual void on_lost() = 0;

	private:
		resorces*	mManager	= nullptr;
		friend	resorces;
	};


	~resorces()
	{
		clear();
	}

	void set(memory_base* _o)
	{
		mContainor.push_back(_o);
		_o->mManager	= this;
	}

	void remove(memory_base* _o)
	{
		auto	it	= std::remove(mContainor.begin(), mContainor.end(), _o);
		if (it != mContainor.end())
		{
			_o->mManager	= nullptr;
			mContainor.erase(it, mContainor.end());
		}
	}

	void clear()
	{
		for (auto& o : mContainor) { o->mManager = nullptr; } 
		mContainor.clear();
	}

	auto begin() const noexcept { return mContainor.begin(); }
	auto end() const noexcept { return mContainor.end(); }
	std::size_t size() const noexcept { return mContainor.size(); }

	void cleanup()
	{
		for (auto& m : mContainor)
		{
			if (m->use_count() == 1)
			{
				m->mManager	= nullptr;
				m.clear();
			}
		}
		auto	it	= std::remove_if(mContainor.begin(), mContainor.end(), [](auto& a) { return a.empty(); });
		if (it != mContainor.end())
		{
			mContainor.erase(it, mContainor.end());
		}
	}

private:
	std::vector<ref<memory_base>>		mContainor;
};

// ============================================================================
//! GPUが扱うメモリの基本インターフェース。
// ----------------------------------------------------------------------------
//! メモリへのデータ転送とサイズの管理だけを行います。
//! GPUが直接バインドするメモリは g3d::device のみが生成することができ、
//! レンダリング時に自動生成／関連付けされます。
// ----------------------------------------------------------------------------
template<class OffsetType, class SizeType>
class	memory
		: public resorces::memory_base
{
public:
	using	offset_type		= OffsetType;
	using	size_type		= SizeType;
	using	memory_type		= memory<offset_type, size_type>;
	using	mapped_type		= memory_mapped<offset_type, size_type>;


	//! データを転送します。
	virtual void transfer(offset_type _pos, const void* _data, size_type _size) = 0;

	void transfer(const void* _data) { transfer({0}, _data, size()); }

	//! サイズを取得します。
	size_type size() const noexcept { return mSize; }

protected:
	size_type						mSize;
};


// ============================================================================
//! memory<> にマップされたデータを保持するクラスが持つインターフェース。
// ----------------------------------------------------------------------------
template<class OffsetType, class SizeType>
class	memory_mapped
{
public:
	using	offset_type		= OffsetType;
	using	size_type		= SizeType;
	using	memory_type		= memory<offset_type, size_type>;
	using	translate_queue	= std::function<void(memory_type*)>;


	memory_type* get_memory() const noexcept { return mMemory; }

	void set_memory(memory_type* _memory) noexcept { mMemory = _memory; }

	bool empty_transfer() const noexcept
	{
		return	mTranslateQueue.empty();
	}

	void transfer()
	{
		if (!mTranslateQueue.empty())
		{
			for (auto& queue : mTranslateQueue)
			{
				queue(get_memory());
			}
			mTranslateQueue.clear();
		}
	}

	void set_translate(translate_queue&& _queue)
	{
		mTranslateQueue.push_back(_queue);
	}

private:
	ref<memory_type>				mMemory;
	std::vector<translate_queue>	mTranslateQueue;
};


AMTRS_G3D_NAMESPACE_END
#endif

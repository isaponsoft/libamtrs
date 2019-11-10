#ifndef	__passion__vmbuffer__hpp
#define	__passion__vmbuffer__hpp
#include "def.hpp"
#include "vmobject.hpp"
PASSION_NAMESPACE_BEGIN


// ****************************************************************************
//! サイズを保持する配列。
// ----------------------------------------------------------------------------
//! 機能を絞った std::vector のようなものです。機能が少ない分軽量です。
// ----------------------------------------------------------------------------
template<class T, class Allocator = std::allocator<T>>
class	vmbuffer
{
public:
	using	allocator_type		= Allocator;			//!< not use this version.
	using	size_type			= std::size_t;
	using	value_type			= T;
	using	pointer				= T*;
	using	const_pointer		= const T*;
	using	iterator			= T*;
	using	const_iterator		= const T*;
	using	reference			= T&;
	using	const_reference		= const T&;

	// コピー禁止
	vmbuffer(const vmbuffer& _v) = delete;
	vmbuffer& operator = (const vmbuffer& _v) = delete;

	// ========================================================================
	//! デフォルトコンストラクタ
	// ------------------------------------------------------------------------
	vmbuffer() = default;

	// ========================================================================
	//! ムーブコンストラクタ
	// ------------------------------------------------------------------------
	vmbuffer(vmbuffer&& _v) : mMemory(_v.mMemory), mSize(_v.mSize) { _v.mMemory = nullptr; _v.mSize = 0; }

	// ========================================================================
	//! 指定するサイズの分だけ確保する
	// ------------------------------------------------------------------------
	vmbuffer(size_type _s, const allocator_type& _alloc = allocator_type())
		: mAllocator(_alloc)
	{
		if (_s > 0)
		{
			mMemory	= mAllocator.allocate(_s);
			while (mSize < _s)
			{
				new (&mMemory[mSize++]) value_type();
			}
		}
	}

	// ========================================================================
	//! 指定する範囲のデータで初期化した配列を用意する。
	// ------------------------------------------------------------------------
	template<class InputIt>
	vmbuffer(InputIt _first, InputIt _last, const allocator_type& _alloc = allocator_type())
		: mAllocator(_alloc)
	{
		if (_first != _last)
		{
			auto	s	= std::distance(_first, _last);
			mMemory	= mAllocator.allocate(s);
			while (mSize < s)
			{
				new (&mMemory[mSize]) value_type(_first[mSize]);
				++mSize;
			}
		}
	}

	~vmbuffer() { clear(); }


	vmbuffer& operator = (vmbuffer&& _v) noexcept { clear(); mMemory = _v.mMemory; mSize = _v.mSize; _v.mMemory = nullptr; _v.mSize = 0; return *this; }

	iterator begin() noexcept { return mMemory; }
	iterator end() noexcept { return mMemory + mSize; }

	const_iterator begin() const noexcept { return mMemory; }
	const_iterator end() const noexcept { return mMemory + mSize; }
	const_iterator cbegin() const noexcept { return mMemory; }
	const_iterator cend() const noexcept { return mMemory + mSize; }

	iterator rbegin() noexcept { return end() - 1; }
	iterator rend() noexcept { return begin() - 1; }

	const_iterator rbegin() const noexcept { return end() - 1; }
	const_iterator rend() const noexcept { return begin() - 1; }
	const_iterator crbegin() const noexcept { return end() - 1; }
	const_iterator crend() const noexcept { return begin() - 1; }

	reference front() { return *begin(); }
	const_reference front() const { return *begin(); }

	reference back() { return *rbegin(); }
	const_reference back() const { return *rbegin(); }

	size_type size() const noexcept { return mSize; }

	bool empty() const noexcept { return size() == 0; }

	void clear()
	{
		if (mSize > 0)
		{
			auto	s	= mSize;
			do
			{
				mMemory[--mSize].~value_type();
			} while (mSize > 0);
			mAllocator.deallocate(mMemory, s);
		}
	}

	value_type* data() noexcept { return mMemory; }
	const value_type* data() const noexcept { return mMemory; }

	reference operator [] (size_type _i) noexcept { return mMemory[_i]; }
	const_reference operator [] (size_type _i) const noexcept { return mMemory[_i]; }

	void swap(vmbuffer _b)
	{
		std::swap(mMemory, _b.mMemory);
		std::swap(mSize, _b.mSize);
	}

private:
	T*			mMemory	= nullptr;
	size_type	mSize	= 0;
	Allocator	mAllocator;
};


PASSION_NAMESPACE_END
#endif

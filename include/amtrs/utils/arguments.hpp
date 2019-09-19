/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__type_traits__hpp
#define	__libamtrs__type_traits__hpp
#include "../amtrs.hpp"
AMTRS_NAMESPACE_BEGIN
template<class T>
class	basic_arguments
{
public:
	using	size_type		= std::size_t;
	using	value_type		= T;
	using	const_pointer	= const value_type*;
	using	pointer			= const_pointer;
	using	reference		= pointer&;
	using	const_reference	= const_pointer&;

	// ========================================================================
	//! 先頭の値を返し、かつpop() を行います。
	// ------------------------------------------------------------------------
	//! 値があれば先頭の値を返しますが、なければ nullptr を返します。
	// ------------------------------------------------------------------------
	const_pointer shift() noexcept { if (empty()) { return nullptr; } auto retval = front(); pop(); return retval; }


	// ========================================================================
	//! 先頭の値をポップします。
	// ------------------------------------------------------------------------
	void pop() noexcept { if (mCursor < mArgc) { ++mCursor; } }


	// ========================================================================
	//! カーソルを戻します。
	// ------------------------------------------------------------------------
	void rollback(size_type _size) { mCursor -= std::min(_size, mCursor); }


	basic_arguments() = default;
	basic_arguments(const basic_arguments&) = default;
	basic_arguments(basic_arguments&&) = default;
	basic_arguments& operator = (const basic_arguments&) = default;
	basic_arguments& operator = (basic_arguments&&) = default;

	basic_arguments(const_pointer* _args, size_type _argc)
		: mArgs(_args)
		, mArgc(_argc)
	{}

	basic_arguments(value_type* _args[], size_type _argc)
		: mArgs(const_cast<const_pointer*>(_args))
		, mArgc(_argc)
	{}


	bool empty() const noexcept { return size() == 0; }
	size_type size() const noexcept { return mArgc - mCursor; }
	const_pointer* begin() const noexcept { return &mArgs[mCursor]; }
	const_pointer* end() const noexcept { return &mArgs[mArgc]; }
	const_reference front() const noexcept { return *begin(); }

	const_pointer operator [] (int _index) const noexcept { return mArgs[mCursor + _index]; }

private:
	const_pointer*	mArgs	= nullptr;
	size_type		mArgc	= 0;
	size_type		mCursor	= 0;
};
using	arguments	= basic_arguments<char>;
AMTRS_NAMESPACE_END
#endif


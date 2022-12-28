/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__g3d__vulkan__error__hpp
#define	__libamtrs__g3d__vulkan__error__hpp
AMTRS_G3D_VKUTIL_NAMESPACE_BEGIN


class	_vklasterror
{
public:
	using	value_type	= VkResult;

	operator value_type () const noexcept
	{
		return	storage();
	}

	value_type operator = (value_type _r) const noexcept
	{
		storage()	= _r;
		return	_r;
	}

	void clear() const noexcept
	{
		storage()	= VK_SUCCESS;
	}

	bool good() const noexcept
	{
		return	storage() == VK_SUCCESS;
	}

protected:
	static value_type& storage() noexcept
	{
		static thread_local value_type	_s = VK_SUCCESS;
		return	_s;
	}
};


// ******************************************************************
//! vklasterr 変数は vkutil が最後に起こしたエラーを記録します。
// ------------------------------------------------------------------
//! ※この値はスレッドセーフです。
//! vklasterr が VK_SUCCESS 以外の時、各関数は無条件に失敗します。
//! エラーをリカバリーするときは必ず vklasterr.clear() を呼び出して
//! ください。
// ------------------------------------------------------------------
inline _vklasterror		vklasterr;



AMTRS_G3D_VKUTIL_NAMESPACE_END
#endif

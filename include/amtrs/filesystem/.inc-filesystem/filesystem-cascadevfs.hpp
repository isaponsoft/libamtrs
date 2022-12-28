/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef __libamtrs__filesystem__cascadevfs__hpp
#define __libamtrs__filesystem__cascadevfs__hpp
AMTRS_FILESYSTEM_NAMESPACE_BEGIN

// ============================================================================
//! 複数の vfs を保持します。
// ----------------------------------------------------------------------------
class	cascadevfs
		: public vfs
{
public:
	using	vfsptr		= ref<vfs>;

	static ref<cascadevfs> create();

	virtual void push_front(vfsptr _fs) = 0;
	virtual void push_back(vfsptr _fs) = 0;
	virtual void erase(vfs* _fs) = 0;
};


AMTRS_FILESYSTEM_NAMESPACE_END
#endif

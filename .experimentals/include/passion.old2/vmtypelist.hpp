/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__passion__vmtypelist__hpp
#define	__libamtrs__passion__vmtypelist__hpp
#include <unordered_map>
#include "string.hpp"
#include "vmtypedata.hpp"
PASSION_NAMESPACE_BEGIN

//! 型リスト
class	vmtypelist
{
public:
	vmtypedata& add(vmtypedata _data);
	vmtypedata* find(vmstring const& _name);

	auto begin() { return mTypes.begin(); }
	auto end() { return mTypes.end(); }
	auto begin() const { return mTypes.begin(); }
	auto end() const { return mTypes.end(); }

	//! 組み込み型をセットアップします。
	void setup_fundamental();

private:
	std::unordered_map<vmstring, vmtypedata>	mTypes;
};


PASSION_NAMESPACE_END
#endif

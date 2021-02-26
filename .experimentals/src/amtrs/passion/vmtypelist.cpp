/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <passion/vmtypelist.hpp>
using namespace amtrs;
using namespace PASSION_NAMESPACE;


vmtypedata& vmtypelist::add(vmtypedata _data)
{
	auto	key	= _data.string();
	auto	r	= mTypes.emplace(key, std::move(_data));
	return	r.first->second;
}

vmtypedata* vmtypelist::find(vmstring const& _name)
{
	auto	it	= mTypes.find(_name);
	return	it == mTypes.end() ? nullptr : &it->second;
}



//! 組み込み型をセットアップします。
void vmtypelist::setup_fundamental()
{
	add(vmtypedata(vmtypedata::fundamental, "int"))		.set_align(sizeof(void*));
	add(vmtypedata(vmtypedata::fundamental, "int8_t"))	.set_align(1);
	add(vmtypedata(vmtypedata::fundamental, "int16_t"))	.set_align(2);
	add(vmtypedata(vmtypedata::fundamental, "int32_t"))	.set_align(4);
	add(vmtypedata(vmtypedata::fundamental, "int64_t"))	.set_align(8);
}

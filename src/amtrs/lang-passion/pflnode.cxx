/* Copyright (c) 2022, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
/*
 * 汎用ライブラリ
 * OSや環境問わずに使用できる。
 */
PSNVM_LANG_NAMESPACE_BEGIN

auto is_destractor_symbol(pflnode& _n) -> bool
{
	auto	t	= _n.type();
	if (t == pflnode_types::symbol_name)
	{
		return	_n.at<pn_symbol_name>()->name.starts_with("~");
	}
	if (t == pflnode_types::symbol_scope)
	{
		return	is_destractor_symbol(_n.at<pn_symbol_scope>()->scopes.back());
	}
	return	false;
}


auto pflnode::is_symbol() const noexcept -> bool
{
	auto t = type();
	return t == pflnode_types::symbol_decltye
		|| t == pflnode_types::symbol_name || t == pflnode_types::symbol_template
		|| t == pflnode_types::symbol_scope || t == pflnode_types::sepc_symbol_pointer || t == pflnode_types::spec_symbol_const
		|| t == pflnode_types::spec_symbol_reference || t == pflnode_types::spec_symbol_move || t == pflnode_types::spec_symbol_parampack
		|| t == pflnode_types::symbol_functype || t == pflnode_types::sepc_symbol_pointer
	;
}


PSNVM_LANG_NAMESPACE_END

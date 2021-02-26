/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
/*
 * namespace.h's c++ wrapper.
 */
#ifndef	__passion__namespaces__hpp
#define	__passion__namespaces__hpp
#ifdef	__cplusplus
#include "namespaces.h"
PASSION_NAMESPACE_BEGIN


class	namespaces
{
public:
	using	value_type			= passion_namespace_node;
	using	view_type			= amtrs_string_view;
	using	enumlate_callback	= passion_bool(*)(value_type* _node, void* _userdata);

	static inline view_type normalize(view_type _namespace) noexcept { return passion_namespace_normalize_namespace(_namespace); }


	static inline amtrs_string* join(view_type _namespace1, view_type _namespace2) noexcept { return passion_namespace_join(_namespace1, _namespace2); }


	static inline view_type parent(view_type _namespace) noexcept { return passion_namespace_parent_namespace(_namespace); }

	namespaces();
	~namespaces();


	auto add(view_type _namespace) noexcept -> value_type*;


	auto find(view_type _namespace) noexcept -> value_type*;


	auto find_by_current(view_type _current_namespace, view_type _namespace) noexcept -> value_type*;


	auto find_joined(view_type _parent_namespace, view_type _namespace) noexcept -> value_type*;


	bool has(view_type _namespace) noexcept;


	size_t enumlate(enumlate_callback _callback, void* _userdata) noexcept;

private:
	passion_namespaces	mNamespaces;
};



inline namespaces::namespaces()
{
	passion_namespaces_initialize(&mNamespaces);
}

inline namespaces::~namespaces()
{
	passion_namespaces_destroy(&mNamespaces);
}


inline auto namespaces::add(view_type _namespace) noexcept -> value_type*
{
	return	passion_namespaces_add(&mNamespaces, _namespace);
}

inline auto namespaces::find(view_type _namespace) noexcept -> value_type*
{
	return	passion_namespaces_find(&mNamespaces, _namespace);
}

inline auto namespaces::find_by_current(view_type _current_namespace, view_type _namespace) noexcept -> value_type*
{
	return	passion_namespaces_find_by_current_namespace(&mNamespaces, _current_namespace, _namespace);
}

inline auto namespaces::find_joined(view_type _parent_namespace, view_type _namespace) noexcept -> value_type*
{
	return	passion_namespaces_find_joined(&mNamespaces, _parent_namespace, _namespace);
}

inline bool namespaces::has(view_type _namespace) noexcept
{
	return	passion_namespaces_has(&mNamespaces, _namespace);
}

inline size_t namespaces::enumlate(enumlate_callback _callback, void* _userdata) noexcept
{
	return	passion_namespaces_enumlate(&mNamespaces, _callback, _userdata);
}

PASSION_NAMESPACE_END
#endif	// __cplusplus
#endif

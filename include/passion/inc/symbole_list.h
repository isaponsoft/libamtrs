/*
 * Copyright (c) 2019 Isao Shibuya (isapon)
 * Released under the MIT license
 * https://opensource.org/licenses/mit-license.php
 */
#ifndef	__passion__symbole_list__h
#define	__passion__symbole_list__h
#include "string.h"
#include "namespaces.h"
PASSION_EXTERN_C_BEGIN

struct	passion_class;						// prototype.
struct	passion_function;					// prototype.

enum	passion_symbole_type
{
	passion_symbole_none			= 0,		//!< 存在しない
	passion_symbole_type_alias		= 1,		//!< 型へのエイリアス
	passion_symbole_namespace		= 3,		//!< 名前空間
	passion_symbole_function		= 4,		//!< 関数定義
	passion_symbole_variable		= 5,		//!< 変数定義(ローカル・引数を除く)
	passion_symbole_class			= 6,		//!< クラス定義(構造体含む)
	passion_symbole_namespace_alias	= 7			//!< 名前空間へのエイリアス
};



struct	passion_symbole
{
	enum passion_symbole_type	type;
	amtrs_string*				text;
	union
	{
		amtrs_string*				alias;			//!< type == passion_symbole_type_alias, passion_symbole_namespace_alias
		amtrs_string*				var_type;			//!< type == passion_symbole_variable
		struct passion_function*	function;		//!< type == passion_symbole_function
		struct passion_class*		clazz;			//!< type == passion_symbole_class
	}							data;
	size_t						hash;
	struct passion_symbole*		left;
	struct passion_symbole*		right;
};


struct	passion_symbole_list
{
	struct passion_symbole*				root;
	struct passion_symbole_list const*	cascade;		//!< 値をセットしておくと自身に見つからなかったときにカスケード先から検索するようになる。

	#ifdef	__cplusplus
	passion_symbole_list() = default;
	passion_symbole_list(const passion_symbole_list&) = delete;
	passion_symbole_list& operator = (const passion_symbole_list&) = delete;
	passion_symbole_list(passion_symbole_list&& _r)
		: root(_r.root)
		, cascade(_r.cascade)
	{
		_r.root		= nullptr;
		_r.cascade	= nullptr;
	}
	#endif
};




//! Impliments. Not use.
inline extern size_t __passion_symbole_list_enumlate(passion_bool* _continue, struct passion_symbole* _node, passion_bool(*_callback)(struct passion_symbole*, void*), void* _userdata);
inline extern struct passion_symbole* __passion_symbole_list_add(struct passion_symbole_list* _list, amtrs_string_view _add);



// ============================================================================
//! Initialize namespaces.
// ----------------------------------------------------------------------------
inline extern struct passion_symbole_list* passion_symbole_list_initialize(struct passion_symbole_list* _list)
{
	_list->root			= passion_null;
	_list->cascade		= passion_null;
	return	_list;
}


// ============================================================================
//! Destroy namespace node.
// ----------------------------------------------------------------------------
inline extern void passion_symbole_destroy(struct passion_symbole* _node)
{
	if (_node)
	{
		passion_symbole_destroy(_node->left);
		passion_symbole_destroy(_node->right);
		amtrs_string_destroy(_node->text);
		if (_node->type == passion_symbole_type_alias)
		{
			amtrs_string_destroy(_node->data.alias);
		}
		free((void*)_node);
	}
}


// ============================================================================
//! Destroy namespaces.
// ----------------------------------------------------------------------------
inline extern void passion_symbole_list_destroy(struct passion_symbole_list* _list)
{
	if (_list)
	{
		passion_symbole_destroy(_list->root);
	}
}


inline extern struct passion_symbole* passion_symbole_list_add_type_alias(struct passion_symbole_list* _list, amtrs_string_view _alias, amtrs_string_view _alias_type)
{
	struct passion_symbole*	retval	= __passion_symbole_list_add(_list, _alias);
	if (retval)
	{
		retval->type		= passion_symbole_type_alias;
		retval->data.alias	= amtrs_string_create_from_string_view(_alias_type);
	}
	return	retval;
}


inline extern struct passion_symbole* passion_symbole_list_add_variable(struct passion_symbole_list* _list, amtrs_string_view _name, amtrs_string_view _type)
{
	struct passion_symbole*	retval	= __passion_symbole_list_add(_list, _name);
	if (retval)
	{
		retval->type			= passion_symbole_variable;
		retval->data.var_type	= amtrs_string_create_from_string_view(_type);
	}
	return	retval;
}

inline extern struct passion_symbole* passion_symbole_list_add_function(struct passion_symbole_list* _list, amtrs_string_view _function_name, struct passion_function* _function)
{
	struct passion_symbole*	retval	= __passion_symbole_list_add(_list, _function_name);
	if (retval)
	{
		retval->type			= passion_symbole_function;
		retval->data.function	= _function;
	}
	return	retval;
}


inline extern struct passion_symbole* passion_symbole_list_add_class(struct passion_symbole_list* _list, amtrs_string_view _class_name)
{
	struct passion_symbole*	retval	= __passion_symbole_list_add(_list, _class_name);
	if (retval)
	{
		retval->type	= passion_symbole_class;
	}
	return	retval;
}


inline extern struct passion_symbole* passion_symbole_list_add_namespace(struct passion_symbole_list* _list, amtrs_string_view _namespace)
{
	struct passion_symbole*	retval	= __passion_symbole_list_add(_list, _namespace);
	if (retval)
	{
		retval->type			= passion_symbole_namespace;

		amtrs_string_view	parent	= passion_namespace_parent_namespace(_namespace);
		if (parent.len > 0)
		{
			passion_symbole_list_add_namespace(_list, parent);
		}
	}
	return	retval;
}



// ============================================================================
//! Find namespace node in namespaces.
// ----------------------------------------------------------------------------
inline extern struct passion_symbole* passion_symbole_list_find(struct passion_symbole_list const* _list, amtrs_string_view _find, passion_bool _no_alias)
{
	if (!_list)
	{
		return	passion_null;
	}
	else
	{
		struct passion_symbole*	current	= _list->root;
		size_t							hash	= amtrs_string_view_compute_hash(_find);
		while (current)
		{
			if (hash == current->hash)
			{
				if (amtrs_string_view_equals(_find, amtrs_string_to_string_view(current->text)))
				{
					if (_no_alias)
					{
						return	current;
					}
					struct passion_symbole*	alias	= passion_symbole_list_find(_list, amtrs_string_to_string_view(current->data.alias), passion_true);
					return	alias ? alias : current;
				}
			}
			current	= (hash < current->hash)
					? current->left
					: current->right;
		}
		if (_list->cascade)
		{
			return	passion_symbole_list_find(_list->cascade, _find, _no_alias);
		}
		return	passion_null;
	}
}




// ============================================================================
//! Find namespace node in namespaces.
// ----------------------------------------------------------------------------
inline extern passion_bool passion_symbole_list_has(struct passion_symbole_list const* _list, amtrs_string_view _find)
{
	return	passion_symbole_list_find(_list, _find, passion_false)
			? passion_true
			: passion_false;
}


// ============================================================================
//! Enumlare namespaces
// ----------------------------------------------------------------------------
inline extern size_t passion_symbole_list_enumlate(struct passion_symbole_list const* _list, passion_bool(*_callback)(struct passion_symbole*, void*), void* _userdata)
{
	size_t			count	= 0;
	if (_list)
	{
		passion_bool	ctn		= passion_true;
		if (_list->root)
		{
			count	= __passion_symbole_list_enumlate(&ctn, _list->root, _callback, _userdata);
		}
		if (_list->cascade && ctn)
		{
			count += passion_symbole_list_enumlate(_list->cascade, _callback, _userdata);
		}
	}
	return	count;
}


// ============================================================================
//! Cascade top.
// ----------------------------------------------------------------------------
inline extern struct passion_symbole_list* passion_symbole_top_cascade(struct passion_symbole_list* _list)
{
	while (_list->cascade)
	{
		_list	= (struct passion_symbole_list*)_list->cascade;
	}
	return	_list;
}





inline extern size_t __passion_symbole_list_enumlate(passion_bool* _continue, struct passion_symbole* _node, passion_bool(*_callback)(struct passion_symbole*, void*), void* _userdata)
{
	size_t	retval	= 1;
	*_continue	= _callback(_node, _userdata);
	if (*_continue)
	{
		if (_node->left)
		{
			retval += __passion_symbole_list_enumlate(_continue, _node->left, _callback, _userdata);
		}
		if (_node->right && *_continue)
		{
			retval += __passion_symbole_list_enumlate(_continue, _node->right, _callback, _userdata);
		}
	}
	return	retval;
}




// ============================================================================
//! Add
// ----------------------------------------------------------------------------
inline extern struct passion_symbole* __passion_symbole_list_add(struct passion_symbole_list* _list, amtrs_string_view _add)
{
	amtrs_string_view		add		= passion_namespace_normalize_namespace(_add);
	if (_list && add.len > 0)
	{
		struct passion_symbole**	current	= &_list->root;
		size_t						hash	= amtrs_string_view_compute_hash(add);
		while (*current)
		{
			if (hash == (*current)->hash)
			{
				if (amtrs_string_view_equals(add, amtrs_string_to_string_view((*current)->text)))
				{
					return	(*current);
				}
			}
			current	= (hash < (*current)->hash)
					? &(*current)->left
					: &(*current)->right;
		}

		*current	= (struct passion_symbole*)malloc(sizeof(struct passion_symbole));
		memset(*current, 0, sizeof(struct passion_symbole));
		(*current)->type			= passion_symbole_none;
		(*current)->text			= amtrs_string_create_from_string_view(add);
		(*current)->hash			= hash;
		return	(*current);
	}
	return	passion_null;
}

PASSION_EXTERN_C_END
#endif

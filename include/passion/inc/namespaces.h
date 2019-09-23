/*
 * Copyright (c) 2019 Isao Shibuya (isapon)
 * Released under the MIT license
 * https://opensource.org/licenses/mit-license.php
 */
#ifndef	__passion__namespaces__h
#define	__passion__namespaces__h
PASSION_EXTERN_C_BEGIN

struct	passion_namespace_node
{
	amtrs_string*			text;
	size_t							hash;
	struct passion_namespace_node*	left;
	struct passion_namespace_node*	right;
};


struct	passion_namespaces
{
	struct passion_namespace_node*	root;
};


//! Impliments. Not use.
inline extern size_t __passion_namespaces_enumlate(struct passion_namespace_node* _node, passion_bool(*_callback)(struct passion_namespace_node*, void*), void* _userdata);


// ============================================================================
//! Join namespace.
// ----------------------------------------------------------------------------
inline extern amtrs_string* passion_namespace_join(amtrs_string_view _parent, amtrs_string_view _child)
{
	amtrs_string_view	separator		= amtrs_string_view_make("::");
	if (amtrs_string_view_ends_with(_parent, separator))
	{
		_parent.len	-= 2;
	}
	if (amtrs_string_view_starts_with(_child, separator))
	{
		_child.txt	+= separator.len;
		_child.len	-= 2;
	}

	size_t					totalSize		= _parent.len + separator.len + _child.len;
	amtrs_string*	retval			= amtrs_string_create(totalSize);
	if (!retval)
	{
		return	passion_null;
	}

	char*				cursor			= (char*)retval->text;
	memcpy(cursor, _parent.txt,		_parent.len);		cursor += _parent.len;
	memcpy(cursor, separator.txt,	separator.len);	cursor += separator.len;
	memcpy(cursor, _child.txt,		_child.len);		cursor += _child.len;
	return	retval;
}


// ============================================================================
//! Normalize namespace.
// ----------------------------------------------------------------------------
//! "NS1"           => "NS1"
//! "NS1::"         => "NS1"
//! "NS1::NS2::NS3" => "NS1::NS2::NS3"
//! "::NS1::NS2"    => "NS1::NS2"
//! ""              => ""
//! "::"            => ""
// ----------------------------------------------------------------------------
inline extern amtrs_string_view passion_namespace_normalize_namespace(amtrs_string_view _namespace)
{
	amtrs_string_view	retval	= _namespace;
	const char*					end		= retval.txt + retval.len;
	while (retval.txt != end && *retval.txt == ':')
	{
		++retval.txt;
	}
	retval.len	= end - retval.txt;

	while (retval.txt != end && *(end-1) == ':')
	{
		--end;
	}
	retval.len	= end - retval.txt;
	return	retval;
}


// ============================================================================
//! Parent namespace and current.
// ----------------------------------------------------------------------------
//! "NS1"           => "" "NS1"
//! "NS1::"         => "NS1"
//! "NS1::NS2::NS3" => "NS1::NS2" "NS3"
//! "::NS1::NS2"    => "::NS1" "NS2"
//! ""              => ""
//! "::"            => "::"
// ----------------------------------------------------------------------------
inline extern amtrs_string_view passion_namespace_parent_namespace_and_current(amtrs_string_view* _current, amtrs_string_view _namespace)
{
	const char*			last	= amtrs_string_view_rfind(_namespace, amtrs_string_view_make("::"));
	amtrs_string_view	retval;
	retval.txt	= _namespace.txt;
	retval.len	= 0;
	if (last)
	{
		retval.len		= last - retval.txt;
		_current->txt	= retval.txt + retval.len + 2;
		_current->len	= _namespace.len - retval.len - 2;
	}
	else
	{
		retval.len		= 0;
		*_current		= _namespace;
	}
	return	retval;
}


// ============================================================================
//! Parent namespace.
// ----------------------------------------------------------------------------
//! "NS1"           => ""
//! "NS1::"         => "NS1"
//! "NS1::NS2::NS3" => "NS1::NS2"
//! "::NS1::NS2"    => "::NS1"
//! ""              => ""
//! "::"            => "::"
// ----------------------------------------------------------------------------
inline extern amtrs_string_view passion_namespace_parent_namespace(amtrs_string_view _namespace)
{
	const char*					last	= amtrs_string_view_rfind(_namespace, amtrs_string_view_make("::"));
	amtrs_string_view	retval;
	retval.txt	= _namespace.txt;
	retval.len	= 0;
	if (last)
	{
		retval.len	= last - retval.txt;
	}
	return	retval;
}


// ============================================================================
//! Initialize namespaces.
// ----------------------------------------------------------------------------
inline extern struct passion_namespaces* passion_namespaces_initialize(struct passion_namespaces* _list)
{
	// register global namespace "".
	amtrs_string_view	root	= amtrs_string_view_make("");
	_list->root			= (struct passion_namespace_node*)malloc(sizeof(struct passion_namespace_node));
	_list->root->text	= amtrs_string_create_from_string_view(root);
	_list->root->hash	= amtrs_string_view_compute_hash(root);
	_list->root->left	= passion_null;
	_list->root->right	= passion_null;
	return	_list;
}


// ============================================================================
//! Destroy namespace node.
// ----------------------------------------------------------------------------
inline extern void passion_namespaces_destroy_namespace_node(struct passion_namespace_node* _node)
{
	if (_node)
	{
		passion_namespaces_destroy_namespace_node(_node->left);
		passion_namespaces_destroy_namespace_node(_node->right);
		amtrs_string_destroy(_node->text);
		free((void*)_node);
	}
}


// ============================================================================
//! Destroy namespaces.
// ----------------------------------------------------------------------------
inline extern void passion_namespaces_destroy(struct passion_namespaces* _list)
{
	if (_list)
	{
		passion_namespaces_destroy_namespace_node(_list->root);
	}
}


// ============================================================================
//! Find namespace node in namespaces.
// ----------------------------------------------------------------------------
inline extern struct passion_namespace_node* passion_namespaces_find(struct passion_namespaces* _list, amtrs_string_view _find)
{
	if (!_list)
	{
		return	passion_null;
	}
	else
	{
		struct passion_namespace_node*	current	= _list->root;
		size_t							hash	= amtrs_string_view_compute_hash(_find);
		while (current)
		{
			if (hash == current->hash)
			{
				if (amtrs_string_view_equals(_find, amtrs_string_to_string_view(current->text)))
				{
					return	current;
				}
			}
			current	= (hash < current->hash)
					? current->left
					: current->right;
		}
		return	passion_null;
	}
}



// ============================================================================
//! Find namespace. Join namespace.
// ----------------------------------------------------------------------------
inline extern struct passion_namespace_node* passion_namespaces_find_joined(struct passion_namespaces* _list, amtrs_string_view _parent_namespace, amtrs_string_view _namespace)
{
	if (!_list)
	{
		return	passion_null;
	}

	if (_parent_namespace.len == 0)
	{
		return	passion_namespaces_find(_list, _namespace);
	}
	if (_namespace.len == 0)
	{
		return	passion_namespaces_find(_list, _parent_namespace);
	}

	struct passion_namespace_node*	current		= _list->root;
	size_t							totalSize	= _parent_namespace.len + 2 + _namespace.len;
	size_t							hash		= 0;
	hash	= amtrs_string_view_compute_hash_continue(hash, _parent_namespace);
	hash	= amtrs_string_view_compute_hash_continue(hash, amtrs_string_view_make("::"));
	hash	= amtrs_string_view_compute_hash_continue(hash, _namespace);
	while (current)
	{
		if (hash == current->hash && current->text->size == totalSize)
		{
			amtrs_string_view	text	= amtrs_string_to_string_view(current->text);
			if (amtrs_string_view_starts_with(text, _parent_namespace))
			{
				text.txt	+= _parent_namespace.len;
				text.len	-= _parent_namespace.len;
				if (amtrs_string_view_starts_with(text, amtrs_string_view_make("::")))
				{
					text.txt	+= 2;
					text.len	-= 2;
					if (amtrs_string_view_equals(text, _namespace))
					{
						return	current;
					}
				}
			}
		}
		current	= (hash < current->hash)
				? current->left
				: current->right;
	}
	return	passion_null;
}




// ============================================================================
//! Find namespace. by current namespace.
// ----------------------------------------------------------------------------
inline extern struct passion_namespace_node* passion_namespaces_find_by_current_namespace(struct passion_namespaces* _list, amtrs_string_view _current_namespace, amtrs_string_view _namespace)
{
	if (!_list)
	{
		return	passion_null;
	}

	amtrs_string_view	find	= passion_namespace_normalize_namespace(_namespace);
	if (_namespace.len >= 2 && amtrs_string_view_starts_with(_namespace, amtrs_string_view_make("::")))
	{
		return	passion_namespaces_find(_list, find);
	}

	amtrs_string_view	current	= passion_namespace_normalize_namespace(_current_namespace);
	for (;;)
	{
		struct passion_namespace_node*	retval	= passion_namespaces_find_joined(_list, current, find);
		if (retval)
		{
			return	retval;
		}
		if (current.len == 0)
		{
			return	passion_null;
		}
		current	= passion_namespace_parent_namespace(current);
	}
}



// ============================================================================
//! Find namespace node in namespaces.
// ----------------------------------------------------------------------------
inline extern passion_bool passion_namespaces_has(struct passion_namespaces* _list, amtrs_string_view _find)
{
	return	passion_namespaces_find(_list, _find)
			? passion_true
			: passion_false;
}



// ============================================================================
//! Add namespace.
// ----------------------------------------------------------------------------
inline extern struct passion_namespace_node* passion_namespaces_add(struct passion_namespaces* _list, amtrs_string_view _add)
{
	amtrs_string_view		add		= passion_namespace_normalize_namespace(_add);
	if (_list && add.len > 0)
	{
		amtrs_string_view		work;
		struct passion_namespace_node**	current	= &_list->root;
		size_t							hash	= amtrs_string_view_compute_hash(add);
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

		*current	= (struct passion_namespace_node*)malloc(sizeof(struct passion_namespace_node));
		(*current)->text	= amtrs_string_create_from_string_view(add);
		(*current)->hash	= hash;
		(*current)->left	= passion_null;
		(*current)->right	= passion_null;

		work	= passion_namespace_parent_namespace(add);
		if (work.len > 0)
		{
			passion_namespaces_add(_list, work);
		}

		return	(*current);
	}
	return	passion_null;
}


// ============================================================================
//! Enumlare namespaces
// ----------------------------------------------------------------------------
inline extern size_t passion_namespaces_enumlate(struct passion_namespaces* _list, passion_bool(*_callback)(struct passion_namespace_node*, void*), void* _userdata)
{
	if (_list && _list->root)
	{
		return	__passion_namespaces_enumlate(_list->root, _callback, _userdata);
	}
	return	0;
}







inline extern size_t __passion_namespaces_enumlate(struct passion_namespace_node* _node, passion_bool(*_callback)(struct passion_namespace_node*, void*), void* _userdata)
{
	size_t	retval	= 1;
	_callback(_node, _userdata);
	if (_node->left)
	{
		retval += __passion_namespaces_enumlate(_node->left, _callback, _userdata);
	}
	if (_node->right)
	{
		retval += __passion_namespaces_enumlate(_node->right, _callback, _userdata);
	}
	return	retval;
}


PASSION_EXTERN_C_END
#endif

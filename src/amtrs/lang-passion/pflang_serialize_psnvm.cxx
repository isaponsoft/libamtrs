/* Copyright (c) 2022, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
/*
 * 汎用ライブラリ
 * OSや環境問わずに使用できる。
 */
#if	0
using namespace amtrs;
using namespace psnvm;
using namespace psnvm::lang;
PSNVM_LANG_NAMESPACE_BEGIN

namespace {
struct	serializer
{
	bool		statement_init	= false;
	pflnode_types	last_type		= (pflnode_types)~0;

	bool term()
	{
		return	!(last_type == pflnode_types::stmt_compound
				||last_type == pflnode_types::stmt_for || last_type == pflnode_types::stmt_if
				||last_type == pflnode_types::stmt_for_rangedbase || last_type == pflnode_types::stmt_switch
				||last_type == pflnode_types::declare_function_conversion
				||last_type == pflnode_types::declare_function_constractor
				||last_type == pflnode_types::declare_function
				||last_type == pflnode_types::declare_function_operator
				||last_type == pflnode_types::declare_variable
				||last_type == pflnode_types::label
		);
	}


	void _serialize_psnvm(pfl& _fplang, pflnode& _node, typeutil::callback<void(int, text)> _callback, int _indent)
	{
		int		count	= 0;
		if (!_node) { return; }

		switch (_node.type())
		{
			default :
			{
				auto	s	= format("Unkown pflnode type = {}", {(int)_node.type()});
				_callback(0, s);
				break;
			}

			case pflnode_types::operator_bin :
			{
				auto	n	= _node.at<pn_operator_bin>();
				_callback(_indent, "(");
				_serialize_psnvm(_fplang, n->left, _callback, 0);
				_callback(0, " ");
				_callback(0, get_operator_string(n->ope));
				_callback(0, " ");
				_serialize_psnvm(_fplang, n->right, _callback, 0);
				_callback(0, ")");
				break;
			}

			case pflnode_types::operator_call :
			{
				auto	n	= _node.at<pn_operator_call>();
				_serialize_psnvm(_fplang, n->function, _callback, _indent);
				_callback(0, "(");
				for (auto& p : n->arguments)
				{
					if (count++ > 0)	_callback(0, ", ");
					_serialize_psnvm(_fplang, p, _callback, 0);
				}
				_callback(0, ")");
				break;
			}

			case pflnode_types::stmt_compound :
			{
				auto	n	= _node.at<pn_stmt_compound>();
				_callback(_indent, "{\n");
				for (auto& s : n->statements)
				{
					_serialize_psnvm(_fplang, s, _callback, _indent + 1);
					if (term())
					{
						_callback(0, ";\n");
					}
				}
				_callback(_indent, "}\n");
				break;
			}

			case pflnode_types::declare_class :
			{
				auto	n	= _node.at<pn_declare_class>();
				_callback(_indent, "class ");
				_serialize_psnvm(_fplang, n->name, _callback, 0);
				if (n->members.size() > 0)
				{
					_callback(0, "\n");
					_callback(_indent, "{\n");
					for (auto& m : n->members)
					{
						_serialize_psnvm(_fplang, m, _callback, _indent+1);
						if (term())
						{
							_callback(0, ";");
						}
						_callback(0, "\n");
					}
					_callback(_indent, "};\n");
				}
				break;
			}

			case pflnode_types::declare_function_constractor :
			{
				auto	n	= _node.at<pn_declare_function_constractor>();
				_serialize_psnvm(_fplang, n->name, _callback, _indent);
				_callback(0, "(");
				_serialize_psnvm(_fplang, n->parameters, _callback, 0);
				if (!n->codes)	_callback(0, ")");
				else			_callback(0, ")\n");
				if (n->initializers.size() > 0)
				{
					count	= 0;
					for (auto& i : n->initializers)
					{
						_callback(_indent + 1, count++ == 0 ? ": " : ", ");
						_serialize_psnvm(_fplang, i.name, _callback, 0);
						_callback(0, "(");
						_serialize_psnvm(_fplang, i.init, _callback, 0);
						_callback(0, ")\n");
					}
				}
				_serialize_psnvm(_fplang, n->codes, _callback, _indent);
				break;
			}

			case pflnode_types::declare_function :
			{
				auto	n	= _node.at<pn_declare_function>();
				_callback(_indent, "auto ");
				_serialize_psnvm(_fplang, n->name, _callback, 0);
				_callback(0, "(");
				_serialize_psnvm(_fplang, n->parameters, _callback, 0);
				_callback(0, ") -> ");
				_serialize_psnvm(_fplang, n->rettype, _callback, 0);
				if (!n->codes)	_callback(0, ";");
				else			_callback(0, "\n");
				_serialize_psnvm(_fplang, n->codes, _callback, _indent);
				break;
			}

			case pflnode_types::declare_function_conversion :
			{
				auto	n	= _node.at<pn_declare_function_conversion>();
				_callback(_indent, "operator ");
				_serialize_psnvm(_fplang, n->rettype, _callback, 0);
				_callback(0, " ()");
				if (!n->codes)	_callback(0, "");
				else			_callback(0, "\n");
				_serialize_psnvm(_fplang, n->codes, _callback, _indent);
				break;
			}

			case pflnode_types::declare_namespace :
			{
				auto	n	= _node.at<pn_declare_namespace>();
				_callback(_indent, "namespace ");
				_serialize_psnvm(_fplang, n->name, _callback, _indent);
				_callback(0, "\n");
				_callback(_indent, "{\n");
				for (auto& s : n->statements)
				{
					_serialize_psnvm(_fplang, s, _callback, _indent + 1);
				}
				_callback(_indent, "}\n\n");
				break;
			}

			case pflnode_types::declare_function_operator :
			{
				auto	n	= _node.at<pn_declare_function_operator>();
				_callback(_indent, "auto operator ");
				_callback(0, get_operator_string(n->ope));
				_callback(0, " (");
				_serialize_psnvm(_fplang, n->parameters, _callback, 0);
				_callback(0, ") -> ");
				_serialize_psnvm(_fplang, n->rettype, _callback, 0);
				if (!n->codes)	_callback(0, "");
				else			_callback(0, "\n");
				_serialize_psnvm(_fplang, n->codes, _callback, _indent);
				break;
			}

			case pflnode_types::declare_template :
			{
				auto	n		= _node.at<pn_declare_template>();
				_callback(_indent, "template<");
				count	= 0;
				for (auto& p : n->parameters)
				{
					if (count++ > 0) _callback(0, ", ");
					_serialize_psnvm(_fplang, p.first, _callback, 0);
					if (p.second)
					{
						_callback(0, " ");
						_serialize_psnvm(_fplang, p.second, _callback, 0);
					}
					if (p.init)
					{
						_callback(0, " = ");
						_serialize_psnvm(_fplang, p.init, _callback, 0);
					}
				}
				_callback(0, ">\n");
				_serialize_psnvm(_fplang, n->declare, _callback, _indent);
				return;
			}

			case pflnode_types::declare_variable :
			{
				auto	n	= _node.at<pn_declare_variable>();
				_serialize_psnvm(_fplang, n->typesym, _callback, _indent);
				if (n->values.size() > 0)
				{
					int	count	= 0;
					for (auto& v : n->values)
					{
						if (count++ > 0) { _callback(0, ","); }
						_callback(0, " ");
						_serialize_psnvm(_fplang, v.name, _callback, 0);
						if (v.init)
						{
							_callback(0, " = ");
							_serialize_psnvm(_fplang, v.init, _callback, 0);
						}
					}
				}
				if (!statement_init)
				{
					_callback(0, ";\n");
				}
				break;
			}

			case pflnode_types::label :
			{
				auto	n	= _node.at<pn_label>();
				_callback(_indent - 1, n->name);
				_callback(0, " :\n");
				break;
			}

			case pflnode_types::literal_bool :
			{
				auto	n	= _node.at<pn_literal_bool>();
				_callback(0, n->value ? "true" : "false");
				break;
			}

			case pflnode_types::literal_integer :
			{
				auto	str	= format("{}", {_node.at<pn_literal_integer>()->integer});
				_callback(0, str);
				break;
			}

			case pflnode_types::literal_nullptr :
			{
				_callback(0, "nullptr");
				break;
			}

			case pflnode_types::literal_string :
			{
				auto	n	= _node.at<pn_literal_string>();
				_callback(_indent, "\"");
				_callback(0, n->str);
				_callback(0, "\"");
				break;
			}

			case pflnode_types::initializer_list :
			{
				auto	n	= _node.at<pn_initializer_list>();
				_callback(_indent, "{");
				count	= 0;
				for (auto& v : n->values)
				{
					if (count++ > 0) _callback(0, ", ");
					if (!v.name.empty())
					{
						_callback(0, ".");
						_callback(0, v.name);
						_callback(0, " = ");
					}
					_serialize_psnvm(_fplang, v.value, _callback, 0);
				}
				_callback(0, "}");
				break;
			}


			case pflnode_types::operator_member_access :
			{
				auto	n	= _node.at<pn_operator_member_access>();
				_serialize_psnvm(_fplang, n->value, _callback, 0);
				_callback(0, ".");
				_serialize_psnvm(_fplang, n->member, _callback, 0);
				break;
			}

			case pflnode_types::operator_member_access_arrow :
			{
				auto	n	= _node.at<pn_operator_member_access_arrow>();
				_serialize_psnvm(_fplang, n->value, _callback, 0);
				_callback(0, "->");
				_serialize_psnvm(_fplang, n->member, _callback, 0);
				break;
			}

			case pflnode_types::spec_symbol_move :
			{
				auto	n	= _node.at<pn_spec_symbol_move>();
				_serialize_psnvm(_fplang, n->name, _callback, _indent);
				_callback(_indent, "&&");
				break;
			}

			case pflnode_types::parameter_list :
			{
				auto	n	= _node.at<pn_parameter_list>();
				count	= 0;
				for (auto& p : n->parameters)
				{
					if (count++ > 0) _callback(0, ", ");
					_serialize_psnvm(_fplang, p.first, _callback, 0);
					if (p.second)
					{
						_callback(0, " ");
						_serialize_psnvm(_fplang, p.second, _callback, 0);
					}
					if (p.init)
					{
						_callback(0, " = ");
						_serialize_psnvm(_fplang, p.init, _callback, 0);
					}
				}
				break;
			}

			case pflnode_types::spec_symbol_parampack :
			{
				auto	n	= _node.at<pn_spec_symbol_parampack>();
				_serialize_psnvm(_fplang, n->name, _callback, _indent);
				_callback(_indent, "...");
				break;
			}

			case pflnode_types::stmt_return :
			{
				auto	n	= _node.at<pn_stmt_return>();
				_callback(_indent, "return ");
				_serialize_psnvm(_fplang, n->value, _callback, 0);
				break;
			}

			case pflnode_types::operator_single :
			{
				auto	n	= _node.at<pn_operator_single>();
				_callback(_indent, get_operator_string(n->ope));
				_serialize_psnvm(_fplang, n->value, _callback, 0);
				break;
			}

			case pflnode_types::spec_member_friend :
			{
				auto	n	= _node.at<pn_spec_member_friend>();
				_callback(_indent, "friend ");
				_serialize_psnvm(_fplang, n->decl, _callback, 0);
				break;
			}

			case pflnode_types::stmt_for :
			{
				auto	n	= _node.at<pn_stmt_for>();
				_callback(_indent, "for (");
				statement_init	= true;
				_serialize_psnvm(_fplang, n->init, _callback, 0);
				_callback(0, "; ");
				statement_init	= false;
				_serialize_psnvm(_fplang, n->conditions, _callback, 0);
				_callback(0, "; ");
				_serialize_psnvm(_fplang, n->increments, _callback, 0);
				_callback(0, ")\n");
				if (n->statements)
				{
					_serialize_psnvm(_fplang, n->statements, _callback, _indent);
				}
				else
				{
					_callback(0, ";");
				}
				break;
			}

			case pflnode_types::stmt_if :
			{
				auto	n	= _node.at<pn_stmt_if>();
				_callback(_indent, "if (");
				if (n->init)
				{
					statement_init	= true;
					_serialize_psnvm(_fplang, n->init, _callback, 0);
					statement_init	= false;
					_callback(0, "; ");
				}
				_serialize_psnvm(_fplang, n->conditions, _callback, 0);
				_callback(0, ")\n");
				if (n->statements1)
				{
					_serialize_psnvm(_fplang, n->statements1, _callback, _indent);
				}
				else
				{
					_callback(0, ";");
				}
				if (n->statements2)
				{
					_callback(_indent, "else\n");
					_serialize_psnvm(_fplang, n->statements2, _callback, _indent);
				}

				break;
			}

			case pflnode_types::stmt_switch :
			{
				auto	n	= _node.at<pn_stmt_switch>();
				_callback(_indent, "switch (");
				_serialize_psnvm(_fplang, n->conditions, _callback, 0);
				_callback(0, ")\n");
				_callback(_indent, "{\n");
				for (auto& c : n->cases)
				{
					_callback(_indent+1, "case ");
					_serialize_psnvm(_fplang, c.value, _callback, 0);
					_callback(0, " :\n");
					_serialize_psnvm(_fplang, c.statements, _callback, _indent+1);
				}
				_callback(_indent, "}\n");
				break;
			}

			case pflnode_types::symbol_scope :
			{
				auto	n	= _node.at<pn_symbol_scope>();
				for (auto& s : n->scopes)
				{
					if (count++ > 0) _callback(0, "::");
					_serialize_psnvm(_fplang, s, _callback, 0);
				}
				break;
			}

			case pflnode_types::struct_initializer :
			{
				auto	n	= _node.at<pn_struct_initializer>();
				_serialize_psnvm(_fplang, n->name, _callback, _indent);
				_serialize_psnvm(_fplang, n->initlist, _callback, 0);
				break;
			}


			case pflnode_types::operator_subscriptl :
			{
				auto	n	= _node.at<pn_operator_subscriptl>();
				_serialize_psnvm(_fplang, n->value, _callback, _indent);
				_callback(0, "[");
				_serialize_psnvm(_fplang, n->idx, _callback, 0);
				_callback(0, "]");
				break;
			}

			case pflnode_types::symbol_name :
			{
				auto	n	= _node.at<pn_symbol_name>();
				_callback(_indent, n->name);
				break;
			}

			case pflnode_types::symbol_template :
			{
				auto	n	= _node.at<pn_symbol_template>();
				_callback(_indent, n->name);
				_callback(0, "<");
				count	= 0;
				for (auto& p : n->templateParams)
				{
					if (count++ > 0)	_callback(0, ", ");
					_serialize_psnvm(_fplang, p, _callback, 0);
				}
				_callback(0, ">");
				break;
			}

			case pflnode_types::spec_symbol_reference :
			{
				auto	n	= _node.at<pn_spec_symbol_reference>();
				_serialize_psnvm(_fplang, n->name, _callback, _indent);
				_callback(0, "&");
				break;
			}

			case pflnode_types::sepc_symbol_pointer :
			{
				auto	n	= _node.at<pn_spec_symbol_pointer>();
				_serialize_psnvm(_fplang, n->name, _callback, _indent);
				_callback(0, "*");
				break;
			}

			case pflnode_types::using_alias :
			{
				auto	n	= _node.at<pn_using_alias>();
				_callback(_indent, "using ");
				_serialize_psnvm(_fplang, n->name, _callback, 0);
				_callback(_indent, " = ");
				_serialize_psnvm(_fplang, n->target, _callback, 0);
				_callback(0, "*");
				break;
			}

		}
		last_type	= _node.type();
	}
};
} // namespace 

void pfl::serialize_psnvm(pflnode& _node, typeutil::callback<void(text)> _callback)
{
	serializer	s;
	s._serialize_psnvm(*this, _node, [&](int indent, text str)
	{
		for (int c = 0; c < indent; ++c)
		{
			_callback("    ");
		}
		_callback(str);
	}, 0);
}
PSNVM_LANG_NAMESPACE_END
#endif

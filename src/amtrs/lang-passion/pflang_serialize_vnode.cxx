/* Copyright (c) 2022, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
/*
 * 汎用ライブラリ
 * OSや環境問わずに使用できる。
 */
#include <iostream>

using namespace amtrs;
using namespace psnvm;
using namespace psnvm::lang;
PSNVM_LANG_NAMESPACE_BEGIN
struct	vnode_serialer
{
	typeutil::callback<void(text)> callback;
	void serialize(pvlnode& _node)
	{
		int	count;
		switch (_node.type())
		{
			default:
				AMTRS_FORCE_LOG("Unkown pvlnode serialize pvlnode type = {}", (int)_node.type());
				break;

			case vn_imm_integer::type :
			{
				auto	n	= _node.at<vn_imm_integer>();
				out << "(int " << n->value << ")";
				break;
			}

			case vn_imm_sizeof::type :
			{
				auto	n	= _node.at<vn_imm_sizeof>();
				out << "(sizeof " << n->t.name() << ")";
				break;
			}

			case vn_bin_operation_int::type :
			{
				auto	n	= _node.at<vn_bin_operation_int>();
				out << "(bin " << get_operator_string(n->operation) << " " << n->v1 << " " << n->v2 << ")";
				break;
			}

			case vn_expr_call::type :
			{
				auto	n	= _node.at<vn_expr_call>();
				out << "(call `" << n->func->name << "` `" << n->func->signature << "`";
				for (auto& a : n->arguments)
				{
					out << " " << a;
				}
				out << ")";
				break;
			}

			case vn_expr_new::type :
			{
				auto	n	= _node.at<vn_expr_new>();
				out << "(new " << n->allocate << ")";
				break;
			}

			case vn_expr_vcall::type :
			{
				auto	n	= _node.at<vn_expr_vcall>();
				out << "(vcall `" << n->func->name << "` `" << n->func->signature << "`";
				for (auto& a : n->arguments)
				{
					out << " " << a;
				}
				out << ")";
				break;
			}

			case vn_list::type :
			{
				auto	n	= _node.at<vn_list>();
				out << "(list";
				for (auto& v : n->values)
				{
					out << " " << v;
				}
				out << ")";
				break;
			}

			case vn_load_integral::type :
			{
				auto	n	= _node.at<vn_load_integral>();
				out << "(loadint " << n->ref << ")";
				break;
			}

			case vn_return::type :
			{
				out << "(return)";
				break;
			}

			case vn_returnval::type :
			{
				auto	n	= _node.at<vn_returnval>();
				out << "(returnval";
				if (!n->expr.empty())
				{
					out << " " << n->expr;
				}
				out << ")";
				break;
			}

			case vn_typename::type :
			{
				auto	n	= _node.at<vn_typename>();
				out << "(typename " << (text)_node.datatype().name() << ")";
				break;
			}

			case vn_compound::type :
			{
				auto	n	= _node.at<vn_compound>();
				int	count	= 0;
				for (auto& s : n->statements)
				{
					if (count++ > 0) out << " ";
					out << s;
				}
				break;
			}

			case vn_address_argvref::type :
			{
				auto	n	= _node.at<vn_address_argvref>();
				out << "(&a[" << n->index << "])";
				break;
			}

			case vn_address_localref::type :
			{
				auto	n	= _node.at<vn_address_localref>();
				out << "(&l[" << n->index << "])";
				break;
			}

			case vn_address_cast::type :
			{
				auto		n	= _node.at<vn_address_cast>();
				out << "(cast " << n->value << " `" << n->to->name << "` `" << n->from->name << "`)";
				break;
			}

			case vn_address_memberref::type :
			{
				auto		n	= _node.at<vn_address_memberref>();
				out << "(&m " << n->obj << " " << (text)n->thiscls->variables[n->index_base_of].name << ")";
				break;
			}

			case vn_local_allocate::type :
			{
				auto	n	= _node.at<vn_local_allocate>();
				out << "(localalloc";
				count	= 0;
				for (auto& v : n->variables)
				{
					out << " (" << v->datatype.name() << " " << v->name << ")";
				}
				out << ")";
				for (auto& v : n->variables)
				{
					if (v->init)
					{
						out << " " << v->init;
					}
				}
				break;
			}

			case vn_local_free::type :
			{
				auto	n	= _node.at<vn_local_free>();
				for (auto& v : n->variables)
				{
					if (v->destroy)
					{
						out << " " << v->destroy;
					}
				}
				out << "(localfree";
				count	= 0;
				for (auto& v : n->variables)
				{
					out << " (" << v->datatype.name() << " " << v->name << ")";
				}
				out << ")";
				break;
			}

			case vn_assign::type :
			{
				auto	n	= _node.at<vn_assign>();
				out << "(assign " << n->dest << " " << n->src << ")";
				break;
			}

			case vn_operation_single_int::type :
			{
				auto	n	= _node.at<vn_operation_single_int>();
				out << "(single " << get_operator_string(n->operation) << " " << n->val << ")";
				break;
			}

			case vn_stmt_delete::type :
			{
				auto	n	= _node.at<vn_stmt_delete>();
				out << "(delete " << n->value << ")";
				break;
			}

			case vn_stmt_for::type :
			{
				auto	n	= _node.at<vn_stmt_for>();
				out << "(for " << n->conditions << " " << n->increment << " " << n->statement << ")";
				break;
			}

			case vn_stmt_if::type :
			{
				auto	n	= _node.at<vn_stmt_if>();
				out << "(if " << n->conditions << " " << n->statement1 << " " << n->statement2 << ")";
				break;
			}

			case vn_this::type :
			{
				out << "(this)";
				break;
			}

			case vn_vtable::type :
			{
				auto	n	= _node.at<vn_vtable>();
				out << "(vtbl `" << n->cls->name << "` " << n->offset << ")";
				break;
			}

		}
	}

	struct	output
	{
		vnode_serialer*	thiz;
		output& operator << (pvlnode& _n)  { thiz->serialize(_n); return *this; }
		output& operator << (intmax_t _n)
		{
			*this << format("{d}", {_n});
			return	*this;
		}
		output& operator << (text  _t)
		{
			thiz->callback(_t);
			return	*this;
		}
	};

	output							out{this};
};



void serialize(pvlnode& _node, typeutil::callback<void(text)> _callback)
{
	vnode_serialer	s;
	s.callback	= std::move(_callback);
	s.serialize(_node);
}
PSNVM_LANG_NAMESPACE_END

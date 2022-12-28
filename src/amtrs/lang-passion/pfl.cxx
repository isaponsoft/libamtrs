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
struct	pfl_core
{
	pfl*							fp;
	typeutil::callback<void(text)>	callback;


	void serialize(pflnode& _node)
	{
		int	count	= 0;
		switch (_node.type())
		{
			case pn_parameter_list::type :
			{
				auto	n	= _node.at<pn_parameter_list>();
				out << "(parameter_list";
				for (auto& p : n->parameters)
				{
					out << " (" << p.first;
					if (p.second)
					{
						out << " (name " << p.second << ")";
					}
					if (p.init)
					{
						out << " (default " << p.init << ")";
					}
					out << ")";
				}
				out << ")";
				break;
			}

			case pn_declare_class::type :
			{
				auto	n	= _node.at<pn_declare_class>();
				out << "(class " << n->name << nest_in;
				if (n->extends.size() > 0)
				{
					out << "(extends ";
					count	= 0;
					for (auto& e : n->extends)
					{
						if (count++ > 0) out << " ";
						int	count2	= 0;
						out << "(";
						if (e.access & (uint32_t)pn_declare_specifier::spec_virtual)	out << (count2++ > 0 ? " " : "") << "virtual";
						if (e.access & (uint32_t)pn_declare_specifier::spec_public)		out << (count2++ > 0 ? " " : "") << "public";
						if (e.access & (uint32_t)pn_declare_specifier::spec_protected)	out << (count2++ > 0 ? " " : "") << "protected";
						if (e.access & (uint32_t)pn_declare_specifier::spec_private)	out << (count2++ > 0 ? " " : "") << "private";
						out << (count2++ > 0 ? " " : "") << e.name << ")";
					}
					out << ")" << ret;
				}
				if (n->members.size() > 0)
				{
					for (auto& m : n->members)
					{
						out << m << ret;
					}
				}
				out << nest_out << ")";
				break;
			}

			case pn_declare_concept::type :
			{
				auto	n	= _node.at<pn_declare_concept>();
				out << "(concept " << n->name << " " << n->expr << ")";
				break;
			}

			case pn_declare_enum::type :
			{
				auto	n	= _node.at<pn_declare_enum>();
				out << "(enum";
				if (n->name) out << " " << n->name;
				if (n->base)
				{
					out << " " << n->base;
				}
				if (n->values.size() > 0)
				{
					out << " [";
					for (auto& v : n->values)
					{
						if (count++ > 0) out << " ";
						out << "(`" << v.name<<"`";
						if (v.value)
						{
							out << " " << v.value;
						}
						out << ")";
					}
					out << "]";
				}
				out << ")";
				break;
			}

			case pn_declare_function::type :
			{
				auto	n	= _node.at<pn_declare_function>();
				out << "(func " << n->name;
				if (n->specifier & ~(uint32_t)pn_declare_specifier::spec_public)
				{
					int	count2	= 0;
					out << " [";
					if (n->specifier & (uint32_t)pn_declare_specifier::spec_const) 			out << (count2++ > 0 ? " " : "") << "const";
					if (n->specifier & (uint32_t)pn_declare_specifier::spec_volatile)		out << (count2++ > 0 ? " " : "") << "volatile";
					if (n->specifier & (uint32_t)pn_declare_specifier::spec_noexcept)		out << (count2++ > 0 ? " " : "") << "noexcept";
					if (n->specifier & (uint32_t)pn_declare_specifier::spec_virt_override)	out << (count2++ > 0 ? " " : "") << "override";
					if (n->specifier & (uint32_t)pn_declare_specifier::spec_virt_final)		out << (count2++ > 0 ? " " : "") << "final";
					if (n->specifier & (uint32_t)pn_declare_specifier::spec_default)		out << (count2++ > 0 ? " " : "") << "default";
					out << "]";
				}
				out << nest_in << n->rettype << " " << n->parameters << ret;
				if (n->codes)
				{
					out << " " << n->codes;
				}
				out << nest_out << ")";
				break;
			}


			case pn_declare_function_constractor::type :
			{
				auto	n	= _node.at<pn_declare_function_constractor>();
				out << "(func constractor " << n->name;
				if (n->specifier & ~(uint32_t)pn_declare_specifier::spec_public)
				{
					int	count2	= 0;
					out << " [";
					if (n->specifier & (uint32_t)pn_declare_specifier::spec_const) 			out << (count2++ > 0 ? " " : "") << "const";
					if (n->specifier & (uint32_t)pn_declare_specifier::spec_volatile)		out << (count2++ > 0 ? " " : "") << "volatile";
					if (n->specifier & (uint32_t)pn_declare_specifier::spec_noexcept)		out << (count2++ > 0 ? " " : "") << "noexcept";
					if (n->specifier & (uint32_t)pn_declare_specifier::spec_virt_override)	out << (count2++ > 0 ? " " : "") << "override";
					if (n->specifier & (uint32_t)pn_declare_specifier::spec_virt_final)		out << (count2++ > 0 ? " " : "") << "final";
					if (n->specifier & (uint32_t)pn_declare_specifier::spec_default)			out << (count2++ > 0 ? " " : "") << "default";
					out << "]";
				}
				out << nest_in << n->parameters << ret;
				if (n->initializers.size() > 0)
				{
					out << "(initializers ";
					for (auto& i : n->initializers)
					{
						out << " (" << i.name;
						for (auto& v : i.init)
						{
							out << " " << v;
						}
						out << ")";
					}
					out << ")" << ret;
				}
				if (n->codes)
				{
					out << n->codes;
				}
				out << nest_out << ")";
				break;
			}

			case pn_declare_function_conversion::type :
			{
				auto	n	= _node.at<pn_declare_function_conversion>();
				out << "(func conversion ";
				if (n->specifier & ~(uint32_t)pn_declare_specifier::spec_public)
				{
					int	count2	= 0;
					out << " [";
					if (n->specifier & (uint32_t)pn_declare_specifier::spec_const) 			out << (count2++ > 0 ? " " : "") << "const";
					if (n->specifier & (uint32_t)pn_declare_specifier::spec_volatile)		out << (count2++ > 0 ? " " : "") << "volatile";
					if (n->specifier & (uint32_t)pn_declare_specifier::spec_noexcept)		out << (count2++ > 0 ? " " : "") << "noexcept";
					if (n->specifier & (uint32_t)pn_declare_specifier::spec_virt_override)	out << (count2++ > 0 ? " " : "") << "override";
					if (n->specifier & (uint32_t)pn_declare_specifier::spec_virt_final)		out << (count2++ > 0 ? " " : "") << "final";
					if (n->specifier & (uint32_t)pn_declare_specifier::spec_default)			out << (count2++ > 0 ? " " : "") << "default";
					out << "]";
				}
				out << nest_in << n->rettype << " " << n->parameters << ret;
				if (n->codes)
				{
					out << " " << n->codes;
				}
				out << nest_out << ")";
				break;
			}


			case pn_declare_function_destractor::type :
			{
				auto	n	= _node.at<pn_declare_function_destractor>();
				out << "(func destractor " << n->name;
				if (n->specifier & ~(uint32_t)pn_declare_specifier::spec_public)
				{
					int	count2	= 0;
					out << " [";
					if (n->specifier & (uint32_t)pn_declare_specifier::spec_const) 			out << (count2++ > 0 ? " " : "") << "const";
					if (n->specifier & (uint32_t)pn_declare_specifier::spec_volatile)		out << (count2++ > 0 ? " " : "") << "volatile";
					if (n->specifier & (uint32_t)pn_declare_specifier::spec_noexcept)		out << (count2++ > 0 ? " " : "") << "noexcept";
					if (n->specifier & (uint32_t)pn_declare_specifier::spec_virt_override)	out << (count2++ > 0 ? " " : "") << "override";
					if (n->specifier & (uint32_t)pn_declare_specifier::spec_virt_final)		out << (count2++ > 0 ? " " : "") << "final";
					if (n->specifier & (uint32_t)pn_declare_specifier::spec_default)			out << (count2++ > 0 ? " " : "") << "default";
					out << "]";
				}
				out << nest_in << n->parameters << ret;
				if (n->codes)
				{
					out << n->codes;
				}
				out << nest_out << ")";
				break;
			}


			case pn_declare_function_operator::type :
			{
				auto	n	= _node.at<pn_declare_function_operator>();
				out << "(func operator `" << get_operator_string(n->ope) << "`";
				if (n->specifier & ~(uint32_t)pn_declare_specifier::spec_public)
				{
					int	count2	= 0;
					out << " [";
					if (n->specifier & (uint32_t)pn_declare_specifier::spec_const) 			out << (count2++ > 0 ? " " : "") << "const";
					if (n->specifier & (uint32_t)pn_declare_specifier::spec_volatile)		out << (count2++ > 0 ? " " : "") << "volatile";
					if (n->specifier & (uint32_t)pn_declare_specifier::spec_noexcept)		out << (count2++ > 0 ? " " : "") << "noexcept";
					if (n->specifier & (uint32_t)pn_declare_specifier::spec_virt_override)	out << (count2++ > 0 ? " " : "") << "override";
					if (n->specifier & (uint32_t)pn_declare_specifier::spec_virt_final)		out << (count2++ > 0 ? " " : "") << "final";
					if (n->specifier & (uint32_t)pn_declare_specifier::spec_default)			out << (count2++ > 0 ? " " : "") << "default";
					out << "]";
				}
				out << " " << n->rettype << " " << n->parameters << ret;
				if (n->codes)
				{
					out << " " << n->codes;
				}
				out << ")";
				break;
			}

			case pn_declare_namespace::type :
			{
				auto	n	= _node.at<pn_declare_namespace>();
				out << "(namespace";
				if (n->name)
				{
					out << " " << n->name;
				}
				out << nest_in;
				if (n->statements.size() > 0)
				{
					for (auto& s : n->statements)
					{
						out << s << ret;
					}
				}
				out << nest_out << ")";
				break;
			}

			case pn_declare_template::type :
			{
				auto	n	= _node.at<pn_declare_template>();
				out << "(template (parameter_list ";
				for (auto& p : n->parameters)
				{
					out << "(" << p.first;
					if (p.second)
					{
						out << " (name " << p.second << ")";
					}
					if (p.init)
					{
						out << " (default " << p.init << ")";
					}
					out << ")";
				}
				out << ")" << nest_in;
				out << n->declare;
				out << nest_out << ")";
				break;
			}

			case pn_declare_variable::type :
			{
				auto	n	= _node.at<pn_declare_variable>();
				out << "(var " << n->typesym;
				if (n->specifier)
				{
					int	count2	= 0;
					out << " [";
					if (n->specifier & (uint32_t)pn_declare_specifier::spec_const) 			out << (count2++ > 0 ? " " : "") << "const";
					if (n->specifier & (uint32_t)pn_declare_specifier::spec_volatile)		out << (count2++ > 0 ? " " : "") << "volatile";
					if (n->specifier & (uint32_t)pn_declare_specifier::spec_noexcept)		out << (count2++ > 0 ? " " : "") << "noexcept";
					if (n->specifier & (uint32_t)pn_declare_specifier::spec_virt_override)	out << (count2++ > 0 ? " " : "") << "override";
					if (n->specifier & (uint32_t)pn_declare_specifier::spec_virt_final)		out << (count2++ > 0 ? " " : "") << "final";
					if (n->specifier & (uint32_t)pn_declare_specifier::spec_default)			out << (count2++ > 0 ? " " : "") << "default";
					out << "]";
				}
				out << " [";
				for (auto& v : n->values)
				{
					if (count++ > 0) out << " ";
					out << "(" << v.name;
					if (v.init)
					{
						out << " " << v.init;
					}
					out << ")";
				}
				out << "])";
				break;
			}


			case pn_expr_cast::type :
			{
				auto	n	= _node.at<pn_expr_cast>();
				out << "(cast ";
				if (n->casttype == pn_expr_cast::cast_types::cast_ctype)		out << "c";
				if (n->casttype == pn_expr_cast::cast_types::cast_const)		out << "const";
				if (n->casttype == pn_expr_cast::cast_types::cast_reinterpret)	out << "reinterpret";
				if (n->casttype == pn_expr_cast::cast_types::cast_static)		out << "static";
				out << " " << n->totype << " " << n->value << ")";
				break;
			}

			case pn_expr_lambda::type :
			{
				auto	n	= _node.at<pn_expr_lambda>();
				out << "(lambda ";
				if (n->rettype)
				{
					out << n->rettype << " ";
				}
				if (n->captures)
				{
					out << " (captures " << n->captures << ")";
				}
				out << n->parameters << " " << n->statements << ")";
				break;
			}

			case pn_expr_requires::type :
			{
				auto	n	= _node.at<pn_expr_requires>();
				out << "(requires ";
				if (n->parameters)
				{
					out << n->parameters;
				}
				out << "[";
				for (auto& c : n->conditions)
				{
					if (count++ > 0) out << " ";
					if (c.symbol)
					{
						out << "(match " << c.symbol << " " << c.expr << ")";
					}
					else
					{
						out << "(expr " << c.expr << ")";
					}
				}
				out << "]";
				out << ")";
				break;
			}

			case pn_expr_static_integer::type :
			{
				auto	n	= _node.at<pn_expr_static_integer>();
				out << "(dec " << format("{d}", {n->integer}) << ")";
				break;
			}

			case pn_expr_static_string::type :
			{
				auto	n	= _node.at<pn_expr_static_string>();
				out << "\"" << n->str << "\"";
				break;
			}

			case pn_literal_bool::type :
			{
				auto	n	= _node.at<pn_literal_bool>();
				out << (n->value ? "(true)" : "(false)");
				break;
			}

			case pn_literal_char::type :
			{
				auto	n	= _node.at<pn_literal_char>()->c;
				if ((n >= 0x20) && (n < 0x80))
				{
					out << "'" << format("{c}", {n}) << "'";
				}
				else
				{
						 if (n == '\n')	out << "'\\n'";
					else if (n == '\r')	out << "'\\r'";
					else if (n == '\t')	out << "'\\t'";
					else if (n == '\\')	out << "'\\\\'";
					else if (n == '\0')	out << "'\\0'";
					else
					{
						out << format("'\\x{02x}'", {n});
					}
				}
				return;
			}

			case pn_literal_integer::type :
			{
				auto	n	= _node.at<pn_literal_integer>();
				out << "(dec " << format("{d}", {n->integer}) << ")";
				break;
			}

			case pn_literal_string::type :
			{
				auto	n	= _node.at<pn_literal_string>();
				out << "\"" << n->str << "\"";
				break;
			}

			case pn_literal_nullptr::type :
			{
				out << "(nullptr)";
				break;
			}

			case pn_literal_this::type :
			{
				out << "(this)";
				break;
			}

			case pn_literal_sizeof::type :
			{
				auto	n	= _node.at<pn_literal_sizeof>();
				out << "(sizeof " << n->value << ")";
				break;
			}

			case pn_operator_address_of::type :
			{
				auto	n	= _node.at<pn_operator_address_of>();
				out << "(prefix `&` " << n->value << ")";
				break;
			}

			case pn_operator_bin::type :
			{
				auto	n	= _node.at<pn_operator_bin>();
				out << "(ope `" << get_operator_string(n->ope) << "` " << n->left << " " << n->right << ")";
				break;
			}

			case pn_operator_call::type :
			{
				auto	n	= _node.at<pn_operator_call>();
				out << "(call ";
				out << n->function;
				out << " [";
				for (auto& a : n->arguments)
				{
					if (count++ > 0) out << " ";
					out << a;
				}
				out << "])";
				break;
			}


			case pn_operator_conditions::type :
			{
				auto	n	= _node.at<pn_operator_conditions>();
				out << "(conditions " << n->condition << " " << n->v1 << " " << n->v2 << ")";
				break;
			}

			case pn_operator_delete::type :
			{
				auto	n	= _node.at<pn_operator_delete>();
				out << "(delete" << (n->is_array ? "[]" : "") << " " << n->value << ")";
				break;
			}

			case pn_operator_incdec::type :
			{
				auto	n	= _node.at<pn_operator_incdec>();
				if (n->ope == operator_type::ot_inc_suffix || n->ope == operator_type::ot_dec_suffix)
				{
					out << "(suffix";
				}
				else
				{
					out << "(prefix";
				}
				out << " `" << get_operator_string(n->ope) << "` " << n->value << ")";
				break;
			}

			case pn_operator_indirection::type :
			{
				auto	n	= _node.at<pn_operator_indirection>();
				out << "(prefix `*` " << n->value << ")";
				break;
			}

			case pn_operator_member_access::type :
			{
				auto	n	= _node.at<pn_operator_member_access>();
				out << "(ope `.` " << n->value << " " << n->member << ")";
				break;
			}

			case pn_operator_member_call::type :
			{
				auto	n	= _node.at<pn_operator_member_call>();
				out << "(callthis " << n->thiz << " " << n->function;
				out << " [";
				for (auto& a : n->arguments)
				{
					if (count++ > 0) out << " ";
					out << a;
				}
				out << "])";
				break;
			}

			case pn_operator_member_access_arrow::type :
			{
				auto	n	= _node.at<pn_operator_member_access_arrow>();
				out << "(ope `->` " << n->value << " " << n->member << ")";
				break;
			}

			case pn_operator_member_call_arrow::type :
			{
				auto	n	= _node.at<pn_operator_member_call_arrow>();
				out << "(callthisarrow " << n->thiz << " " << n->function;
				out << " [";
				for (auto& a : n->arguments)
				{
					if (count++ > 0) out << " ";
					out << a;
				}
				out << "])";
				break;
			}

			case pn_operator_new::type :
			{
				auto	n	= _node.at<pn_operator_new>();
				out << "(new" << (n->is_array ? "[]" : "") << " " << n->totype;
				if (n->values.size() > 0)
				{
					out << " (placement";
					for (auto& v : n->values)
					{
						out << " " << v;
					}
					out << ")";
				}
				out << " (arguments";
				for (auto& v : n->arguments)
				{
					out << " " << v;
				}
				out << ")";
				out << ")";
				break;
			}

			case pn_operator_single::type :
			{
				auto	n	= _node.at<pn_operator_single>();
				out << "(prefix `" << get_operator_string(n->ope) << "` " << n->value << ")";
				break;
			}

			case pn_operator_subscriptl::type :
			{
				auto	n	= _node.at<pn_operator_subscriptl>();
				out << "(ope `[]` " << n->value << " " << n->idx << ")";
				break;
			}

			case pn_spec_member_friend::type :
			{
				auto	n	= _node.at<pn_spec_member_friend>();
				out << "(friend " << n->decl << ")";
				break;
			}

			case pn_spec_symbol_array::type :
			{
				auto	n	= _node.at<pn_spec_symbol_array>();
				out << "(type " << n->name << " []";
				for (auto& s : n->sizes)
				{
					out << " (" << s << ")";
				}
				out << ")";
				break;
			}

			case pn_spec_symbol_const::type :
			{
				auto	n	= _node.at<pn_spec_symbol_const>();
				out << "(type " << n->name << " const)";
				break;
			}

			case pn_spec_symbol_reference::type :
			{
				auto	n	= _node.at<pn_spec_symbol_reference>();
				out << "(type " << n->name << " &)";
				break;
			}

			case pn_spec_symbol_move::type :
			{
				auto	n	= _node.at<pn_spec_symbol_move>();
				out << "(type " << n->name << " &&)";
				break;
			}

			case pn_spec_symbol_parampack::type :
			{
				auto	n	= _node.at<pn_spec_symbol_parampack>();
				out << "(type " << n->name << "...)";
				break;
			}

			case pn_spec_symbol_pointer::type :
			{
				auto	n	= _node.at<pn_spec_symbol_pointer>();
				out << "(type " << n->name << " *)";
				break;
			}

			case pn_stmt_branch_break::type :
			{
				out << "(break)";
				break;
			}

			case pn_stmt_branch_continue::type :
			{
				out << "(continue)";
				break;
			}

			case pn_stmt_compound::type :
			{
				auto	n	= _node.at<pn_stmt_compound>();
				out << "[" << nest_in;
				for (auto& s : n->statements)
				{
					out << s << ret;
				}
				out << nest_out << "]";
				break;
			}

			case pn_stmt_for::type :
			{
				auto	n	= _node.at<pn_stmt_for>();
				out << "(for " << nest_in << n->init << ret << n->conditions << ret << n->increments << ret << n->statements << nest_out << ")";
				break;
			}

			case pn_stmt_for_rangedbase::type :
			{
				auto	n	= _node.at<pn_stmt_for_rangedbase>();
				out << "(foreach " << n->init << " " << n->value << " " << n->statements << ")";
				break;
			}

			case pn_stmt_if::type :
			{
				auto	n	= _node.at<pn_stmt_if>();
				out << "(if";
				if (n->init)
				{
					out << " (init " << n->init << ")";
				}
				out << " " << n->conditions;
				if (n->statements1)
				{
					out << " " << n->statements1;
				}
				if (n->statements2)
				{
					out << " " << n->statements2;
				}
				out << ")";
				break;
			}

			case pn_stmt_do_while::type :
			{
				auto	n	= _node.at<pn_stmt_do_while>();
				out << "(do_while";
				if (n->conditions)	{ out << " " << n->conditions;	}
				if (n->statements)	{ out << " " << n->statements;	}
				out << ")";
				break;
			}

			case pn_stmt_return::type :
			{
				auto	n	= _node.at<pn_stmt_return>();
				if (n->value)
				{
					out << "(return " << n->value << ")";
				}
				else
				{
					out << "(return)";
				}
				break;
			}

			case pn_stmt_switch::type :
			{
				auto	n	= _node.at<pn_stmt_switch>();
				out << "(switch ";
				out << n->conditions << nest_in;
				for (auto& c : n->cases)
				{
					out << "(";
					if (c.value)	out << c.value;
					else			out << "default";
					out << " " << c.statements << ")" << ret;
				}
				out << nest_out << ")";
				break;
			}

			case pn_stmt_while::type :
			{
				auto	n	= _node.at<pn_stmt_while>();
				out << "(while " << n->conditions << " " << n->statements << ")";
				break;
			}

			case pn_symbol_decltye::type :
			{
				auto	n	= _node.at<pn_symbol_decltye>();
				out << "(decltype " << n->value << ")";
				break;
			}

			case pn_symbol_name::type :
			{
				auto	n	= _node.at<pn_symbol_name>();
				out << "`" << n->name << "`";
				break;
			}

			case pn_symbol_nameabs::type :
			{
				auto	n	= _node.at<pn_symbol_nameabs>();
				out << "(nsabs " << n->scope << ")";
				break;
			}

			case pn_symbol_functype::type :
			{
				auto	n	= _node.at<pn_symbol_functype>();
				out << "(symbol func " << n->rettype << " " << n->parameters << ")";
				break;
			}

			case pn_symbol_scope::type :
			{
				auto	n	= _node.at<pn_symbol_scope>();
				out << "(ns";
				for (auto& s : n->scopes)
				{
					out << " " << s;
				}
				out << ")";
				break;
			}

			case pn_symbol_operator::type :
			{
				auto	n	= _node.at<pn_symbol_operator>();
				out << "(symbol operator `" << get_operator_string(n->ope) << "`)";
				break;
			}

			case pn_symbol_template::type :
			{
				auto	n	= _node.at<pn_symbol_template>();
				out << "(symbol template `" << n->name << "`";
				for (auto& t : n->templateParams)
				{
					out << " " << t;
				}
				out << ")";
				break;
			}

			case pn_using_alias::type :
			{
				auto	n	= _node.at<pn_using_alias>();
				out << "(using alias " << n->name << " " << n->target << ")";
				break;
			}

			case pn_using_namespace::type :
			{
				auto	n	= _node.at<pn_using_namespace>();
				out << "(using namespace " << n->name << ")";
				break;
			}

			case pn_echo::type :
			{
				auto	n	= _node.at<pn_echo>();
				out << "(echo \"" << n->doc << "\")";
				break;
			}

			case pn_initializer_list::type :
			{
				auto	n	= _node.at<pn_initializer_list>();
				out << "(initializer_list";
				for (auto& v : n->values)
				{
					out << " (";
					if (!v.name.empty())
					{
						out << "`" << v.name << "` ";
					}
					out << v.value << ")";
				}
				out << ")";
				break;
			}

			case pn_struct_initializer::type :
			{
				auto	n	= _node.at<pn_struct_initializer>();
				out << "(struct_initializer " << n->name << " " << n->initlist << ")";
				break;
			}

			case pn_list::type :
			{
				auto	n	= _node.at<pn_list>();
				out << "(list";
				for (auto& v : n->values)
				{
					out << " "<< v;
				}
				out << ")";
				break;
			}

			case pn_label::type :
			{
				auto	n	= _node.at<pn_label>();
				out << "(label `" << (text)n->name << "`)";
				break;
			}

			default:
			//	AMTRS_FORCE_LOG("unkown {}", (int)_node.type());
				break;
		}
	}

	struct	indent {};
	struct	_ret {}			ret;
	struct	_nest_in {}		nest_in;
	struct	_nest_out {}	nest_out;
	struct	output
	{
		pfl_core*	thiz;
		output& operator << (pflnode& _n)  { thiz->serialize(_n); return *this; }
		output& operator << (indent _n) { if (!thiz->fp->humam) thiz->callback(" "); else { for (int i = 0; i < thiz->_indent; ++i) { thiz->callback("    "); } } return *this; }
		output& operator << (_ret _n)
		{
			if (!thiz->_return) { if (thiz->fp->humam) { *this << "\n"; } }
			thiz->_return = true;
			return *this;
		}
		output& operator << (text  _t)
		{
			if (thiz->_return)
			{
				*this << indent();
				thiz->_return	= false;
			}
			thiz->callback(_t);
			return	*this;
		}
		output& operator << (_nest_in _n)  { *this << _ret{}; thiz->_indent++; thiz->_return = true; return *this; }
		output& operator << (_nest_out _n) { *this << _ret{}; thiz->_indent--; thiz->_return = (thiz->_return ? false : true); return *this; }
	};

	output							out{this};
	int								_indent	= 0;
	int								_return	= false;
};


void pfl::serialize(pflnode& _node, typeutil::callback<void(text)> _callback)
{
	pfl_core	s;
	s.fp		= this;
	s.callback	= std::move(_callback);
	s.serialize(_node);
}

PSNVM_LANG_NAMESPACE_END

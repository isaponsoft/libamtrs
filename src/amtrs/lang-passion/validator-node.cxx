/* Copyright (c) 2022, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
/*
 * 汎用ライブラリ
 * OSや環境問わずに使用できる。
 */
PSNVM_LANG_NAMESPACE_BEGIN
using namespace amtrs;
using namespace psnvm;
using namespace psnvm::lang;


auto validator::validate(pflnode& _n) -> pvlnode
{
	pvlnode	ret;

//	if (state().func){	AMTRS_FORCE_LOG("val {} in {}", (int)_n.type(), (text)state().func->name); }
//	else			 {	AMTRS_FORCE_LOG("val {}", (int)_n.type()); }
	switch (_n.type())
	{
		case pflnode_types::none :
		{
			break;
		}

		case pn_literal_integer::type :
		{
			auto	n	= _n.at<pn_literal_integer>();
			ret	= pvlnode::make<vn_imm_integer>(_n.location(), get_class("int"), n->integer);
			break;
		}

		case pn_literal_sizeof::type :
		{
			auto	n	= _n.at<pn_literal_sizeof>();
			pvlnode	v	= validate(n->value);
			ret	= pvlnode::make<vn_imm_sizeof>(_n.location(), get_class("int"), v.datatype());
			break;
		}

		case pn_literal_this::type :
		{
			auto	n	= _n.at<pn_literal_this>();
			ret	= pvlnode::make<vn_this>(_n.location(), ((vdatatype)state().func->cls).add_pointer());
			break;
		}

		case pn_literal_string::type :
		{
			auto	n	= _n.at<pn_literal_string>();
			auto	str	= n->str;
			auto	typ	= get_class("psn::constring");
			ret	= pvlnode::make<vn_imm_string>(_n.location(), typ, str);
			break;
		}

		case pn_declare_function::type :
		{
			add(_n.at<pn_declare_function>());
			return	{};
		}

		case pn_declare_function_constractor::type :
		{
			add(_n.at<pn_declare_function_constractor>());
			return	{};
		}

		case pn_expr_cast::type :
		{
			auto	n		= _n.at<pn_expr_cast>();
			auto	totype	= make_symbol(n->totype);
			auto	exp		= validate(n->value);
			if (bad())
			{
				return	{};
			}
			ret	= std::move(exp);
			ret.datatype(get_class(totype));
			break;
		}

		case pn_initializer_list::type :
		{
			using					initinfo	= vn_value_initializerlist::initinfo;
			auto					n			= _n.at<pn_initializer_list>();
			simplearray<initinfo>	values;
			for (auto& v : n->values)
			{
				values.push_back(initinfo{v.name, validate(v.value)});
			}
			ret	= pvlnode::make<vn_value_initializerlist>(_n.location(), get_class("void"), std::move(values));
			break;
		}

		// 非メンバ関数
		case pn_operator_call::type :
		{
			auto					n			= _n.at<pn_operator_call>();
			pvlnode					name		= validate(n->function);
			if (bad())
			{
				return	{};
			}
			shared_string			fname;
			bool					haveThis	= false;

			// 引数の一覧を生成
			simplearray<pvlnode>		argv;
			simplearray<vdatatype>		argt;
			for (auto& a : n->arguments)
			{
				pvlnode	an	= validate(a);
				if (bad())
				{
					return	{};
				}
				argt.push_back(an.datatype());
				argv.push_back(std::move(an));
			}

			// スカラに対して operator () を探そうとした
			if (name.datatype().is_scalar())
			{
				errors.push_back(errorinfo(_n.location(), errorcode::ec_expr_cant_invoke_scalar, name.location(), name.datatype().remove_reference().name()));
				return	{};
			}

			// 変数に一致していない場合は関数呼び出し
			if (name.type() == vn_symbol::type)
			{
				fname	= name.at<vn_symbol>()->name;

			}
			// 参照を指しているので関数ポインタまたはオブジェクト
			else if (name.datatype().is_reference())
			{
				fname		= name.datatype().remove_reference().name() + "::operator()";
				argv.push_front(name);
				haveThis	= true;
			}
			else
			{
				AMTRS_FORCE_LOG("NO SYMBOL {}", (text)name.datatype().name());
				return	{};
			}

			// 一致する関数を検索
			auto	f	= find_function((text)fname, view<vdatatype>(argt.begin(), argt.end()));
			if (!f)
			{
				// 引数違いの関数がある場合のエラーメッセージ
				auto	fnc	= functions.find(fname);
				if (fnc != functions.end())
				{
					int		count		= 0;
					sstring	signature 	= fname + "(";
					for (auto& a : argt)
					{
						if (count++ > 0)	signature += ",";
						signature += a.name();
					}
					signature += ")";
					errorinfo	err(_n.location(), errorcode::ec_expr_unmatch_function_paramtypes, (text)signature, {});
					for (auto& amb : fnc->second)
					{
						err.ambitiouses.push_back({ amb->location, amb->display_name });
					}
					errors.push_back(std::move(err));
					return	{};
				}

				// そもそも operator() が見つからない
				if (name.datatype().is_reference())
				{
					errors.push_back(errorinfo(_n.location(), errorcode::ec_expr_nothing_invoke_operator, fname, {}));
					return	{};
				}

				errors.push_back(errorinfo(_n.location(), errorcode::ec_unkown_error, fname, {}));
				return	{};
			}

			// this call
			if (f->is_thiscall() && !haveThis)
			{
				int		count		= 0;
				sstring	signature 	= fname + "(";
				for (auto& a : argt)
				{
					if (count++ > 0)	signature += ",";
					signature += a.name();
				}
				signature += ")";
				errors.push_back(errorinfo(_n.location(), errorcode::ec_expr_cant_call_thiscall, (shared_string)(text)signature));
				return	{};
			}

			// アクセス制限
			if (!can_access(f))
			{
				errorinfo	err(_n.location(), errorcode::ec_expr_cant_access_controll, (text)f->display_name, "private");
				errors.push_back(std::move(err));
				return	{};
			}


			// 必要に応じてキャスト
			for (size_t i = haveThis ? 1 : 0; i < argv.size(); ++i)
			{
				argv[i]	= cast_to(f->parameters[i - (haveThis ? 1 : 0)].datatype, argv[i]);
			}


			if (f->spec & (uint32_t)pn_declare_specifier::spec_constexpr)
			{
				simplearray<vmvariable>	argvdata;
				simplearray<vmreg>		argvreg;

				vmachine	vm;
				for (auto& a : argv)
				{
					argvdata.push_back(vm.run(a));
				}
				for (auto& a : argvdata)
				{
					argvreg.push_back(a.value);
				}

				auto		rv		= vm.run(f, view<vmreg>(argvreg.begin(), argvreg.end()));
				if (rv.datatype.is_scalar())
				{
					auto	n	= _n.at<pn_literal_integer>();
					ret	= pvlnode::make<vn_imm_integer>(_n.location(), rv.datatype, rv.value.i);
					break;
				}

			}
			ret	= pvlnode::make<vn_expr_call>(_n.location(), f->rettype, f, std::move(argv));
			break;
		}

		case pn_operator_delete::type :
		{
			auto					n	= _n.at<pn_operator_delete>();
			if (!n->is_array)
			{
				auto					value	= validate(n->value);
				if (bad())
				{
					return	{};
				}
				value	= cast_to(value.datatype().remove_reference(), value);
				auto					cls		= value.datatype().remove_pointer();

				ref<vn_expr_call>		deallocate;
				ref<vn_expr_call>		destractor;

				// destractor call
				auto	df		= cls.is_destructible();
				if (df)
				{
					simplearray<pvlnode>	args;
					args.push_back({});
					auto	expr	= pvlnode::make<vn_expr_call>(_n.location(), get_class("void"), df, std::move(args));
					destractor		= expr.at<vn_expr_call>();
				}

				// deallocator
				{
					simplearray<pvlnode>	args;
					args.push_back({});
					simplearray<vdatatype>	dargs;
					dargs.push_back(get_class("void*"));

					// 一致する関数を検索
					auto	f	= find_function("operator delete", view<vdatatype>(dargs.data(), dargs.size()));
					if (!f)
					{
						AMTRS_FORCE_LOG("unmatch function operator ");
					}
					auto	expr= pvlnode::make<vn_expr_call>(_n.location(), get_class("void"), f, std::move(args));
					deallocate	= expr.at<vn_expr_call>();
				}

				ret	= pvlnode::make<vn_stmt_delete>(_n.location(), get_class("void"), std::move(value), std::move(deallocate), std::move(destractor));
			}
			break;
		}


		case pn_operator_new::type :
		{
			auto					n	= _n.at<pn_operator_new>();
			pvlnode					typ	= validate(n->totype);
			if (bad())
			{
				return	{};
			}

			// not array
			if (!n->is_array)
			{
				pvlnode					allocate;
				ref<vn_expr_call>		constractor;
				// allocator
				{
					simplearray<pvlnode>	argsNew;
					if (argsNew.size() == 0)
					{
						auto	on	= pvlnode::make<vn_imm_sizeof>(_n.location(), get_class("size_t"), typ.datatype());
						argsNew.push_back(std::move(on));
					}
					else
					{
						for (auto& a : n->values)
						{
							argsNew.push_back(validate(a));
							if (bad())
							{
								return	{};
							}
						}
					}
					simplearray<vdatatype>		argt;
					for (auto& a : argsNew)
					{
						argt.push_back(a.datatype());
					}

					// 一致する関数を検索
					auto	f	= find_function("operator new", view<vdatatype>(argt.begin(), argt.end()));
					if (!f)
					{
						AMTRS_FORCE_LOG("unmatch function operator ");
					}


					// 必要に応じてキャスト
					for (size_t i = 0; i < argsNew.size(); ++i)
					{
						argsNew[i]	= cast_to(f->parameters[i].datatype, argsNew[i]);
					}
					allocate	= pvlnode::make<vn_expr_call>(_n.location(), f->rettype, f, std::move(argsNew));
				}
				// constractor
				{
					simplearray<pvlnode>	argsConst;
					simplearray<text>		argt;
					argsConst.push_back({});
					for (auto& a : n->arguments)
					{
						argsConst.push_back(validate(a));
						if (bad())
						{
							return	{};
						}
						argt.push_back(argsConst.back().datatype().name());
					}

					auto	f		= typ.datatype().is_constructible(view<text>(argt.data(), argt.size()));
					if (!can_access(f))
					{
						errorinfo	err(_n.location(), errorcode::ec_expr_cant_access_controll, f->display_name, "private");
						err.ambitiouses.push_back({ f->location, f->display_name });
						errors.push_back(std::move(err));
						return	{};
					}
					auto	expr	= pvlnode::make<vn_expr_call>(_n.location(), get_class("void"), f, std::move(argsConst));
					constractor		= expr.at<vn_expr_call>();
				}
				ret	= pvlnode::make<vn_expr_new>(_n.location(), get_class("void"), std::move(allocate), std::move(constractor));
				break;
			}
			// array
			else
			{
				pvlnode					allocate;
				ref<vn_expr_call>		constractor;
				// allocator
				{
					simplearray<pvlnode>	argsNew;
					if (argsNew.size() == 0)
					{
						auto	on	= pvlnode::make<vn_imm_sizeof>(_n.location(), get_class("size_t"), typ.datatype());
						argsNew.push_back(std::move(on));
					}
					else
					{
						for (auto& a : n->values)
						{
							argsNew.push_back(validate(a));
							if (bad())
							{
								return	{};
							}
						}
					}
					simplearray<vdatatype>		argt;
					for (auto& a : argsNew)
					{
						argt.push_back(a.datatype());
					}

					// 一致する関数を検索
					auto	f	= find_function("operator new", view<vdatatype>(argt.begin(), argt.end()));
					if (!f)
					{
						AMTRS_FORCE_LOG("unmatch function operator ");
					}


					// 必要に応じてキャスト
					for (size_t i = 0; i < argsNew.size(); ++i)
					{
						argsNew[i]	= cast_to(f->parameters[i].datatype, argsNew[i]);
					}
					allocate	= pvlnode::make<vn_expr_call>(_n.location(), f->rettype, f, std::move(argsNew));
				}
				// constractor
				{
					simplearray<pvlnode>	argsConst;
					simplearray<text>		argt;
					argsConst.push_back({});
					for (auto& a : n->arguments)
					{
						argsConst.push_back(validate(a));
						if (bad())
						{
							return	{};
						}
						argt.push_back(argsConst.back().datatype().name());
					}

					auto	f		= typ.datatype().is_constructible(view<text>(argt.data(), argt.size()));
					if (!can_access(f))
					{
						errorinfo	err(_n.location(), errorcode::ec_expr_cant_access_controll, f->display_name, "private");
						err.ambitiouses.push_back({ f->location, f->display_name });
						errors.push_back(std::move(err));
						return	{};
					}
					auto	expr	= pvlnode::make<vn_expr_call>(_n.location(), get_class("void"), f, std::move(argsConst));
					constractor		= expr.at<vn_expr_call>();
				}
				ret	= pvlnode::make<vn_expr_new>(_n.location(), get_class("void"), std::move(allocate), std::move(constractor));
				break;
			}
		}

		case pn_operator_subscriptl::type :
		{
			auto			n		= _n.at<pn_operator_subscriptl>();
			auto			value	= validate(n->value);	if (bad()) { return {}; }
			auto			idx		= validate(n->idx);		if (bad()) { return {}; }

			// ポインタに対する配列アクセス
			if (value.datatype().remove_reference().is_pointer())
			{
				value	= cast_to(value.datatype().remove_reference(), value);
				if (bad()) { return {}; }

				
			}



//AMTRS_FORCE_LOG("scscriptal {}", (text)elmtype.name());

//			ret	= pvlnode::make<vn_load_effective_address>(_n.location(), get_class("void"), std::move(allocate), std::move(constractor));
			break;

//vn_load_effective_address
/*
	pflnode	value;
	pflnode	idx;
*/

			break;
		}

		case pn_declare_variable::type :
		{
			using						varstate	= vn_local_allocate::varstate;
			auto						n			= _n.at<pn_declare_variable>();
			vdatatype					datatype	= get_class(make_symbol(n->typesym));
			vdatatype					reftype		= datatype.add_reference();
			simplearray<ref<varstate>>	vars;
			size_t						localindex	= localvars.size();
			for (auto& v : n->values)
			{
				pvlnode	init;

				// 初期化値が指定されている
				if (v.init)
				{
					simplearray<pvlnode>	argv;
					simplearray<text>		argt;
					pvlnode					src		= validate(v.init);
					if (bad())
					{
						return	{};
					}
					auto					nlst	= src.at<vn_list>();
					if (nlst)
					{
						argv	= nlst->values;
					}
					else
					{
						argv.push_back(src);
					}
					for (auto& a : argv)
					{
						argt.push_back((text)a.datatype().name());
					}
					auto	f	= datatype.is_constructible(view<text>(argt.data(), argt.size()));
					if (f)
					{
						pvlnode						obj		= pvlnode::make<vn_address_localref>(_n.location(), reftype, localindex);
						simplearray<pvlnode>		arglist;
						arglist.push_front(std::move(obj));
						for (size_t i = 0; i < argv.size(); ++i)
						{
							arglist.push_back(cast_to(f->parameters[i].datatype, argv[i]));
						}
						init	= pvlnode::make<vn_expr_call>(_n.location(), get_class("void"), f, std::move(arglist));
					}
					else
					{
						pvlnode	dst	= pvlnode::make<vn_address_localref>(_n.location(), reftype, localindex);
						init	= expr_assign(dst, src);
					}
				}
				// 初期化値はないがコンストラクタがあるならコンストラクタを使用する
				else
				{
					auto	f	= datatype.is_constructible({});
					if (f)
					{
						pvlnode					obj		= pvlnode::make<vn_address_localref>(_n.location(), reftype, localindex);
						simplearray<pvlnode>		argv;
						argv.push_back(std::move(obj));
						init	= pvlnode::make<vn_expr_call>(_n.location(), get_class("void"), f, std::move(argv));
					}
				}

				ref<varstate>	var	= new varstate();
				var->datatype	= datatype;
				var->name		= shared_string(make_symbol(v.name));
				var->init		= std::move(init);
				vars.push_back(std::move(var));
				++localindex;
			}
			for (auto& v : vars)
			{
				localvars.push_back(v);
			}
			return	pvlnode::make<vn_local_allocate>(_n.location(), std::move(datatype), std::move(vars));
		}

		case pn_list::type :
		{
			auto					n			= _n.at<pn_list>();
			simplearray<pvlnode>	values;
			for (auto& v : n->values)
			{
				values.push_back(validate(v));
				if (bad())
				{
					return	{};
				}
			}
			ret	= pvlnode::make<vn_list>(_n.location(), get_class("void"), std::move(values));
			break;
		}

		case pn_stmt_branch_break::type :
		{
			ret	= pvlnode::make<vn_stmt_branch_break>(_n.location(), get_class("void"));
			break;
		}

		case pn_stmt_compound::type :
		{
			size_t					localsize	= localvars.size();
			auto					n			= _n.at<pn_stmt_compound>();
			simplearray<pvlnode>	statements;
			bool					enabling	=  !mState.disabled();
			for (auto& s : n->statements)
			{
				pvlnode	v	= validate(s);
				if (bad())
				{
					return	{};
				}
				if (v && enabling)
				{
					statements.push_back(v);
				}
				if (enabling)
				{
					enabling	= !mState.disabled();
				}
				else
				{
					break;
				}
			}
			if (enabling)
			{
				free_locals(statements, _n.location(), localsize);
			}
			ret	= pvlnode::make<vn_compound>(_n.location(), get_class("void"), std::move(statements));
			break;
		}

		case pn_stmt_do_while::type :
		{
			size_t		localsize	= localvars.size();
			auto		n			= _n.at<pn_stmt_do_while>();
			pvlnode		cond		= cast_to(get_class("bool"), validate(n->conditions));
			if (bad())
			{
				return	{};
			}
			pvlnode		stmt		= validate(n->statements);
			if (bad())
			{
				return	{};
			}
			ret	= pvlnode::make<vn_stmt_while>(_n.location(), get_class("void"), std::move(cond), std::move(stmt));
			break;
		}

		case pn_stmt_for::type :
		{
			size_t					localsize	= localvars.size();
			auto					n		= _n.at<pn_stmt_for>();
			simplearray<pvlnode>	statements;
			if (n->init)
			{
				statements.push_back(validate(n->init));
				if (bad())
				{
					return	{};
				}
			}

			pvlnode		cond	= cast_to(get_class("bool"), validate(n->conditions));
			pvlnode		incr	= validate(n->increments);
			if (bad())
			{
				return	{};
			}
			pvlnode		stmt	= validate(n->statements);
			if (bad())
			{
				return	{};
			}
			statements.push_back(pvlnode::make<vn_stmt_for>(_n.location(), get_class("void"), std::move(cond), std::move(incr), std::move(stmt)));

			free_locals(statements, _n.location(), localsize);
			if (statements.size() > 1)
			{
				ret	= pvlnode::make<vn_compound>(_n.location(), get_class("void"), std::move(statements));
			}
			else
			{
				ret	= statements.front();
			}
			break;
		}

		case pn_stmt_if::type :
		{
			size_t					localsize	= localvars.size();
			auto					n		= _n.at<pn_stmt_if>();
			simplearray<pvlnode>	statements;
			if (n->init)
			{
				statements.push_back(validate(n->init));
				if (bad())
				{
					return	{};
				}
			}

			pvlnode		cond	= cast_to(get_class("bool"), validate(n->conditions));
			if (bad())
			{
				return	{};
			}

			codestate	st		= mState;
			pvlnode		stm1	= validate(n->statements1);
			if (bad())
			{
				return	{};
			}
			codestate	st1		= mState;
			mState	= st;
			pvlnode		stm2	= validate(n->statements2);
			if (bad())
			{
				return	{};
			}
			codestate	st2		= mState;
			statements.push_back(pvlnode::make<vn_stmt_if>(_n.location(), get_class("void"), std::move(cond), std::move(stm1), std::move(stm2)));

			mState.is_return	= st1.is_return && st2.is_return;
			mState.is_break		= st1.is_break && st2.is_break;
			mState.is_continue	= st1.is_continue && st2.is_continue;

			// free local variables
			free_locals(statements, _n.location(), localsize);
			if (statements.size() > 1)
			{
				ret	= pvlnode::make<vn_compound>(_n.location(), get_class("void"), std::move(statements));
			}
			else
			{
				ret	= statements.front();
			}
			break;
		}

		case pn_stmt_switch::type :
		{
			auto									n			= _n.at<pn_stmt_switch>();
			pvlnode									cond		= validate(n->conditions);
			if (bad())
			{
				return	{};
			}
			if (cond.datatype().is_reference())
			{
				cond	= cast_to(cond.datatype().remove_reference(), cond);
				if (bad())
				{
					return	{};
				}
			}
			simplearray<vn_stmt_switch::casedata>	cases;
			for (auto& c : n->cases)
			{
				cases.push_back({
					validate(c.value),
					validate(c.statements),
				});
				if (bad())
				{
					return	{};
				}
			}
			ret	= pvlnode::make<vn_stmt_switch>(_n.location(), get_class("void"), std::move(cond), std::move(cases));
			break;
		}


		case pn_stmt_while::type :
		{
			auto		n			= _n.at<pn_stmt_while>();
			pvlnode		cond		= cast_to(get_class("bool"), validate(n->conditions));
			if (bad())
			{
				return	{};
			}
			pvlnode		stmt		= validate(n->statements);
			if (bad())
			{
				return	{};
			}
			ret	= pvlnode::make<vn_stmt_while>(_n.location(), get_class("void"), std::move(cond), std::move(stmt));
			break;
		}

		case pn_stmt_return::type :
		{
			simplearray<pvlnode>	statements;
			auto					n		= _n.at<pn_stmt_return>();
			pvlnode					val		= validate(n->value);
			if (bad())
			{
				return	{};
			}

			// 関数の戻り値と式の結果の型を比較する
			vdatatype				rettype;
			if (state().func)
			{
				// 関数に指定されている戻り値を調べる
				rettype	= state().func->rettype;
			}
			else
			{
				// グローバルスコープではどんな値でも返す
				rettype	= val.datatype().remove_reference();
			}

			// void なのに戻り値があればエラー
			if (rettype.name() == "void")
			{
				if (val.type() != vnodetypes::_none)
				{
					errors.push_back(errorinfo(_n.location(), errorcode::ec_return_not_void));
					return	{};
				}
			}
			else
			{
				if (val.type() == vnodetypes::_none)
				{
					errors.push_back(errorinfo(_n.location(), errorcode::ec_return_nothing));
					return	{};
				}
				auto	rv	= cast_to(rettype, val);
				if (rv.type() == vnodetypes::_none)
				{
					errors.push_back(errorinfo(_n.location(), errorcode::ec_return_unmatch_type, rettype.name(), val.datatype().remove_reference().name()));
					return	{};
				}
				statements.push_back(pvlnode::make<vn_returnval>(_n.location(), rettype, std::move(rv)));
			}

			// ローカル変数の破棄
			free_locals(statements, _n.location(), 0);
			mState.is_return	= true;
			ret	= pvlnode::make<vn_return>(_n.location(), rettype);
			if (statements.size() > 0)
			{
				statements.push_back(std::move(ret));
				ret	= pvlnode::make<vn_compound>(_n.location(), get_class("void"), std::move(statements));
			}
			break;
		}

		case pn_operator_address_of::type :
		{
			auto	n		= _n.at<pn_operator_address_of>();
			pvlnode	val		= validate(n->value);
			if (bad())
			{
				return	{};
			}
			if (!val.datatype().is_reference())
			{
				AMTRS_FORCE_LOG("ERROR rval is {}", (text)val.datatype().name());
				return	{};
			}
			val.datatype(val.datatype().remove_reference().add_pointer());
			ret	= val;
			break;
		}

		case pn_operator_bin::type :
		{
			auto	n		= _n.at<pn_operator_bin>();
			auto	v1		= validate(n->left);	if (bad()) { return	{}; }
			auto	v2		= validate(n->right);	if (bad()) { return	{}; }
			auto	v1type	= v1.datatype();
			auto	v2type	= v2.datatype();
			auto	ope		= n->ope;

			// スカラ同士の演算ではないようなのでオペレータで処理する
			if (!v1type.is_scalar() || !v2type.is_scalar())
			{
				// オペレータを検索する
				shared_string	fname	= shared_string("operator") + get_operator_string(ope);
				auto			ltype	= v1type.remove_reference();
				auto			lcls	= ltype.at<vdecl_class>();

				// L::operatorXX(R&)
				auto	f	= find_function((text)(ltype.name() + "::" + fname), view<vdatatype>(&v2type, 1));
				if (f)
				{
					// 必要に応じてキャスト
					simplearray<pvlnode>	argv;
					argv.push_back(v1);
					argv.push_back(cast_to(f->parameters[0].datatype, v2));
					return	pvlnode::make<vn_expr_call>(_n.location(), f->rettype, f, std::move(argv));
				}

				// operatorXX(L&, R&)
				vdatatype	t[2]	= { ltype, v2type };
				f			= find_function(shared_string("operator") + get_operator_string(ope), view<vdatatype>(t, 2));
				if (f)
				{
					// 必要に応じてキャスト
					simplearray<pvlnode>	argv;
					argv.push_back(cast_to(f->parameters[0].datatype, v1));
					argv.push_back(cast_to(f->parameters[1].datatype, v2));
					return	pvlnode::make<vn_expr_call>(_n.location(), f->rettype, f, std::move(argv));
				}

				AMTRS_FORCE_LOG("unmatch type `{}` `{}`", (text)v1type.name(), (text)v2type.name());
				break;
			}

			// 代入式
			if (ope == operator_type::ot_assign)
			{
				ret	= expr_assign(v1, v2);
				break;
			}

			// 通常の演算
			if (ope != operator_type::ot_assign)
			{
				// ポインタに対する演算かどうか調べる
				if (v1type.is_pointer())
				{
				}

				// bool では演算は比較のみ
				if (v1type.name() == "bool")
				{
				}

				// 浮動小数
				if (v1type.name() == "float" || v1type.name() == "double")
				{
				}

				// それ以外は整数として扱う

				// 静的演算可能なので静的に演算する
				if (v1.type() == vnodetypes::_imm_integer && v2.type() == vnodetypes::_imm_integer)
				{
					intmax_t	val	= 0;
					intmax_t	v1n	= val	= v1.at<vn_imm_integer>()->value;
					intmax_t	v2n	= val	= v2.at<vn_imm_integer>()->value;
					switch (ope)
					{
						case operator_type::ot_addition :			val	= v1n + v2n;	break;
						case operator_type::ot_subtraction :		val	= v1n - v2n;	break;
						case operator_type::ot_multiplication :		val	= v1n * v2n;	break;
						case operator_type::ot_division :			val	= v1n / v2n;	break;
						case operator_type::ot_remainder :			val	= v1n % v2n;	break;
						case operator_type::ot_left_shift :			val	= v1n << v2n;	break;
						case operator_type::ot_right_shift :		val	= v1n >> v2n;	break;
						case operator_type::ot_three_way_compare :	val	= v1n < v2n ? -1 : v1n > v2n ? 1 : 0;	break;
						case operator_type::ot_leter :				val	= v1n <  v2n;	v1type = get_class("bool");	break;
						case operator_type::ot_leter_equals :		val	= v1n <= v2n;	v1type = get_class("bool");	break;
						case operator_type::ot_grater :				val	= v1n >  v2n;	v1type = get_class("bool");	break;
						case operator_type::ot_grater_equals :		val	= v1n >= v2n;	v1type = get_class("bool");	break;
						case operator_type::ot_equals :				val	= v1n == v2n;	v1type = get_class("bool");	break;
						case operator_type::ot_not_equals :			val	= v1n != v2n;	v1type = get_class("bool");	break;
						case operator_type::ot_bitwise_and :		val	= v1n & v2n;	break;
						case operator_type::ot_bitwise_xor :		val	= v1n ^ v2n;	break;
						case operator_type::ot_bitwise_or :			val	= v1n | v2n;	break;
						case operator_type::ot_logical_and :		val	= v1n && v2n;	break;
						case operator_type::ot_logical_or :			val	= v1n || v2n;	break;
					}
					ret	= pvlnode::make<vn_imm_integer>(text(v1.location().s, v2.location().e), v1type, val);
					break;
				}

				// 動的スカラ
				vdatatype	rettype	= v1type.remove_reference();
				switch (ope)
				{
					case operator_type::ot_three_way_compare :	rettype	= get_class("int");		break;
					case operator_type::ot_leter :				rettype = get_class("bool");	break;
					case operator_type::ot_leter_equals :		rettype = get_class("bool");	break;
					case operator_type::ot_grater :				rettype = get_class("bool");	break;
					case operator_type::ot_grater_equals :		rettype = get_class("bool");	break;
					case operator_type::ot_equals :				rettype = get_class("bool");	break;
					case operator_type::ot_not_equals :			rettype = get_class("bool");	break;
				}
				if (v1type.is_reference())	v1	= pvlnode::make<vn_load_integral>(v1.location(), v1type.remove_reference(), std::move(v1));
				if (v2type.is_reference())	v2	= pvlnode::make<vn_load_integral>(v2.location(), v2type.remove_reference(), std::move(v2));
				ret	= pvlnode::make<vn_bin_operation_int>(text(v1.location().s, v2.location().e), rettype, ope, std::move(v1), std::move(v2));
				break;
			}

			// 代入演算
			
			

AMTRS_FORCE_LOG("bin operator unkown proccess");			
			break;
		}

		case pn_operator_indirection::type :
		{
			auto	n		= _n.at<pn_operator_indirection>();
			ret		= validate(n->value);	if (bad()) { return	{}; }
			if (ret.datatype().is_reference())
			{
				ret	= pvlnode::make<vn_load_integral>(_n.location(), ret.datatype().remove_reference(), std::move(ret));
			}
			if (!ret.datatype().is_pointer())
			{
				AMTRS_FORCE_LOG("ERROR rval is {}", (text)ret.datatype().name());
				return	{};
			}
			ret.datatype(ret.datatype().remove_pointer().add_reference());
			break;
		}

		case pn_operator_member_access::type :
		case pn_operator_member_access_arrow::type :
		{
			vdecl_class*				cls		= nullptr;
			auto						type	= _n.type();
			pn_operator_member_access*	n		= _n.at<pn_operator_member_access>();
			if (!n)
			{
				n	= _n.at<pn_operator_member_access_arrow>();
			}
			auto	value	= validate(n->value);	if (bad()) { return	{}; }
			auto	mtype	= n->member.type();

			if (type == pn_operator_member_access::type)
			{
				if (!value.datatype().is_reference())
				{
					// ERROR
					AMTRS_FORCE_LOG("no ref");
					return	{};
				}
				if (mtype == pn_symbol_name::type || mtype == pn_symbol_scope::type || mtype == pn_symbol_nameabs::type || mtype == pn_symbol_operator::type)
				{
					cls		= static_cast<vdecl_class*>(get_class(value.datatype().remove_reference().name()).cls());
				}
			}
			else if (type == pn_operator_member_access_arrow::type)
			{
				// 参照であれば参照を外してポインタに置き換える
				if (value.datatype().is_reference())
				{
					value	= pvlnode::make<vn_load_integral>(_n.location(), value.datatype().remove_reference(), std::move(value));
				}
				if (!value.datatype().is_pointer())
				{
					// ERROR
					AMTRS_FORCE_LOG("no pointer");
					return	{};
				}
				if (mtype == pn_symbol_name::type || mtype == pn_symbol_scope::type || mtype == pn_symbol_nameabs::type || mtype == pn_symbol_operator::type)
				{
					cls		= static_cast<vdecl_class*>(get_class(value.datatype().remove_pointer().name()).cls());
				}
			}


			// シンボルの場合はメンバの名前を直接指定している。
			if (mtype == pn_symbol_name::type || mtype == pn_symbol_scope::type || mtype == pn_symbol_nameabs::type || mtype == pn_symbol_operator::type)
			{
				auto		name	= make_symbol(n->member);
				auto		mem		= cls->find_member(name);
				if (mem.empty())
				{
					// ERROR
					AMTRS_FORCE_LOG("member err");
					return	{};
				}

				if (!can_access(mem))
				{
					errorinfo	err(_n.location(), errorcode::ec_expr_cant_access_controll, (text)mem.name(), "private");
					errors.push_back(std::move(err));
					return	{};
				}

				if (mem.is_variable())
				{
					auto*		var		= mem.v;
					vdatatype	reftype	= var->datatype.add_reference();
					ret	= pvlnode::make<vn_address_memberref>(_n.location(), reftype, value, nullptr, cls, mem.index);
					break;
				}
			}

			// シンボルではないのでメンバ関数ポインタの可能性を調べる
			AMTRS_FORCE_LOG("{}->*", (text)value.datatype().name());
			break;
		}


		case pn_operator_member_call::type :
		case pn_operator_member_call_arrow::type :
		{
			auto						type		= _n.type();
			pn_operator_member_call*	n			= _n.at<pn_operator_member_call>();
			if (!n)
			{
				n	= _n.at<pn_operator_member_call_arrow>();
			}
			pvlnode						thiz		= validate(n->thiz);	if (bad()) { return	{}; }
			vdatatype					thizcls;
			if (type == pn_operator_member_call::type)
			{
				thizcls		= thiz.datatype().remove_reference();
			}
			else
			{
				thizcls		= thiz.datatype().remove_reference().remove_pointer();
			}
			shared_string				backscope	= thizcls.name();
			std::swap(backscope, mExprScope);
			pvlnode					name		= validate(n->function);	if (bad()) { return	{}; }
			std::swap(backscope, mExprScope);

			if (name.type() == vn_symbol::type)
			{
				// 引数の一覧を生成
				simplearray<pvlnode>		argv;
				simplearray<vdatatype>		argt;
				argv.push_back(thiz);
				for (auto& a : n->arguments)
				{
					pvlnode	an	= validate(a);	if (bad()) { return	{}; }
					argt.push_back(an.datatype());
					argv.push_back(std::move(an));
				}
				shared_string	fname	= name.at<vn_symbol>()->name;

				fname	= thizcls.name() + "::" + fname;

				vdecl_class*	cls		= static_cast<vdecl_class*>(thizcls.cls());
				auto			member	= cls->find_member(fname);
				if (member.type != vdecl_class::_function && member.type != vdecl_class::_vfunction)
				{
					AMTRS_FORCE_LOG("unmatch function {}", (text)fname);
					return	{};
				}
				auto	f	= member.f;

				// 関数呼び出しのアクセスコントロールの確認
				if (!can_access(f))
				{
					errorinfo	err(_n.location(), errorcode::ec_expr_cant_access_controll, f->display_name, "private");
					err.ambitiouses.push_back({ f->location, f->display_name });
					errors.push_back(std::move(err));
					return	{};
				}


				// this call かつ継承元の関数の呼び出しの場合はthisポインタのキャストを行う
				if (f->thiscls)
				{
					auto	thiscls		= static_cast<vdecl_class*>(thizcls.cls());
					if (f->thiscls != thiscls)
					{
						argv.front() = pvlnode::make<vn_address_cast>(_n.location(), ((vdatatype)f->thiscls).add_reference(), f->thiscls, thiscls, argv.front());
					}
				}


				// 必要に応じてキャスト
				for (size_t i = 1; i < argv.size(); ++i)
				{
					argv[i]	= cast_to(f->parameters[i-1].datatype, argv[i]);
				}

				if (member.type == vdecl_class::_vfunction)
				{
					ret	= pvlnode::make<vn_expr_vcall>(_n.location(), f->rettype, f->thiscls, f, member.index, std::move(argv));
				}
				else
				{
					ret	= pvlnode::make<vn_expr_call>(_n.location(), f->rettype, f, std::move(argv));
				}
				break;
			}

			AMTRS_FORCE_LOG("NO SYMBOL");
			break;
		}


		case pn_operator_incdec::type :
		{
			auto	n		= _n.at<pn_operator_incdec>();
			ret		= validate(n->value);
			if (bad())
			{
				return	{};
			}
			if (!ret.datatype().is_reference())
			{
				AMTRS_FORCE_LOG("single operator value is not reference {}.", (text)ret.datatype().name());
				break;
			}
			if (ret.datatype().remove_reference().is_scalar())
			{
				auto	vtype	= ret.datatype();
				if (n->ope == operator_type::ot_inc_suffix || n->ope == operator_type::ot_dec_suffix)
				{
					vtype	= ret.datatype().remove_reference();
				}
				ret	= pvlnode::make<vn_operation_single_int>(_n.location(), vtype, n->ope, std::move(ret));
				break;
			}
			AMTRS_FORCE_LOG("Unkown single operator {}", get_operator_string(n->ope));
			break;
		}


		case pn_operator_single::type :
		{
			auto	n		= _n.at<pn_operator_single>();
			ret		= validate(n->value);	if (bad()) { return	{}; }
			if (!ret.datatype().is_reference())
			{
				AMTRS_FORCE_LOG("single operator value is not reference {}.", (text)ret.datatype().name());
				break;
			}
			if (ret.datatype().remove_reference().is_scalar())
			{
				auto	vtype	= ret.datatype();
				if (n->ope == operator_type::ot_inc_suffix || n->ope == operator_type::ot_dec_suffix)
				{
					vtype	= ret.datatype().remove_reference();
				}
				ret	= pvlnode::make<vn_operation_single_int>(_n.location(), vtype, n->ope, std::move(ret));
				break;
			}
			AMTRS_FORCE_LOG("Unkown single operator {}", get_operator_string(n->ope));
			break;
		}

		case pn_symbol_nameabs::type :
		case pn_symbol_name::type :
		case pn_symbol_scope::type :
		{
			auto		name	= make_symbol(_n);
			// ローカル変数を検索
			for (intmax_t index = localvars.size(); index > 0; --index)
			{
				auto&	var	= localvars[index-1];
				if (var->name == name)
				{
					vdatatype	rtype	= var->datatype;
					if (!rtype.is_reference())
					{
						rtype	= rtype.add_reference();
					}
					ret	= pvlnode::make<vn_address_localref>(_n.location(), rtype, index - 1);
					if (var->datatype.is_reference())
					{
						ret	= pvlnode::make<vn_load_integral>(_n.location(), rtype, std::move(ret));
					}
					return	ret;
				}
			}

			// 引数を検索
			if (state().func)
			{
				intmax_t index	= 0;
				if (state().func->cls)
				{
					++index;
				}
				for (auto& p : state().func->parameters)
				{
					if (name == p.name)
					{
						vdatatype	rtype	= p.datatype;
						if (!rtype.is_reference())
						{
							rtype	= rtype.add_reference();
						}
						ret	= pvlnode::make<vn_address_argvref>(_n.location(), rtype, index);
						if (p.datatype.is_reference())
						{
							ret	= pvlnode::make<vn_load_integral>(_n.location(), rtype, std::move(ret));
						}
						return	ret;
					}
					++index;
				}
			}

			// クラスメンバならメンバ変数も検索
			if (state().func && state().func->cls)
			{
				auto	m	= state().func->cls->find_member(name);
				if (m.is_variable())
				{
					if (!state().func->is_thiscall())
					{
						errors.push_back(errorinfo(_n.location(), errorcode::ec_expr_cant_access_static_member, (text)format("{}::{}", {(text)state().func->cls->name, (text)name}), {}));
						return	{};
					}

					pvlnode	thiz	= pvlnode::make<vn_this>(_n.location(), ((vdatatype)state().func->cls).add_reference());
					ret	= pvlnode::make<vn_address_memberref>(_n.location(), m.datatype.add_reference(), thiz, state().func->cls, state().func->thiscls, m.index);
					return	ret;
				}
			}


			// クラスにマッチ
			auto	cls	= get_class(name);
			if (!cls.empty())
			{
				ret	= pvlnode::make<vn_typename>(_n.location(), cls);
				return	ret;
			}

			// 関数にマッチ
			auto	fname	= name;
			auto	funcs	= find_function(mExprScope, fname);
			if (funcs)
			{
				ret	= pvlnode::make<vn_symbol>(_n.location(), vdatatype{}, (shared_string)(text)name);
				return	ret;
			}

			// 何かわからん
			errors.push_back(errorinfo(_n.location(), errorcode::ec_unkown_symbol, name, {}));
			return	{};
		}

		default:
		{
			AMTRS_FORCE_LOG("Unkown pflnode {}", (int)_n.type());
			break;
		}
	}
	return	ret;
}


PSNVM_LANG_NAMESPACE_END


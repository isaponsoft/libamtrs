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

static thread_local	vmachine*	gCurrent	= nullptr;

struct	vmkepp
{
	vmkepp(vmachine* vm)
	{
		old			= gCurrent;
		gCurrent	= vm;
	}
	~vmkepp()
	{
		gCurrent	= old;
	}
	vmachine*	old;
};


vmachine::vmachine(size_t _stacksize, size_t _stackdepth)
	: mLocalvars(_stacksize)
	, mLocalSizes(_stackdepth)
{
	reset();
}


void vmachine::reset()
{
	mArguments	= nullptr;
	mReturn			= false;
	mBreak			= false;
	mBoundsError	= nullptr;
	memory_error	= [](void* mem, text location, bool write) -> bool
	{
		return	true;
	};
}


vmachine* vmachine::current()
{
	return	gCurrent;
}


void vmachine::add_memory(void* m, size_t s)
{
	mMemory.add({m, (void*)(((intptr_t)m) + s)});
}


bool vmachine::remove_memory(void* m)
{
	return	!mMemory.release(m).empty();
}


bool vmachine::in_memory(void* m, size_t s)
{
	bool	ok	= !mMemory.is_bounds({m, (void*)(((intptr_t)m) + s)}).empty();
	if (!ok)
	{
//		AMTRS_FORCE_LOG(" MERR {P}-{P} {04X} NOAVAILABLE", m, (intmax_t)m + (intmax_t)s, (intmax_t)s);
	}
	return	ok;
}


void vmachine::local_alloc(vdatatype _datatype)
{
	size_t	s	= _datatype.size_of();
	mLocalvars.alloc(s);
	*mLocalSizes.push<size_t>()	= s;
}



void vmachine::local_free()
{
	size_t	s	= *reinterpret_cast<size_t*>(mLocalSizes.sp());
	mLocalvars.free(s);
	mLocalSizes.pop<size_t>();
}


auto vmachine::local(size_t _index) -> void*
{
	intptr_t	p		= reinterpret_cast<intptr_t>(mLocalvars.top());
	size_t*		sizes	= reinterpret_cast<size_t*>(mLocalSizes.top());
	p -= *--sizes;
	while (_index > 0) { --_index; p -= *--sizes; }
	return	reinterpret_cast<void*>(p);
}


void vmachine::run(vmvariable& _retval, vdecl_function* _func, view<vmreg> _arguments)
{
	vmkepp		keep(this);
	auto	a	= mArguments;
	mArguments	=_arguments;
	add_memory(mArguments.data(), sizeof(vmreg) * mArguments.size());
	if (_func->native)
	{
		_retval.datatype	= _func->rettype;
		_retval.value.i		= _func->native(reinterpret_cast<intmax_t*>(_arguments.data()), (int)_arguments.size());
	}
	else
	{
		run(_retval, _func->statement);
	}
	remove_memory(mArguments.data());
	mArguments	= a;
}


void vmachine::run(vmvariable& _retval, pvlnode& _node)
{
	vmkepp		keep(this);
#if	0
	//AMTRS_FORCE_LOG("{}", (int)_node.type());
	switch (_node.type())
	{
		default:									AMTRS_FORCE_LOG("? {}", (int)_node.type());	break;
		case vnodetypes::_tmpnode:					AMTRS_FORCE_LOG("tmpnode");		break;
		case vnodetypes::_list:						AMTRS_FORCE_LOG("list");		break;
		case vnodetypes::_imm_integer:				AMTRS_FORCE_LOG("integer");		break;
		case vnodetypes::_imm_offset:				AMTRS_FORCE_LOG("offset");		break;
		case vnodetypes::_imm_sizeof:				AMTRS_FORCE_LOG("sizeof");		break;
		case vnodetypes::_imm_string:				AMTRS_FORCE_LOG("string");		break;
		case vnodetypes::_compound:					AMTRS_FORCE_LOG("compound");	break;
		case vnodetypes::_return:					AMTRS_FORCE_LOG("return");		break;
		case vnodetypes::_returnval:				AMTRS_FORCE_LOG("returnval");	break;
		case vnodetypes::_bin_operation_int:		AMTRS_FORCE_LOG("bin");			break;
		case vnodetypes::_address_argvref:			AMTRS_FORCE_LOG("argref");		break;
		case vnodetypes::_address_localref:			AMTRS_FORCE_LOG("localref");	break;
		case vnodetypes::_address_cast:				AMTRS_FORCE_LOG("cast");		break;
		case vnodetypes::_address_memberref:		AMTRS_FORCE_LOG("memberref");	break;
		case vnodetypes::_assign:					AMTRS_FORCE_LOG("assign");		break;
		case vnodetypes::_declare_function:			AMTRS_FORCE_LOG("decl func");	break;
		case vnodetypes::_expr_call:				AMTRS_FORCE_LOG("call");		break;
		case vnodetypes::_expr_new:					AMTRS_FORCE_LOG("new");			break;
		case vnodetypes::_expr_vcall:				AMTRS_FORCE_LOG("vcall");		break;
		case vnodetypes::_load_integral:			AMTRS_FORCE_LOG("loadint");		break;
		case vnodetypes::_local_allocate:			AMTRS_FORCE_LOG("local alloc");	break;
		case vnodetypes::_local_free:				AMTRS_FORCE_LOG("local free");	break;
		case vnodetypes::_operation_single_int:		AMTRS_FORCE_LOG("single int");	break;
		case vnodetypes::_stmt_delete:				AMTRS_FORCE_LOG("delete");		break;
		case vnodetypes::_stmt_if:					AMTRS_FORCE_LOG("if");			break;
		case vnodetypes::_symbol:					AMTRS_FORCE_LOG("symbol");		break;
		case vnodetypes::_this:						AMTRS_FORCE_LOG("this");		break;
		case vnodetypes::_typename:					AMTRS_FORCE_LOG("typename");	break;
		case vnodetypes::_vtable:					AMTRS_FORCE_LOG("vtable");		break;
	}
#endif
	switch (_node.type())
	{
		default :
			AMTRS_FORCE_LOG("run unkown type {}", (int)_node.type());
			break;

		case vn_imm_integer::type :
		{
			_retval.datatype	= _node.datatype();
			_retval.value.i		= _node.at<vn_imm_integer>()->value;
			break;
		}

		case vn_imm_sizeof::type :
		{
			_retval.datatype	= _node.datatype();
			_retval.value.i		= _node.at<vn_imm_sizeof>()->t.size_of();
			break;
		}

		case vn_imm_string::type :
		{
			auto	n	= _node.at<vn_imm_string>();
			_retval.datatype	= _node.datatype();
			_retval.value.p		= const_cast<char*>(n->str.data());
			break;
		}

		case vn_local_allocate::type :
		{
			auto	n	= _node.at<vn_local_allocate>();
			for (auto& var : n->variables)
			{
				auto	top	= mLocalvars.sp();
				local_alloc(_node.datatype());
				auto	end	= mLocalvars.sp();
				add_memory(end, static_cast<size_t>((uintptr_t)top - (uintptr_t)end));
//AMTRS_FORCE_LOG("ALLOC {P} {04X} {}", end, static_cast<size_t>((uintptr_t)top - (uintptr_t)end), (text)_node.datatype().name());
			}

			for (auto& var : n->variables)
			{
				if (var->init)
				{
					run(_retval, var->init);
				}
			}
			break;
		}

		case vn_local_free::type :
		{
			auto	n	= _node.at<vn_local_free>();
			for (auto& var : n->variables)
			{
				if (var->destroy)
				{
					run(_retval, var->destroy);
				}
			}
			for (auto& var : n->variables)
			{
				remove_memory(mLocalvars.sp());
				local_free();
			}
			break;
		}

		case vn_address_argvref::type :
		{
			auto		n	= _node.at<vn_address_argvref>();
			intmax_t	idx	= n->index;
			vmreg&		var	= const_cast<vmreg&>(mArguments[(int)idx]);
			_retval.datatype	= _node.datatype();
			_retval.value.p		= var.v;
			break;
		}

		case vn_address_localref::type :
		{
			auto		n	= _node.at<vn_address_localref>();
			intmax_t	idx	= n->index;
			auto		var	= local(idx);
			_retval.datatype	= _node.datatype();
			_retval.value.p		= var;
			break;
		}

		case vn_address_cast::type :
		{
			auto		n	= _node.at<vn_address_cast>();
			auto		obj	= run(n->value);
			auto		off	= n->offset();
			_retval.datatype	= _node.datatype();
			_retval.value.u		= obj.value.u + off;
			break;
		}

		case vn_address_memberref::type :
		{
			auto		n	= _node.at<vn_address_memberref>();
			auto		obj	= run(n->obj);
			auto		off	= n->offset();
			_retval.datatype	= _node.datatype();
			_retval.value.u		= obj.value.u + off;
			break;
		}

		case vn_assign::type :
		{
			auto		n	= _node.at<vn_assign>();
			vmvariable	dst;	run(dst, n->dest);
			size_t		sz	= _node.datatype().size_of(true);
			vmvariable	src;	run(src, n->src);
			if (!in_memory(dst.value.p, sz))
			{
				memory_error(dst.value.p, _node.location(), true);
				mBoundsError	= _node.location();
				break;
			}
			switch (sz)
			{
				case 1 :	*static_cast<int8_t*>(dst.value.p)		= static_cast<int8_t>(src.value.i);		break;
				case 2 :	*static_cast<int16_t*>(dst.value.p)		= static_cast<int16_t>(src.value.i);	break;
				case 4 :	*static_cast<int32_t*>(dst.value.p)		= static_cast<int32_t>(src.value.i);	break;
				case 8 :	*static_cast<long long*>(dst.value.p)	= static_cast<long long>(src.value.i);	break;
			}
			_retval.datatype	= _node.datatype();
			_retval.value.p		= dst.value.p;
			break;
		}

		case vn_bin_operation_int::type :
		{
			auto		n	= _node.at<vn_bin_operation_int>();
			vmvariable	v1;	run(v1, n->v1);
			vmvariable	v2;	run(v2, n->v2);
			switch (n->operation)
			{
				case operator_type::ot_addition :			_retval.value.i	= v1.value.i +  v2.value.i;	break;
				case operator_type::ot_subtraction :		_retval.value.i	= v1.value.i -  v2.value.i;	break;
				case operator_type::ot_multiplication :		_retval.value.i	= v1.value.i *  v2.value.i;	break;
				case operator_type::ot_division :			_retval.value.i	= v1.value.i /  v2.value.i;	break;
				case operator_type::ot_remainder :			_retval.value.i	= v1.value.i %  v2.value.i;	break;
				case operator_type::ot_left_shift :			_retval.value.i	= v1.value.i << v2.value.i;	break;
				case operator_type::ot_right_shift :		_retval.value.i	= v1.value.i >> v2.value.i;	break;
				case operator_type::ot_three_way_compare :	_retval.value.i	= v1.value.i <  v2.value.i ? -1 : v1.value.i > v2.value.i ? 1 : 0;	break;
				case operator_type::ot_leter :				_retval.value.i	= v1.value.i <  v2.value.i;	break;
				case operator_type::ot_leter_equals :		_retval.value.i	= v1.value.i <= v2.value.i;	break;
				case operator_type::ot_grater :				_retval.value.i	= v1.value.i >  v2.value.i;	break;
				case operator_type::ot_grater_equals :		_retval.value.i	= v1.value.i >= v2.value.i;	break;
				case operator_type::ot_equals :				_retval.value.i	= v1.value.i == v2.value.i;	break;
				case operator_type::ot_not_equals :			_retval.value.i	= v1.value.i != v2.value.i;	break;
				case operator_type::ot_bitwise_and :		_retval.value.i	= v1.value.i &  v2.value.i;	break;
				case operator_type::ot_bitwise_xor :		_retval.value.i	= v1.value.i ^  v2.value.i;	break;
				case operator_type::ot_bitwise_or :			_retval.value.i	= v1.value.i |  v2.value.i;	break;
				case operator_type::ot_logical_and :		_retval.value.i	= v1.value.i && v2.value.i;	break;
				case operator_type::ot_logical_or :			_retval.value.i	= v1.value.i || v2.value.i;	break;
			}
			_retval.datatype = _node.datatype();
			break;
		}

		case vn_compound::type :
		{
			auto		n		= _node.at<vn_compound>();
			vmvariable		dummy;
			vmvariable*		ret		= &_retval;
			for (auto& s : n->statements)
			{
				run(*ret, s);
				if (s.type() == vn_returnval::type)
				{
					ret	= &dummy;
				}
				if (mReturn || mBreak)
				{
					break;
				}
			}
			break;
		}

		case vn_expr_call::type :
		{
			auto	n	= _node.at<vn_expr_call>();
			simplearray<vmreg>	argv;
			for (auto& a : n->arguments)
			{
				vmvariable	v;
				run(v, a);
				argv.push_back(v.value);
			}
			auto	a	= mArguments;
			mArguments	= view<vmreg>(argv.begin(), argv.end());
			add_memory(mArguments.data(), sizeof(vmreg) * mArguments.size());
			if (n->func->native)
			{
				_retval.datatype	= n->func->rettype;
				_retval.value.i		= n->func->native(reinterpret_cast<intmax_t*>(argv.data()), (int)argv.size());
			}
			else
			{
				run(_retval, n->func->statement);
			}
			remove_memory(mArguments.data());
			mReturn		= false;
			mBreak		= false;
			mArguments	= a;
			break;
		}

		case vn_expr_new::type :
		{
			auto	n	= _node.at<vn_expr_new>();
			run(_retval, n->allocate);
			if (n->constractor)
			{
				simplearray<vmreg>	argv;
				argv.push_back(_retval.value);
				for (size_t i = 1; i < n->constractor->arguments.size(); ++i)
				{
					vmvariable	v;
					run(v, n->constractor->arguments[i]);
					argv.push_back(v.value);
				}

				auto	a	= mArguments;
				mArguments	= view<vmreg>(argv.begin(), argv.end());
				add_memory(mArguments.data(), sizeof(vmreg) * mArguments.size());
				if (n->constractor->func->native)
				{
					n->constractor->func->native(reinterpret_cast<intmax_t*>(argv.data()), (int)argv.size());
				}
				else
				{
					vmvariable	v;
					run(v, n->constractor->func->statement);
				}
				remove_memory(mArguments.data());
				mReturn		= false;
				mBreak		= false;
				mArguments	= a;
			}
			break;
		}

		case vn_expr_vcall::type :
		{
			auto	n	= _node.at<vn_expr_vcall>();
			simplearray<vmreg>	argv;
			for (auto& a : n->arguments)
			{
				vmvariable	v;
				run(v, a);
				argv.push_back(v.value);
			}
			auto	a		= mArguments;
			mArguments		= view<vmreg>(argv.begin(), argv.end());
			add_memory(mArguments.data(), sizeof(vmreg) * mArguments.size());

			auto	pvtbl	= reinterpret_cast<intptr_t>(argv[0].p);
			auto	vtbl	= *reinterpret_cast<vdecl_function***>(pvtbl);
			auto	vfnc	= reinterpret_cast<vdecl_function*>(vtbl[n->tblidx]);
			if (vfnc->native)
			{
				_retval.datatype	= vfnc->rettype;
				_retval.value.i		= vfnc->native(reinterpret_cast<intmax_t*>(argv.data()), (int)argv.size());
			}
			else
			{
				run(_retval, vfnc->statement);
			}
			remove_memory(mArguments.data());
			mReturn		= false;
			mBreak		= false;
			mArguments	= a;
			break;
		}

		case vn_load_integral::type :
		{
			auto		n	= _node.at<vn_load_integral>();
			vmvariable	r;	run(r, n->ref);
			_retval.datatype	= _node.datatype();
			auto		sz	= _retval.datatype.size_of();
			if (!in_memory(r.value.p, sz))
			{
				memory_error(r.value.p, _node.location(), false);
				mBoundsError	= _node.location();
				_retval.value.i	= 0;
				break;
			}
			switch (sz)
			{
				case 1 :	_retval.value.i = *static_cast<int8_t*>(r.value.p);		break;
				case 2 :	_retval.value.i = *static_cast<int16_t*>(r.value.p);	break;
				case 4 :	_retval.value.i = *static_cast<int32_t*>(r.value.p);	break;
				case 8 :	_retval.value.i = *static_cast<long long*>(r.value.p);	break;
			}
			break;
		}

		case vn_operation_single_int::type :
		{
			auto	n	= _node.at<vn_operation_single_int>();
			run(_retval, n->val);
			auto		sz	= _retval.datatype.size_of(true);
			if (!in_memory(_retval.value.p, sz))
			{
				memory_error(_retval.value.p, _node.location(), false);
				mBoundsError	= _node.location();
				break;
			}
			if (n->operation == operator_type::ot_dec_prefix)
			{
				switch (sz)
				{
					case 1 :	--(*static_cast<int8_t*>(_retval.value.p));		break;
					case 2 :	--(*static_cast<int16_t*>(_retval.value.p));	break;
					case 4 :	--(*static_cast<int32_t*>(_retval.value.p));	break;
					case 8 :	--(*static_cast<long long*>(_retval.value.p));	break;
				}
			}
			else if (n->operation == operator_type::ot_inc_prefix)
			{
				switch (sz)
				{
					case 1 :	++(*static_cast<int8_t*>(_retval.value.p));		break;
					case 2 :	++(*static_cast<int16_t*>(_retval.value.p));	break;
					case 4 :	++(*static_cast<int32_t*>(_retval.value.p));	break;
					case 8 :	++(*static_cast<long long*>(_retval.value.p));	break;
				}
			}
			// 後置型の場合だけは戻り値が参照になっているのでsize_of()の呼び出しに注意
			else if (n->operation == operator_type::ot_dec_suffix)
			{
				void*	p	= _retval.value.p;
				switch (sz)
				{
					case 1 :	_retval.value.i	= (*static_cast<int8_t*>(p));		--(*static_cast<int8_t*>(p));		break;
					case 2 :	_retval.value.i	= (*static_cast<int16_t*>(p));		--(*static_cast<int16_t*>(p));		break;
					case 4 :	_retval.value.i	= (*static_cast<int32_t*>(p));		--(*static_cast<int32_t*>(p));		break;
					case 8 :	_retval.value.i	= (*static_cast<long long*>(p));	--(*static_cast<long long*>(p));	break;
				}
			}
			else if (n->operation == operator_type::ot_inc_suffix)
			{
				void*	p	= _retval.value.p;
				switch (sz)
				{
					case 1 :	_retval.value.i	= (*static_cast<int8_t*>(p));		++(*static_cast<int8_t*>(p));		break;
					case 2 :	_retval.value.i	= (*static_cast<int16_t*>(p));		++(*static_cast<int16_t*>(p));		break;
					case 4 :	_retval.value.i	= (*static_cast<int32_t*>(p));		++(*static_cast<int32_t*>(p));		break;
					case 8 :	_retval.value.i	= (*static_cast<long long*>(p));	++(*static_cast<long long*>(p));	break;
				}
			}
			break;
		}

		case vn_return::type :
		{
			mReturn	= true;
			break;
		}

		case vn_returnval::type :
		{
			auto	n	= _node.at<vn_returnval>();
			run(_retval, n->expr);
			break;
		}

		case vn_stmt_branch_break::type :
		{
			mBreak	= true;
			break;
		}

		case vn_stmt_delete::type :
		{
			auto	n		= _node.at<vn_stmt_delete>();
			run(_retval, n->value);
#if	0
			if (n->destractor)
			{
				simplearray<vmreg>	argv;
				argv.push_back(_retval.value);
				for (size_t i = 1; i < n->destractor->arguments.size(); ++i)
				{
					vmvariable	v;
					run(v, n->destractor->arguments[i]);
					argv.push_back(v.value);
				}

				auto	a	= mArguments;
				mArguments	= view<vmreg>(argv.begin(), argv.end());
				add_memory(mArguments.data(), sizeof(vmreg) * mArguments.size());
				if (n->destractor->func->native)
				{
					n->destractor->func->native(reinterpret_cast<intmax_t*>(argv.data()), (int)argv.size());
				}
				else
				{
					run(n->destractor->func->statement);
				}
				remove_memory(mArguments.data());
				mArguments	= a;
			}
#endif
#if	1
			if (n->deallocate)
			{
				simplearray<vmreg>	argv;
				argv.push_back(_retval.value);

				auto	a	= mArguments;
				mArguments	= view<vmreg>(argv.begin(), argv.end());
				add_memory(mArguments.data(), sizeof(vmreg) * mArguments.size());
				if (n->deallocate->func->native)
				{
					n->deallocate->func->native(reinterpret_cast<intmax_t*>(argv.data()), (int)argv.size());
				}
				else
				{
					run(n->deallocate->func->statement);
				}
				remove_memory(mArguments.data());
				mArguments	= a;
			}
#endif
			break;
		}

		case vn_stmt_do_while::type :
		{
			auto	n	= _node.at<vn_stmt_do_while>();
			for (;;)
			{
				run(n->statement);
				auto	v	= run(n->conditions);
				if (!v.value.i)
				{
					break;
				}
				if (mBreak)
				{
					break;
				}
			}
			mBreak	= false;
			break;
		}

		case vn_stmt_for::type :
		{
			auto	n	= _node.at<vn_stmt_for>();
			for (;;)
			{
				auto	v	= run(n->conditions);
				if (!v.value.i)
				{
					break;
				}
				run(n->statement);
				if (mBreak)
				{
					break;
				}
				run(n->increment);
			}
			mBreak	= false;
			break;
		}

		case vn_stmt_if::type :
		{
			auto	n	= _node.at<vn_stmt_if>();
			auto	v	= run(n->conditions);
			if (v.value.i)
			{
				run(_retval, n->statement1);
			}
			else
			{
				run(_retval, n->statement2);
			}
			break;
		}

		case vn_stmt_switch::type :
		{
			auto	n		= _node.at<vn_stmt_switch>();
			auto	v		= run(n->conditions);
			bool	active	= false;
			for (auto& c : n->cases)
			{
				if (!active)
				{
					if (!c.value.empty() && run(c.value).value.i == v.value.i)
					{
						active	= true;
					}
					else
					{
						continue;
					}
				}
				run(_retval, c.statements);
				if (mBreak)
				{
					break;
				}
			}
			if (!active)
			{
				for (auto& c : n->cases)
				{
					if (!active)
					{
						if (c.value.empty())
						{
							active	= true;
						}
						else
						{
							continue;
						}
					}
					run(_retval, c.statements);
					if (mBreak)
					{
						break;
					}
				}
			}
			mBreak	= false;
			break;
		}

		case vn_stmt_while::type :
		{
			auto	n	= _node.at<vn_stmt_while>();
			for (;;)
			{
				auto	v	= run(n->conditions);
				if (!v.value.i)
				{
					break;
				}
				run(n->statement);
				if (mBreak)
				{
					break;
				}
			}
			mBreak	= false;
			break;
		}

		case vn_this::type :
		{
			vmreg&		var	= const_cast<vmreg&>(mArguments[0]);
			_retval.datatype	= _node.datatype();
			_retval.value.p		= var.p;
			break;
		}

		case vn_vtable::type :
		{
			auto	n			= _node.at<vn_vtable>();
			auto	cls			= n->cls;
			auto	offset		= n->offset;
			_retval.datatype	= _node.datatype();
			_retval.value.p		= cls->vtbl.data() + offset;
			break;
		}

	}
}


PSNVM_LANG_NAMESPACE_END

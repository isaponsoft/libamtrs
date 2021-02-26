/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__passion__vmbytecode__hpp
#define	__passion__vmbytecode__hpp
#include <amtrs/logging/@>
#include "def.hpp"
#include "vmfunction.hpp"
#include "vmopecode.hpp"
#include "vmshared_data.hpp"
PASSION_NAMESPACE_BEGIN

#ifdef	PASSION_VMBYTECODE_TRACE
#define	PSNVMBC_TRACE_PRINTX86(_x86, _msg,...)		_PSNVMBC_TRACE_PRINT("[%04x][sp%04x] %-10s %-8s" _msg, (int)startIp, (int)_ctx.sp, get_opecodename((opecode_id)opecode), _x86, __VA_ARGS__)
#define	PSNVMBC_TRACE_PRINTX86L2(_x86, _msg,...)	PSNVMBC_TRACE_LOG("%25s %-8s" _msg, "", _x86, __VA_ARGS__)
#define	PSNVMBC_TRACE_PRINT(_msg,...)				_PSNVMBC_TRACE_PRINT("[%04x][sp%04x] %-10s " _msg, (int)startIp, (int)_ctx.sp, get_opecodename((opecode_id)opecode), __VA_ARGS__)
#define	_PSNVMBC_TRACE_PRINT(_msg,...)				PSNVMBC_TRACE_LOG(_msg, __VA_ARGS__);
#else
#define	PSNVMBC_TRACE_PRINT(_msg,...)	
#define	PSNVMBC_TRACE_PRINTX86(_x86, _msg,...)
#define	PSNVMBC_TRACE_PRINTX86L2(_x86, _msg,...)
#endif


struct	vmbc
{
	shared_buffer<uint8_t>			code;


	//! 文字列プールを設定します。
	template<class It>
	void set_string_pool(It _first, It _last)
	{
		mStrings	= shared_buffer<ref<vmobj>>(std::distance(_first, _last));
		for (std::size_t i = 0; i < mStrings.size(); ++i)
		{
			mStrings[i]	= vmstrobj::create(*_first++);
		}
	}

	//! 関数呼び出しテーブルを設定します。
	void set_invoke_entry(passion_function** _first, passion_function** _last)
	{
		mInvokeTable.reserve(std::distance(_first, _last));
		for (passion_function* f : make_bucket(_first, _last))
		{
			mInvokeTable.push_back(f);
		}
	}

	//! 変数のオフセットテーブルを設定します。
	template<class It>
	void set_variable_entry(It _first, It _last)
	{
		// 変数のオフセットテーブルを作成する
		mVarOffetTable.reserve(std::distance(_first, _last));
		for (auto& e : make_bucket(_first, _last))
		{
			mVarOffetTable.push_back(e.clazz ? (size_t)e.clazz->variable_offset(e.name, e.thiz) : 0);
		}
	}

	//! new を呼び出す可能性のあるクラスの一覧
	template<class It>
	void set_class_pool(It _first, It _last)
	{
		mNewClasses	= shared_buffer<vmclass*>(std::distance(_first, _last));
		for (std::size_t i = 0; i < mNewClasses.size(); ++i)
		{
			mNewClasses[i] = *_first++;
		}
	}



	shared_buffer<ref<vmobj>>		mStrings;
	shared_buffer<vmclass*>			mNewClasses;		//!< new で呼び出す可能性のあるクラス
	std::vector<std::size_t>		mVarOffetTable;		//!< メンバ変数のオフセットテーブル vmvariable_entry から生成される
	std::vector<passion_function*>	mInvokeTable;		//!< 呼び出す関数テーブル vmfunction_entry から生成される


	//! バイトコード本体
	context_result execute(passion_context& _ctx) const
	{
		auto					bcode		= this->code.data();
		const auto&				pool		= this->mStrings;
		const auto&				fentry		= this->mInvokeTable;
		const auto&				ventry		= this->mVarOffetTable;


		//
		// ニーモニックを解析
		//
		for (;;)
		{
	#ifdef	PASSION_VMBYTECODE_TRACE
			auto	startIp	= _ctx.ip;
			auto	opecode = static_cast<int>(_ctx.read_uint8(bcode));
			//PSNVMBC_TRACE_LOG("[%04x][sp%02x] %7s", (int)startIp, (int)_ctx.sp, get_opecodename((opecode_id)opecode));
	#else
			auto	opecode = static_cast<int>(_ctx.read_uint8(bcode));
	#endif
			switch (opecode)
			{
				default :
				{
					PSNVMBC_TRACE_LOG("[%04x][sp%02x] Unkown code = %d", (int)startIp, (int)_ctx.sp, opecode);
					break;
				}

				case opecode_id::LDINT8 :
				{
					auto	i	= _ctx.read_int8(bcode);
					_ctx.push(i);
					PSNVMBC_TRACE_PRINTX86("push", "%d ; sp = %p", i, &_ctx.top())
					break;
				}

				case opecode_id::LDINT16 :
				{
					auto	i	= _ctx.read_int16(bcode);
					_ctx.push(i);
					PSNVMBC_TRACE_PRINTX86("push", "%d ; sp = %p", i, &_ctx.top())
					break;
				}

				case opecode_id::LDINT32 :
				{
					auto	i	= _ctx.read_int32(bcode);
					_ctx.push(i);
					PSNVMBC_TRACE_PRINTX86("push", "%d ; sp = %p", i, &_ctx.top())
					break;
				}

				case opecode_id::LDSTR :
				{
					auto	s	= pool[_ctx.read_int16(bcode)];
					PSNVMBC_TRACE_PRINTX86	("new", "rex, %p(%p:'%s')", static_cast<vmobj*>(s), reinterpret_cast<vmstrobj*>(s.get() + 1)->c_str(), reinterpret_cast<vmstrobj*>(s.get() + 1)->c_str());
					_ctx.push(s);
					PSNVMBC_TRACE_PRINTX86L2("push", "rex  ; sp = %p", &_ctx.top());
					s->add_ref();
					break;
				}


				case opecode_id::LDTHIS:
				{
					passion_variant*	arg0	= _ctx.argument_address(0);
					PSNVMBC_TRACE_PRINTX86("lea", "rbx, ss:[rap + 0] ; rbx = %p", arg0)
					vmobj*				obj		= static_cast<vmobj*>(arg0->o);
					PSNVMBC_TRACE_PRINTX86L2("mov", "rax, [rbx] ; rax = %p", obj)
					_ctx.push(obj);
					PSNVMBC_TRACE_PRINTX86L2("push", "rax ; sp = %p", &_ctx.top())
					obj->add_ref();
					break;
				}

				case opecode_id::ADD:
				{
					auto	v2	= _ctx.pop_integer();
					auto	v1	= _ctx.pop_integer();
					PSNVMBC_TRACE_PRINTX86	("pop", "rdx ; rdx = %zd", (size_t)v2);
					PSNVMBC_TRACE_PRINTX86L2("pop", "rax ; rax = %zd", (size_t)v1);
					PSNVMBC_TRACE_PRINTX86L2("add", "rax, rdx", "");
					_ctx.push(v1 + v2);
					PSNVMBC_TRACE_PRINTX86L2("push", "rax ; sp = %p", &_ctx.top());
					break;
				}

				case opecode_id::SUB:
				{
					auto	v2	= _ctx.pop_integer();
					auto	v1	= _ctx.pop_integer();
					PSNVMBC_TRACE_PRINTX86	("pop", "rdx ; rdx = %zd", (size_t)v2);
					PSNVMBC_TRACE_PRINTX86L2("pop", "rax ; rax = %zd", (size_t)v1);
					PSNVMBC_TRACE_PRINTX86L2("sub", "rax, rdx", "");
					_ctx.push(v1 - v2);
					PSNVMBC_TRACE_PRINTX86L2("push", "rax ; sp = %p", &_ctx.top());
					break;
				}

				case opecode_id::MUL:
				{
					auto	v2	= _ctx.pop_integer();
					auto	v1	= _ctx.pop_integer();
					PSNVMBC_TRACE_PRINTX86	("pop", "rdx ; rdx = %zd", (size_t)v2);
					PSNVMBC_TRACE_PRINTX86L2("pop", "rax ; rax = %zd", (size_t)v1);
					PSNVMBC_TRACE_PRINTX86L2("mul", "rax, rdx", "");
					_ctx.push(v1 * v2);
					PSNVMBC_TRACE_PRINTX86L2("push", "rax ; sp = %p", &_ctx.top());
					break;
				}

				case opecode_id::DIV:
				{
					auto	v2	= _ctx.pop_integer();
					auto	v1	= _ctx.pop_integer();
					PSNVMBC_TRACE_PRINTX86	("pop", "rdx ; rdx = %zd", (size_t)v2);
					PSNVMBC_TRACE_PRINTX86L2("pop", "rax ; rax = %zd", (size_t)v1);
					PSNVMBC_TRACE_PRINTX86L2("div", "rax, rdx", "");
					_ctx.push(v1 / v2);
					PSNVMBC_TRACE_PRINTX86L2("push", "rax ; sp = %p", &_ctx.top());
					break;
				}

				case opecode_id::MOD:
				{
					auto	v2	= _ctx.pop_integer();
					auto	v1	= _ctx.pop_integer();
					PSNVMBC_TRACE_PRINTX86	("pop", "rdx ; rdx = %zd", (size_t)v2);
					PSNVMBC_TRACE_PRINTX86L2("pop", "rax ; rax = %zd", (size_t)v1);
					PSNVMBC_TRACE_PRINTX86L2("mod", "rax, rdx", "");
					_ctx.push(v1 % v2);
					PSNVMBC_TRACE_PRINTX86L2("push", "rax ; sp = %p", &_ctx.top());
					break;
				}

				case opecode_id::AND:
				{
					auto	v2	= _ctx.pop_integer();
					auto	v1	= _ctx.pop_integer();
					PSNVMBC_TRACE_PRINTX86	("pop", "rdx ; rdx = %zd", (size_t)v2);
					PSNVMBC_TRACE_PRINTX86L2("pop", "rax ; rax = %zd", (size_t)v1);
					PSNVMBC_TRACE_PRINTX86L2("and", "rax, rdx", "");
					_ctx.push(v1 & v2);
					PSNVMBC_TRACE_PRINTX86L2("push", "rax ; sp = %p", &_ctx.top());
					break;
				}

				case opecode_id::OR:
				{
					auto	v2	= _ctx.pop_integer();
					auto	v1	= _ctx.pop_integer();
					PSNVMBC_TRACE_PRINTX86	("pop", "rdx ; rdx = %zd", (size_t)v2);
					PSNVMBC_TRACE_PRINTX86L2("pop", "rax ; rax = %zd", (size_t)v1);
					PSNVMBC_TRACE_PRINTX86L2("or", "rax, rdx", "");
					_ctx.push(v1 | v2);
					PSNVMBC_TRACE_PRINTX86L2("push", "rax ; sp = %p", &_ctx.top());
					break;
				}

				case opecode_id::XOR:
				{
					auto	v2	= _ctx.pop_integer();
					auto	v1	= _ctx.pop_integer();
					PSNVMBC_TRACE_PRINTX86	("pop", "rdx ; rdx = %zd", (size_t)v2);
					PSNVMBC_TRACE_PRINTX86L2("pop", "rax ; rax = %zd", (size_t)v1);
					PSNVMBC_TRACE_PRINTX86L2("xor", "rax, rdx", "");
					_ctx.push(v1 ^ v2);
					PSNVMBC_TRACE_PRINTX86L2("push", "rax ; sp = %p", &_ctx.top());
					break;
				}

				case opecode_id::SHIFTL:
				{
					auto	v2	= _ctx.pop_integer();
					auto	v1	= _ctx.pop_integer();
					PSNVMBC_TRACE_PRINTX86	("pop", "rdx ; rdx = %zd", (size_t)v2);
					PSNVMBC_TRACE_PRINTX86L2("pop", "rax ; rax = %zd", (size_t)v1);
					PSNVMBC_TRACE_PRINTX86L2("shl", "rax, rdx", "");
					_ctx.push(v1 << v2);
					PSNVMBC_TRACE_PRINTX86L2("push", "rax ; sp = %p", &_ctx.top());
					break;
				}

				case opecode_id::SHIFTR:
				{
					auto	v2	= _ctx.pop_integer();
					auto	v1	= _ctx.pop_integer();
					PSNVMBC_TRACE_PRINTX86	("pop", "rdx ; rdx = %zd", (size_t)v2);
					PSNVMBC_TRACE_PRINTX86L2("pop", "rax ; rax = %zd", (size_t)v1);
					PSNVMBC_TRACE_PRINTX86L2("shr", "rax, rdx", "");
					_ctx.push(v1 >> v2);
					PSNVMBC_TRACE_PRINTX86L2("push", "rax ; sp = %p", &_ctx.top());
					break;
				}

				case opecode_id::ABS:
				{
					auto	v1	= _ctx.pop_integer();
					PSNVMBC_TRACE_PRINT("%zd", (std::intmax_t)v1)
					_ctx.push(std::abs(v1));
					break;
				}

				case opecode_id::NEG:
				{
					auto	v1	= _ctx.pop_integer();
					PSNVMBC_TRACE_PRINT("%zd", (std::intmax_t)v1)
					_ctx.push(-v1);
					break;
				}

				case opecode_id::NOT:
				{
					auto	v1	= _ctx.pop_integer();
					PSNVMBC_TRACE_PRINT("%zd", (std::intmax_t)v1)
					_ctx.push(v1 ? 0 : 1);
					break;
				}

				case opecode_id::CALL:
				{
					auto	findex	= _ctx.read_int16(bcode);
					auto	argc	= _ctx.read_int16(bcode);
					auto	func	= fentry[findex];
					PSNVMBC_TRACE_PRINTX86("call", "[%p] ; %s", func, func->symbol.data())
					_ctx.call(func, argc);
					return	context_result::cr_continue;
				}

				case opecode_id::CALLPTR:
				{
					auto	argc	= _ctx.read_int16(bcode);
					auto	func	= _ctx.pop_function();
					PSNVMBC_TRACE_PRINTX86  ("pop", "rax ; rax = %p", func)
					PSNVMBC_TRACE_PRINTX86L2("call", "[rax], %zd ; %s", argc, func->symbol.data())
					_ctx.call(func, argc);
					return	context_result::cr_continue;
				}

				case opecode_id::VCALL:
				{
					auto	vtidx	= _ctx.read_int16(bcode);
					auto	argc	= _ctx.read_int16(bcode);
					auto*	thisp	= &(&_ctx.top())[argc-1];			// this pointer を格納しているスタック位置
					PSNVMBC_TRACE_PRINTX86  ("lea", "rax, [sp - %d * %d] ; rax = %p, sp = %p", (int)argc-1, (int)sizeof(vmvalue), thisp, &_ctx.top());
					vmobj*	obj		= thisp->o;							// this pointer.
					PSNVMBC_TRACE_PRINTX86L2("mov", "rcx, [rax] ; rcx = %p(this)", obj);
					PSNVMBC_TRACE_PRINTX86L2("mov", "rbx, [rcx] ; rax = %p(vtbl)", obj->vtbl);
					auto	func	= obj->vtbl->functions[vtidx];
					PSNVMBC_TRACE_PRINTX86L2("mov", "rax, [rbx + %zd * %zd] ; rax = %p(vfnc)", (size_t)(vtidx + 1), (size_t)sizeof(void*), func);
					PSNVMBC_TRACE_PRINTX86L2("call", "[rax] ; func = %s", func->symbol.data());
					_ctx.call(func, argc);
					return	context_result::cr_continue;
				}

				case opecode_id::LOAD_FUNC:
				{
					auto	findex	= _ctx.read_int16(bcode);
					auto	func	= fentry[findex];
					PSNVMBC_TRACE_PRINTX86("mov", "rax, %s ; rax = %p", func->symbol.data(), func)
					_ctx.push(passion_variant((void*)func));
					PSNVMBC_TRACE_PRINTX86L2("push", "rax ; sp = %p", &_ctx.top())
					break;
				}

				case opecode_id::RET:
				{
					PSNVMBC_TRACE_PRINTX86("ret", "", 0)
					_ctx.pop_context();
					return	context_result::cr_continue;
				}

				case opecode_id::ALLOC8:
				{
					auto	operand = _ctx.read_int8(bcode);
					PSNVMBC_TRACE_PRINTX86("loop", "%d", (int)operand)
					while (operand > 0)
					{
						_ctx.push((std::intmax_t)0);
						PSNVMBC_TRACE_PRINTX86L2("push", "0 ; sp = %p", &_ctx.top());
						--operand;
					}
					break;
				}

				case opecode_id::ALLOC16:
				{
					auto	operand = _ctx.read_int16(bcode);
					PSNVMBC_TRACE_PRINTX86  ("mov", "[sp], 0 ; sp=%p", &_ctx.top())
					PSNVMBC_TRACE_PRINTX86L2("add", "sp, sp - %d * %d", (int)operand, (int)sizeof(vmvalue))
					while (operand > 0)
					{
						_ctx.push((std::intmax_t)0);
						--operand;
					}
					break;
				}

				case opecode_id::FREE:
				{
					PSNVMBC_TRACE_PRINTX86("add", "sp, sp + %d * %d", (int)1, (int)sizeof(vmvalue))
					_ctx.pop();
					break;
				}

				case opecode_id::FREE8:
				{
					auto	operand = _ctx.read_int8(bcode);
					PSNVMBC_TRACE_PRINTX86("add", "sp, sp + %d * %d", (int)operand, (int)sizeof(vmvalue))
					while (operand > 0)
					{
						_ctx.pop();
						--operand;
					}
					break;
				}

				case opecode_id::FREE16:
				{
					auto	operand = _ctx.read_int16(bcode);
					PSNVMBC_TRACE_PRINT("%d", operand)
					while (operand > 0)
					{
						_ctx.pop();
						--operand;
					}
					break;
				}

				case opecode_id::NEW:
				{
					auto	clz			= mNewClasses[_ctx.read_int16(bcode)];
					auto	obj			= vmobject::create(clz);
					auto	o			= obj.release();
					PSNVMBC_TRACE_PRINTX86("new", "rax, '%s' ; rax = %p", clz->identify().c_str(), o)
					_ctx.push(o);
					PSNVMBC_TRACE_PRINTX86L2("push", "rax ; sp = %p", &_ctx.top())
					break;
				}

				case opecode_id::NEW_A:
				{
					// メモリを指定サイズだけアロケートする
					auto	length	= _ctx.pop_integer();
					auto	stride	= _ctx.pop_integer();
					auto	ary		= vmaryobj::create((size_t)stride, (size_t)length);
					PSNVMBC_TRACE_PRINTX86("newa", "eax, %zd, %zd ; eax = %p", stride, length, ary.get())
					_ctx.push(ary.release());
					break;
				}

				case opecode_id::NEW_AREF:
				{
					// メモリを指定サイズだけアロケートする
					auto	length	= _ctx.pop_integer();
					auto	ary		= vmarygcobj::create(sizeof(ref<vmobject>), (size_t)length);
					PSNVMBC_TRACE_PRINTX86("newa", "eax, %zd, %zd ; eax = %p", sizeof(void*), length, ary.get())
					auto	a		= reinterpret_cast<ref<vmobj>*>(ary->base()->native<vmaryobj>()->data());
					for (auto s = 0; s < length; ++s)
					{
						new (&a[s]) ref<vmobj>();
					}
					_ctx.push(ary.release());
					break;
				}

				case opecode_id::CMPE:
				{
					auto	v2	= _ctx.pop();
					auto	v1	= _ctx.pop();
					PSNVMBC_TRACE_PRINTX86  ("pop", "rax ; rax = %zd", v1)
					PSNVMBC_TRACE_PRINTX86L2("pop", "rbx ; rbx = %zd", v2)
					PSNVMBC_TRACE_PRINTX86L2("setz", "rex, rax, rbx ; rax = %zd", (intmax_t)(v1 == v2))
					_ctx.push(v1 == v2);
					PSNVMBC_TRACE_PRINTX86L2("push", "rax ; sp = %p", &_ctx.top())
					break;
				}

				case opecode_id::CMPN:
				{
					auto	v2	= _ctx.pop();
					auto	v1	= _ctx.pop();
					PSNVMBC_TRACE_PRINTX86  ("pop", "rax ; rax = %zd", v1)
					PSNVMBC_TRACE_PRINTX86L2("pop", "rbx ; rbx = %zd", v2)
					PSNVMBC_TRACE_PRINTX86L2("setnz", "rex, rax, rbx ; rax = %zd", (intmax_t)(v1 != v2))
					_ctx.push(v1 != v2);
					PSNVMBC_TRACE_PRINTX86L2("push", "rax ; sp = %p", &_ctx.top())
					break;
				}

				case opecode_id::CMPS:
				{
					auto	v2	= _ctx.pop_integer();
					auto	v1	= _ctx.pop_integer();
					PSNVMBC_TRACE_PRINTX86  ("pop", "rax ; rax = %zd", v1)
					PSNVMBC_TRACE_PRINTX86L2("pop", "rbx ; rbx = %zd", v2)
					PSNVMBC_TRACE_PRINTX86L2("sets", "rex, rax, rbx ; rax = %zd", (intmax_t)(v1 < v2))
					_ctx.push(v1 < v2);
					PSNVMBC_TRACE_PRINTX86L2("push", "rax ; sp = %p", &_ctx.top())
					break;
				}

				case opecode_id::CMPL:
				{
					auto	v2	= _ctx.pop_integer();
					auto	v1	= _ctx.pop_integer();
					PSNVMBC_TRACE_PRINTX86  ("pop", "rax ; rax = %zd", v1)
					PSNVMBC_TRACE_PRINTX86L2("pop", "rbx ; rbx = %zd", v2)
					PSNVMBC_TRACE_PRINTX86L2("setl", "rex, rax, rbx ; rax = %zd", (intmax_t)(v1 > v2))
					_ctx.push(v1 > v2);
					PSNVMBC_TRACE_PRINTX86L2("push", "rax ; sp = %p", &_ctx.top())
					break;
				}

				case opecode_id::CMPSE:
				{
					auto	v2	= _ctx.pop_integer();
					auto	v1	= _ctx.pop_integer();
					PSNVMBC_TRACE_PRINTX86  ("pop", "rax ; rax = %zd", v1)
					PSNVMBC_TRACE_PRINTX86L2("pop", "rbx ; rbx = %zd", v2)
					PSNVMBC_TRACE_PRINTX86L2("setsz", "rex, rax, rbx ; rax = %zd", (intmax_t)(v1 <= v2))
					_ctx.push(v1 <= v2);
					PSNVMBC_TRACE_PRINTX86L2("push", "rax ; sp = %p", &_ctx.top())
					break;
				}

				case opecode_id::CMPLE:
				{
					auto	v2	= _ctx.pop_integer();
					auto	v1	= _ctx.pop_integer();
					PSNVMBC_TRACE_PRINTX86  ("pop", "rax ; rax = %zd", v1)
					PSNVMBC_TRACE_PRINTX86L2("pop", "rbx ; rbx = %zd", v2)
					PSNVMBC_TRACE_PRINTX86L2("setlz", "rex, rax, rbx ; rax = %zd", (intmax_t)(v1 >= v2))
					_ctx.push(v1 >= v2);
					PSNVMBC_TRACE_PRINTX86L2("push", "rax ; sp = %p", &_ctx.top())
					break;
				}

				case opecode_id::LAND:
				{
					auto	v2	= _ctx.pop_integer();
					auto	v1	= _ctx.pop_integer();
					PSNVMBC_TRACE_PRINTX86  ("pop", "rax ; rax = %zd", v1)
					PSNVMBC_TRACE_PRINTX86L2("pop", "rbx ; rbx = %zd", v2)
					PSNVMBC_TRACE_PRINTX86L2("setand", "rex, rax, rbx ; rax = %zd", (intmax_t)(v1 && v2))
					_ctx.push(v1 && v2);
					PSNVMBC_TRACE_PRINTX86L2("push", "rax ; sp = %p", &_ctx.top())
					break;
				}

				case opecode_id::LOR:
				{
					auto	v2	= _ctx.pop_integer();
					auto	v1	= _ctx.pop_integer();
					PSNVMBC_TRACE_PRINTX86  ("pop", "rax ; rax = %zd", v1)
					PSNVMBC_TRACE_PRINTX86L2("pop", "rbx ; rbx = %zd", v2)
					PSNVMBC_TRACE_PRINTX86L2("setor", "rex, rax, rbx ; rax = %zd", (intmax_t)(v1 || v2))
					_ctx.push(v1 || v2);
					PSNVMBC_TRACE_PRINTX86L2("push", "rax ; sp = %p", &_ctx.top())
					break;
				}

				case opecode_id::JMP8:
				{
					auto	i	= _ctx.read_int8(bcode);
					PSNVMBC_TRACE_PRINTX86("jmp", "%+zd", i)
					_ctx.ip += i;
					break;
				}

				case opecode_id::JMP16:
				{
					auto	i	= _ctx.read_int16(bcode);
					PSNVMBC_TRACE_PRINTX86("jmp", "%+zd", i)
					_ctx.ip += i;
					break;
				}

				case opecode_id::JMP32:
				{
					auto	i	= _ctx.read_int32(bcode);
					PSNVMBC_TRACE_PRINTX86("jmp", "%+zd", i)
					_ctx.ip += i;
					break;
				}

				case opecode_id::JTRUE8:
				{
					auto	operand = _ctx.read_int8(bcode);
					auto	cond	= _ctx.pop_integer();
					PSNVMBC_TRACE_PRINTX86  ("pop", "rax ; rax = %zd", cond)
					PSNVMBC_TRACE_PRINTX86L2("jnz", "%+zd", (intmax_t)operand)
					if (cond != 0)
					{
						_ctx.ip += operand;
					}
					break;
				}

				case opecode_id::JTRUE16:
				{
					auto	operand = _ctx.read_int16(bcode);
					auto	cond	= _ctx.pop_integer();
					PSNVMBC_TRACE_PRINTX86  ("pop", "rax ; rax = %zd", cond)
					PSNVMBC_TRACE_PRINTX86L2("jnz", "%+zd", (intmax_t)operand)
					if (cond != 0)
					{
						_ctx.ip += operand;
					}
					break;
				}

				case opecode_id::JTRUE32:
				{
					auto	operand = _ctx.read_int32(bcode);
					auto	cond	= _ctx.pop_integer();
					PSNVMBC_TRACE_PRINTX86  ("pop", "rax ; rax = %zd", cond)
					PSNVMBC_TRACE_PRINTX86L2("jnz", "%+zd", (intmax_t)operand)
					if (cond != 0)
					{
						_ctx.ip += operand;
					}
					break;
				}

				case opecode_id::JFALSE8:
				{
					auto	operand = _ctx.read_int8(bcode);
					auto	cond	= _ctx.pop_integer();
					PSNVMBC_TRACE_PRINTX86  ("pop", "rax ; rax = %zd", cond)
					PSNVMBC_TRACE_PRINTX86L2("jz", "%+zd", (intmax_t)operand)
					if (cond == 0)
					{
						_ctx.ip += operand;
					}
					break;
				}

				case opecode_id::JFALSE16:
				{
					auto	operand = _ctx.read_int16(bcode);
					auto	cond	= _ctx.pop_integer();
					PSNVMBC_TRACE_PRINTX86  ("pop", "rax ; rax = %zd", cond)
					PSNVMBC_TRACE_PRINTX86L2("jz", "%+zd", (intmax_t)operand)
					if (cond == 0)
					{
						_ctx.ip += operand;
					}
					break;
				}

				case opecode_id::JFALSE32:
				{
					auto	operand = _ctx.read_int32(bcode);
					auto	cond	= _ctx.pop_integer();
					PSNVMBC_TRACE_PRINTX86  ("pop", "rax ; rax = %zd", cond)
					PSNVMBC_TRACE_PRINTX86L2("jz", "%+zd", (intmax_t)operand)
					if (cond == 0)
					{
						_ctx.ip += operand;
					}
					break;
				}


				// 整数を読み込む
				case opecode_id::LOAD_INT:
				{
					PSNVMBC_TRACE_PRINTX86("pop", "ebx ; sp = %p, ebx = %p", &_ctx.top(), _ctx.top().i)
					auto		base	= _ctx.pop_value().p;
					void*		value	= *reinterpret_cast<void**>(base);
					PSNVMBC_TRACE_PRINTX86L2("mov", "rax, [ebx] ; rax = %zx(%zd)", (size_t)value, (size_t)value)
					_ctx.push((vmvalue)(value));
					PSNVMBC_TRACE_PRINTX86L2("push", "rax ; sp = %p", &_ctx.top())
					break;
				}


				// 整数を書き込む
				case opecode_id::STORE_INT:
				{
					PSNVMBC_TRACE_PRINTX86	("pop", "rax ; sp = %p, rax = %p", &_ctx.top(), (size_t)_ctx.top().i)
					intptr_t	v		= _ctx.pop().i;
					PSNVMBC_TRACE_PRINTX86L2("pop", "rdx ; sp = %p, rdx = %p", &_ctx.top(), (size_t)_ctx.top().i)
					auto		base	= _ctx.pop_value().p;
					*reinterpret_cast<intptr_t*>(base)	= v;
					PSNVMBC_TRACE_PRINTX86L2("mov", "[rdx], rax", 0)
					break;
				}

				// 整数を書き込む
				case opecode_id::STORENP_INT:
				{
					PSNVMBC_TRACE_PRINT("", 0)
					auto	v		= _ctx.pop().p;
					auto	base	= _ctx.pop_value().p;
					auto	address	= reinterpret_cast<void**>(base);
					*address	= v;
					break;
				}


				// 整数(32bit)を書き込む
				case opecode_id::STORE_I8:
				{
					auto	v		= _ctx.pop().i;
					auto	base	= _ctx.pop_value().i;
					auto	address	= reinterpret_cast<int8_t*>((std::intptr_t)(base));
					*address	= static_cast<int8_t>(v);
					PSNVMBC_TRACE_PRINTX86	("pop", "edx ; edx = %p", address)
					PSNVMBC_TRACE_PRINTX86L2("pop", "eax ; eax = %d", *address)
					PSNVMBC_TRACE_PRINTX86L2("mov", "[edx], eax", 0)
					break;
				}

				// 整数(32bit)を書き込む
				case opecode_id::STORENP_I8:
				{
					auto	v		= _ctx.pop().i;
					auto	base	= _ctx.pop_value().i;
					auto	address	= reinterpret_cast<int8_t*>((std::intptr_t)(base));
					*address	= static_cast<int8_t>(v);
					PSNVMBC_TRACE_PRINTX86	("pop", "edx ; edx = %p", address)
					PSNVMBC_TRACE_PRINTX86L2("pop", "eax ; eax = %d", *address)
					PSNVMBC_TRACE_PRINTX86L2("mov", "[edx], eax", 0)
					break;
				}

				// 整数(32bit)を読み込む
				case opecode_id::LOAD_I8:
				{
					auto		base	= _ctx.pop_value().p;
					PSNVMBC_TRACE_PRINTX86("pop", "ebx ; ebx = %p", base)
					intmax_t	value	= *reinterpret_cast<int8_t*>(base);
					PSNVMBC_TRACE_PRINTX86L2("mov", "rax, [ebx] ; rax = %zd", (intmax_t)value)
					_ctx.push((vmvalue)(value));
					PSNVMBC_TRACE_PRINTX86L2("push", "rax ; sp = %p", &_ctx.top())
					break;
				}

				// 整数(32bit)を書き込む
				case opecode_id::STORE_I16:
				{
					auto	v		= _ctx.pop().i;
					auto	base	= _ctx.pop_value().i;
					auto	address	= reinterpret_cast<int16_t*>((std::intptr_t)(base));
					*address	= static_cast<int16_t>(v);
					PSNVMBC_TRACE_PRINTX86	("pop", "edx ; edx = %p", address)
					PSNVMBC_TRACE_PRINTX86L2("pop", "eax ; eax = %d", *address)
					PSNVMBC_TRACE_PRINTX86L2("mov", "[edx], eax", 0)
					break;
				}

				// 整数(32bit)を書き込む
				case opecode_id::STORENP_I16:
				{
					auto	v		= _ctx.pop().i;
					auto	base	= _ctx.pop_value().i;
					auto	address	= reinterpret_cast<int16_t*>((std::intptr_t)(base));
					*address	= static_cast<int16_t>(v);
					PSNVMBC_TRACE_PRINTX86	("pop", "edx ; edx = %p", address)
					PSNVMBC_TRACE_PRINTX86L2("pop", "eax ; eax = %d", *address)
					PSNVMBC_TRACE_PRINTX86L2("mov", "[edx], eax", 0)
					break;
				}

				// 整数(32bit)を読み込む
				case opecode_id::LOAD_I16:
				{
					auto		base	= _ctx.pop_value().p;
					PSNVMBC_TRACE_PRINTX86("pop", "ebx ; ebx = %p", base)
					intmax_t	value	= *reinterpret_cast<int16_t*>(base);
					PSNVMBC_TRACE_PRINTX86L2("mov", "rax, [ebx] ; rax = %zd", (intmax_t)value)
					_ctx.push((vmvalue)(value));
					PSNVMBC_TRACE_PRINTX86L2("push", "rax ; sp = %p", &_ctx.top())
					break;
				}


				// 整数(32bit)を書き込む
				case opecode_id::STORE_I32:
				{
					auto	v		= _ctx.pop().i;
					auto	base	= _ctx.pop_value().i;
					auto	address	= reinterpret_cast<int32_t*>((std::intptr_t)(base));
					*address	= static_cast<int32_t>(v);
					PSNVMBC_TRACE_PRINTX86	("pop", "edx ; edx = %p", address)
					PSNVMBC_TRACE_PRINTX86L2("pop", "eax ; eax = %d", *address)
					PSNVMBC_TRACE_PRINTX86L2("mov", "[edx], eax", 0)
					break;
				}

				// 整数(32bit)を書き込む
				case opecode_id::STORENP_I32:
				{
					PSNVMBC_TRACE_PRINT("", 0)
					auto	v		= _ctx.pop().i;
					auto	base	= _ctx.pop_value().i;
					auto	address	= reinterpret_cast<int32_t*>((std::intptr_t)(base));
					*address	= static_cast<int32_t>(v);
					break;
				}

				// 整数(32bit)を読み込む
				case opecode_id::LOAD_I32:
				{
					auto		base	= _ctx.pop_value().p;
					PSNVMBC_TRACE_PRINTX86("pop", "ebx ; ebx = %p", base)
					intmax_t	value	= *reinterpret_cast<int32_t*>(base);
					PSNVMBC_TRACE_PRINTX86L2("mov", "rax, [ebx] ; rax = %zd", (intmax_t)value)
					_ctx.push((vmvalue)(value));
					PSNVMBC_TRACE_PRINTX86L2("push", "rax ; sp = %p", &_ctx.top())
					break;
				}

				// オブジェクトを読み込む
				case opecode_id::LOAD_REF:
				{
					auto	offset	= ventry[_ctx.read_int16(bcode)];
					auto	address	= (vmvalue*)(_ctx.pop_value().p);
					PSNVMBC_TRACE_PRINTX86  ("pop", "rdx ; rdx = %p", address)
					PSNVMBC_TRACE_PRINTX86L2("mov", "rax, [rdx] ; rax = %p", address->o)
					vmobj*	obj		= nullptr;
					if (address->o)
					{
						obj	= reinterpret_cast<vmobj*>(((std::intmax_t)address->o) + offset);
						PSNVMBC_TRACE_PRINTX86L2("lea", "rax, [rax + %zx] ; rax = %p", (std::size_t)offset, obj)
						PSNVMBC_TRACE_PRINTX86L2("ref", "[rax] ; rax = %p, ref = %zd + 1", obj, obj->use_count())
						obj->add_ref();
					}
					_ctx.push(obj);
					PSNVMBC_TRACE_PRINTX86L2("push", "rax ; sp = %p", &_ctx.top())
					break;
				}

				// ポインタを読み込む
				case opecode_id::LOAD_ADR:
				{
					auto	offset	= ventry[_ctx.read_int16(bcode)];
					auto	address	= (passion_variant*)(_ctx.pop_value().p);
					auto*	adr		= reinterpret_cast<void*>(((std::intmax_t)address->p) + offset);
					PSNVMBC_TRACE_PRINTX86  ("pop", "rbx ; rbx = %p", address)
					PSNVMBC_TRACE_PRINTX86L2("mov", "rax, [rbx] ; rax = %p", address->p)
					PSNVMBC_TRACE_PRINTX86L2("lea", "rax, [rax + %zx] ; rax = %p", (std::size_t)offset, adr)
					_ctx.push(passion_variant(adr));
					PSNVMBC_TRACE_PRINTX86L2("push", "rax ; sp = %p", &_ctx.top())
					break;
				}


				// オブジェクトを書き込む
				case opecode_id::STORE_REF:
				{
					auto	v		= vmvalue(_ctx.pop_object());
					auto	address	= (vmvalue*)(_ctx.pop_value().p);
					PSNVMBC_TRACE_PRINTX86  ("pop", "rdx ; rdx = %p", address)
					auto	obj		= (*address).o;
					PSNVMBC_TRACE_PRINTX86L2("mov", "rbx, [rdx] ; rbx = %p (rbx is object pointer)", obj)
					*address	= v;
					PSNVMBC_TRACE_PRINTX86L2("pop", "rax : rax = %p", address->o)
					if (obj)
					{
						PSNVMBC_TRACE_PRINTX86L2("mov", "[rdx], rax ; rdx = %p, rax = %p(%zd)", address, address->o, address->o->use_count())
					}
					else
					{
						PSNVMBC_TRACE_PRINTX86L2("mov", "[rdx], rax ; rdx = %p, rax = %p", address, nullptr)
					}
					if (obj)
					{
						if (obj->use_count() == 1)
						{
							_ctx.push(obj);
							PSNVMBC_TRACE_PRINTX86L2("push", "rbx ; sp = %p, op=%p", &_ctx.top(), obj)
							PSNVMBC_TRACE_PRINTX86L2("call", "deallocator", 0)
							_ctx.call(vmshared_data::get_instance()->deallocator()->function(), 1);
							return	context_result::cr_continue;
						}
						PSNVMBC_TRACE_PRINTX86L2("rel", "[rbx] ; rbx = %p", obj)
						obj->down_ref();
					}
					break;
				}

				// オブジェクトを書き込む
				case opecode_id::STORENP_REF:
				{
					auto	v		= _ctx.top();
					auto	address	= (vmvalue*)(_ctx.pop_value().p);
					PSNVMBC_TRACE_PRINTX86("pop", "r1 ; r1 = %p", address)
					*address	= v;
					PSNVMBC_TRACE_PRINT("[%p] %p", address, *address)
					(*(vmobj**)address)->add_ref();
					break;
				}


				// 配列の先頭アドレスを読み込む
				//	LARRAY
				case opecode_id::LARRAY:
				{
					auto	obj		= _ctx.pop_object();
					PSNVMBC_TRACE_PRINTX86  ("pop", "rax ; rax = %p", obj);
					auto	ao		= obj->base()->native<vmaryobj>()->data();
					PSNVMBC_TRACE_PRINTX86L2("lea", "rax, [rax + 0x%zx] ; rax = %p", (intmax_t)ao - (intmax_t)obj, ao);
					_ctx.push(vmvalue(ao));
					PSNVMBC_TRACE_PRINTX86L2("push", "rax ; sp = %p", &_ctx.top());
					break;
				}


				// アドレス計算を行う
				//	LEA		dest, [src1 + src2 * U8 + S16]
				case opecode_id::LEA:
				{
					auto	scale	= static_cast<uint8_t>(_ctx.read_int8(bcode));
					auto	offset	= _ctx.read_int16(bcode);
					auto	src2	= _ctx.pop().i;
					auto	src1	= _ctx.pop().i;
					PSNVMBC_TRACE_PRINTX86  ("pop", "rbx ; rbx = %zx", (size_t)src2);
					PSNVMBC_TRACE_PRINTX86L2("pop", "rax ; rax = %zx", (size_t)src2);
					auto	dest	= src1 + (src2 * scale) + offset;
					PSNVMBC_TRACE_PRINTX86L2("lea", "rax, [rax + rbx * %x + %x] ; rax = %zx", (int)scale, (int)offset, (size_t)dest);
					_ctx.push(vmvalue(dest));
					PSNVMBC_TRACE_PRINTX86L2("push", "rax ; sp = %p", &_ctx.top())
					break;
				}

				// 引数変数のアドレスを計算する
				case opecode_id::LEA_A:
				{
					auto	offset	= _ctx.read_int16(bcode);
					auto	addr	= _ctx.argument_address(offset);
					PSNVMBC_TRACE_PRINTX86  ("lea", "rax, [ap %c %d * %d] ; rax = %p", (offset > 0 ? '-' : '+'), (int)(offset < 0 ? -offset : offset), (int)sizeof(vmvalue), addr)
					_ctx.push(vmvalue(addr));
					PSNVMBC_TRACE_PRINTX86L2("push", "rax ; sp = %p", &_ctx.top())
					break;
				}

				// ローカル変数のアドレスを計算する
				case opecode_id::LEA_L:
				{
					auto	offset	= _ctx.read_int16(bcode);
					auto	addr	= _ctx.local_address(offset);
					PSNVMBC_TRACE_PRINTX86  ("lea", "rax, [lp %c %d * %d] ; rax = %p", (offset > 0 ? '-' : '+'), (int)(offset < 0 ? -offset : offset), (int)sizeof(vmvalue), addr)
					_ctx.push(vmvalue(addr));
					PSNVMBC_TRACE_PRINTX86L2("push", "rax ; sp = %p", &_ctx.top())
					break;
				}

				// スタック変数のアドレスを計算する
				case opecode_id::LEA_S :
				{
					auto	offset	= _ctx.read_int16(bcode);
					auto	addr	= _ctx.stack_address(offset);
					PSNVMBC_TRACE_PRINTX86  ("lea", "rax, [sp %c %d * %d] ; rax = %p", (offset > 0 ? '-' : '+'), (int)(offset < 0 ? -offset : offset), (int)sizeof(vmvalue), addr)
					_ctx.push(vmvalue(addr));
					PSNVMBC_TRACE_PRINTX86L2("push", "rax ; sp = %p", &_ctx.top())
					break;
				}

				// オブジェクトのメンバ変数のアドレスを計算する
				case opecode_id::LEA_O :
				{
					auto	vti		= _ctx.read_int16(bcode);
					auto	obj		= _ctx.pop_object();
					PSNVMBC_TRACE_PRINTX86  ("pop", "rcx ; rcx = %p", obj)
					auto	addr	= reinterpret_cast<std::intmax_t>(obj) + ventry[vti];
					PSNVMBC_TRACE_PRINTX86L2("lea", "rax, [rcx + %d] ; rax = %p", (int)ventry[vti], addr)
					_ctx.push(vmvalue(addr));
					PSNVMBC_TRACE_PRINTX86L2("push", "rax ; sp = %p", &_ctx.top())
					obj->down_ref();
					break;
				}

				// グローバル変数のアドレスを計算する
				case opecode_id::LEA_G :
				{
					auto	vti		= _ctx.read_int16(bcode);
					auto	addr	= reinterpret_cast<std::intmax_t>(_ctx.variables()) + ventry[vti];
					PSNVMBC_TRACE_PRINTX86("mov", "rax, %p", addr)
					_ctx.push(vmvalue(addr));
					PSNVMBC_TRACE_PRINTX86L2("push", "rax ; sp = %p", &_ctx.top())
					break;
				}


				// 配列変数のアドレスを計算する
				case opecode_id::LEA_AI32:
				{
					auto	index	= _ctx.pop_integer();
					auto	obj		= _ctx.pop_object();
					auto	ao		= obj->base()->native<vmaryobj>()->data();
					auto	addr	= reinterpret_cast<std::intmax_t>(ao) + index * sizeof(int32_t);
					PSNVMBC_TRACE_PRINT("%zx * 4 [%p]", (std::size_t)index, addr)
					_ctx.push(vmvalue(addr));
					break;
				}

				// 配列変数のアドレスを計算する
				case opecode_id::LEA_AREF:
				{
					auto	index	= _ctx.pop_integer();
					PSNVMBC_TRACE_PRINTX86("pop", "rsi ; rsi = %zd", (std::size_t)index);
					auto	obj		= _ctx.pop_object();
					PSNVMBC_TRACE_PRINTX86L2("pop", "rbx ; rbx = %p", obj);
					auto	ao		= obj->base()->native<vmaryobj>()->data();
					auto	addr	= reinterpret_cast<std::intmax_t>(ao) + index * sizeof(ref<vmobject>);
					PSNVMBC_TRACE_PRINTX86L2("lea", "rax, [rbx + rsi + 0x%zx] ; rax = %p", (intmax_t)ao - (intmax_t)obj, addr);
					_ctx.push(vmvalue(addr));
					PSNVMBC_TRACE_PRINTX86L2("push", "rax ; sp = %p", &_ctx.top());
					break;
				}

				// アドレスが示すINTの値を増やす
				case opecode_id::INC_INT:
				{
					auto	num		= _ctx.pop_integer();
					PSNVMBC_TRACE_PRINTX86	("pop",	"eax ; eax=%zd", (size_t)num)
					auto	address	= reinterpret_cast<intmax_t*>(_ctx.pop_value().p);
					PSNVMBC_TRACE_PRINTX86L2("pop",	"edx ; edx=%zd", address)
					PSNVMBC_TRACE_PRINTX86L2("add",	"[edx], eax", address)
					(*address) += static_cast<intmax_t>(num);
					break;
				}

				// アドレスが示すINT32の値を増やす
				case opecode_id::INC_I8:
				{
					auto	num		= _ctx.pop_integer();
					PSNVMBC_TRACE_PRINTX86	("pop",	"eax ; eax=%zd", (size_t)num)
					auto	address	= reinterpret_cast<int8_t*>(_ctx.pop_value().p);
					PSNVMBC_TRACE_PRINTX86L2("pop",	"edx ; edx=%zd", address)
					PSNVMBC_TRACE_PRINTX86L2("add",	"[edx], eax", address)
					(*address) += static_cast<int8_t>(num);
					break;
				}

				// アドレスが示すINT32の値を増やす
				case opecode_id::INC_I16:
				{
					auto	num		= _ctx.pop_integer();
					PSNVMBC_TRACE_PRINTX86	("pop",	"eax ; eax=%zd", (size_t)num)
					auto	address	= reinterpret_cast<int16_t*>(_ctx.pop_value().p);
					PSNVMBC_TRACE_PRINTX86L2("pop",	"edx ; edx=%zd", address)
					PSNVMBC_TRACE_PRINTX86L2("add",	"[edx], eax", address)
					(*address) += static_cast<int16_t>(num);
					break;
				}

				// アドレスが示すINT32の値を増やす
				case opecode_id::INC_I32:
				{
					auto	num		= _ctx.pop_integer();
					PSNVMBC_TRACE_PRINTX86	("pop",	"eax ; eax=%zd", (size_t)num)
					auto	address	= reinterpret_cast<int32_t*>(_ctx.pop_value().p);
					PSNVMBC_TRACE_PRINTX86L2("pop",	"edx ; edx=%zd", address)
					PSNVMBC_TRACE_PRINTX86L2("add",	"[edx], eax", address)
					(*address) += static_cast<int32_t>(num);
					break;
				}

				// アドレスが示すINT32の値を増やす
				case opecode_id::INC_I64:
				{
					auto	num		= _ctx.pop_integer();
					PSNVMBC_TRACE_PRINTX86	("pop",	"eax ; eax=%zd", (size_t)num)
					auto	address	= reinterpret_cast<int64_t*>(_ctx.pop_value().p);
					PSNVMBC_TRACE_PRINTX86L2("pop",	"edx ; edx=%zd", address)
					PSNVMBC_TRACE_PRINTX86L2("add",	"[edx], eax", address)
					(*address) += static_cast<int64_t>(num);
					break;
				}

				// アドレスが示すINT32の値を増やす
				case opecode_id::DEC_INT:
				{
					auto	num		= _ctx.pop_integer();
					PSNVMBC_TRACE_PRINTX86	("pop",	"eax ; eax=%zd", (size_t)num)
					auto	address	= reinterpret_cast<intmax_t*>(_ctx.pop_value().p);
					PSNVMBC_TRACE_PRINTX86L2("pop",	"edx ; edx=%zd", address)
					PSNVMBC_TRACE_PRINTX86L2("sub",	"[edx], eax", address)
					(*address) -= static_cast<intmax_t>(num);
					break;
				}

				// アドレスが示すINT32の値を増やす
				case opecode_id::DEC_I8:
				{
					auto	num		= _ctx.pop_integer();
					PSNVMBC_TRACE_PRINTX86	("pop",	"eax ; eax=%zd", (size_t)num)
					auto	address	= reinterpret_cast<int8_t*>(_ctx.pop_value().p);
					PSNVMBC_TRACE_PRINTX86L2("pop",	"edx ; edx=%zd", address)
					PSNVMBC_TRACE_PRINTX86L2("sub",	"[edx], eax", address)
					(*address) -= static_cast<int8_t>(num);
					break;
				}

				// アドレスが示すINT32の値を増やす
				case opecode_id::DEC_I16:
				{
					auto	num		= _ctx.pop_integer();
					PSNVMBC_TRACE_PRINTX86	("pop",	"eax ; eax=%zd", (size_t)num)
					auto	address	= reinterpret_cast<int16_t*>(_ctx.pop_value().p);
					PSNVMBC_TRACE_PRINTX86L2("pop",	"edx ; edx=%zd", address)
					PSNVMBC_TRACE_PRINTX86L2("sub",	"[edx], eax", address)
					(*address) -= static_cast<int16_t>(num);
					break;
				}

				// アドレスが示すINT32の値を増やす
				case opecode_id::DEC_I32:
				{
					auto	num		= _ctx.pop_integer();
					PSNVMBC_TRACE_PRINTX86	("pop",	"eax ; eax=%zd", (size_t)num)
					auto	address	= reinterpret_cast<int32_t*>(_ctx.pop_value().p);
					PSNVMBC_TRACE_PRINTX86L2("pop",	"edx ; edx=%zd", address)
					PSNVMBC_TRACE_PRINTX86L2("sub",	"[edx], eax", address)
					(*address) -= static_cast<int32_t>(num);
					break;
				}

				// アドレスが示すINT32の値を増やす
				case opecode_id::DEC_I64:
				{
					auto	num		= _ctx.pop_integer();
					PSNVMBC_TRACE_PRINTX86	("pop",	"eax ; eax=%zd", (size_t)num)
					auto	address	= reinterpret_cast<int64_t*>(_ctx.pop_value().p);
					PSNVMBC_TRACE_PRINTX86L2("pop",	"edx ; edx=%zd", address)
					PSNVMBC_TRACE_PRINTX86L2("sub",	"[edx], eax", address)
					(*address) -= static_cast<int64_t>(num);
					break;
				}

				// スタックトップのアドレスを重複させる
				case opecode_id::DUP_ADR:
				{
					PSNVMBC_TRACE_PRINTX86	("mov",	"rax, [sp] ; eax=%p", (size_t)_ctx.top().i)
					_ctx.push(_ctx.top());
					PSNVMBC_TRACE_PRINTX86L2("push", "rax ; sp = %p", &_ctx.top());
					break;
				}


				case opecode_id::REFINT:
				{
					PSNVMBC_TRACE_PRINT("", 0)
					_ctx.push(_ctx.pop());
					break;
				}


				case opecode_id::DELETE_REF :
				{
					auto	address	= reinterpret_cast<vmobj**>(_ctx.pop().p);
					PSNVMBC_TRACE_PRINTX86("pop", "rbx ; rbx = %p", address)
					auto	obj	= *address;
					PSNVMBC_TRACE_PRINTX86L2("mov", "rax, [rbx] ; eax = %p", obj)
					if (obj)
					{
						PSNVMBC_TRACE_PRINTX86L2("del", "rax ; %p(%zd)", obj, obj->use_count())
						*address	= nullptr;
						if (obj->use_count() == 1)
						{
							_ctx.push(obj);
							PSNVMBC_TRACE_PRINTX86L2("push", "rax ; sp = %p", &_ctx.top())
							PSNVMBC_TRACE_PRINTX86L2("call", "deallocator", 0)
							_ctx.call(vmshared_data::get_instance()->deallocator()->function(), 1);
							return	context_result::cr_continue;
						}
						obj->down_ref();
					}
					break;
				}
			}
		}
	}
};



struct	vmbytecode
		: vmfunction
{
	vmbc	bc;

	template<class... Args>
	vmbytecode(Args... _args)
		: vmfunction(_args...)
	{
		mFunctionInfo->data		= &bc;
		mFunctionInfo->function	= [](struct passion_context* _context, void* _data) -> int
								{
									return	reinterpret_cast<vmbc*>(_data)->execute(*_context);
								};
	}
};

PASSION_NAMESPACE_END
#endif

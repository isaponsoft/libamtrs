/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__lang__passion__vm_vmstate__hpp
#define	__libamtrs__lang__passion__vm_vmstate__hpp
#include <tuple>
#include <utility>
PSNVM_NAMESPACE_BEGIN
namespace vmni {
struct	pv_module;
}

struct	vmregmap
{
	enum	reg_type
	{
		reg_rax	= 0,		// accumelator
		reg_rbx	= 1,		// base
		reg_rcx	= 2,		// context
	};

	vmreg				rax;					// acucmelator     (return value)
	vmreg				rbx;					// base pointer    (arguments pointer) .. 最初の引数の位置を示す
	vmreg				rcx;					// context pointer (this pointer)      .. クラスメンバの場合、thisポインタを示す

	vmreg& operator [] (size_t i) { return (&rax)[i]; }

	static char const* get_regname(reg_type _r) noexcept;
	static reg_type to_regtype(text _name) noexcept;
};

struct	vmstate
{
	// コンパイル中のスタックの情報を示す構造体
	struct	stackdata
	{
		enum stack_types
		{
			stk_unuse				= 0,			//!< 未使用
			stk_temp				= 1,			//!< 演算などによる一時利用
			stk_arg					= 2,			//!< 引数
			stk_this				= 3,			//!< this
			stk_returnref			= 4,			//!< 戻り値の格納先
			stk_returnpoint			= 5,			//!< 戻り先
			stk_local				= 6,			//!< ローカル変数
			stk_exception			= 7,			//!< 例外
			stk_caller				= 8,			//!< 呼び出し元の位置
		};
		using	dtor_type		= void(void*);

		stack_types				type;
		sstring					identify;				// 識別名
		sstring					datatype;
		size_t					size;					// 値のサイズ
		vmptr					ptr		= nullptr;		// スタックの位置


		stackdata() = default;
		stackdata(stackdata const&) = delete;
		stackdata(stackdata&& r)
			: type(std::move(r.type))
			, identify(std::move(r.identify))
			, datatype(std::move(r.datatype))
			, size(std::move(r.size))
			, ptr(std::move(r.ptr))
		{
			r.ptr	= nullptr;
		}
	};

	struct	modulestate
	{
		vmni::pv_module const*	mod		= nullptr;
		bool					loaded	= false;	// すでにロード済みなら true
	};

	vmclassmgr					classes;
	functionlist				functions;
	stackbuffer					stack;			
	stackbuffer					stackinfo;				// スタックの状態を計算するための作業データ
	vmregmap					reg;
	simplearray<modulestate>	modules;				// モジュールのリスト

	PSNVM_API vmstate();
	PSNVM_API vmstate(vmstate* _parent);
	PSNVM_API ~vmstate();
	PSNVM_API errorval module_import(vmni::pv_module const*	mod);
	PSNVM_API errorval module_import(text _module_name, text _path);
	PSNVM_API void reset();

	// コアライブラリを追加します。
	PSNVM_API void installcore();


	size_t stack_size()
	{
		return	(reinterpret_cast<intmax_t>(stackinfo.top()) - reinterpret_cast<intmax_t>(stackinfo.sp())) / sizeof(stackdata);
	}

	stackdata stack_pop()
	{
		stackdata	r	= std::move(*stackinfo.template pop<stackdata>());
		stack.free(r.size);
		return	r;
	}

	stackdata* stkalloc(stackdata::stack_types _type, sstring _datatype, sstring _identify, size_t _size)
	{
		void*		o	= stack.alloc(_size);
		stackdata*	r	= new (stackinfo.template push<stackdata>()) stackdata ();
		r->type		= _type;
		r->datatype	= std::move(_datatype);
		r->identify	= std::move(_identify);
		r->size		= _size;
		r->ptr		= o;
		return	r;
	}

	template<class T, class... Args>
	stackdata* stkpush(stackdata::stack_types _type, sstring _datatype, sstring _identify, Args&&... _args)
	{
		stackdata*	s	= stkalloc(_type, std::move(_datatype), std::move(_identify), sizeof(T));
		new (s->ptr) T (std::forward<Args>(_args)...);
		return	s;
	}

	stackdata* stack_current()
	{
		return	reinterpret_cast<stackdata*>(stackinfo.sp());
	}

	stackdata* stack_top()
	{
		return	reinterpret_cast<stackdata*>(stackinfo.top());
	}
};
PSNVM_NAMESPACE_END
#endif

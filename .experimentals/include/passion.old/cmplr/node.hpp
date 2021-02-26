/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__passion__compiler__node__hpp
#define	__passion__compiler__node__hpp
#include "types.hpp"


PASSION_NAMESPACE_BEGIN


template<class Compiler>
class	node;

template<class Compiler>
struct	expression_traits
{
	using	traits_type		= expression_traits<Compiler>;
	using	node_type		= node<Compiler>;
	using	compiler_type	= Compiler;
	using	input_type		= const Token;
};


template<class Compiler, class Traits = expression_traits<Compiler>>
struct	statement;

template<class Compiler, class Traits = expression_traits<Compiler>>
struct	codeblock;

template<class Compiler, class Traits = expression_traits<Compiler>>
struct	typeidentify;

template<class Compiler, class Traits = expression_traits<Compiler>>
struct	decl_class;

template<class Compiler, class Traits = expression_traits<Compiler>>
struct	globalscope;


template<class Compiler>
struct	parser_result
{
	static constexpr int	phase_start	= 0;
	static constexpr int	phase_end	= 1;
	static constexpr int	phase_pass	= 2;

	template<class Phase>
	parser_result(Phase _phase) : phase((int)_phase) {}

	template<class Phase, class Node>
	parser_result(Phase _phase, Node _node) : phase((int)_phase), node(std::move(_node)) {}

	int					phase;
	ref<node<Compiler>>	node;
};



template<class Compiler>
class	node
		: public ref_object
{
public:
	using	compiler_type	= Compiler;
	using	input_type		= const Token;
	using	node_type		= node<Compiler>;
	using	location_type	= location;
	using	result_type		= parser_result<compiler_type>;

	static constexpr int	phase_start	= result_type::phase_start;
	static constexpr int	phase_end	= result_type::phase_end;
	static constexpr int	phase_pass	= result_type::phase_pass;


	node_type* parent() const noexcept { return mParent; }



	// ========================================================================
	//! バイトコードへのコンパイルを行います。
	// ------------------------------------------------------------------------
	void compile(compiler_type& _cmplr, assembler& _asmblr)
	{
		PSNCMPLR_TRACE_LOG("compile [%p] %s", this, typeid(*this).name());
		on_compile(_cmplr, _asmblr);
	}


	// ========================================================================
	//! 型情報を取得します。
	// ------------------------------------------------------------------------
	const vmstring& type() const noexcept { if (mType.empty()) { mType = on_type(); } return mType; }


	// ========================================================================
	//! 定義の識別をフルの名前で返します。
	// ------------------------------------------------------------------------
	//! クラスであればクラス名になり、関数であればクラス名+関数名+シグネチャになります。
	//! 識別子を持たないノードは親ノードの識別子になります。
	// ------------------------------------------------------------------------
	virtual vmstring identify() const { return this->parent() ? vmstring(this->parent()->identify()) : vmstring(); }


	// ========================================================================
	//! 構文解析を行います。
	// ------------------------------------------------------------------------
	ref<node_type> lex(compiler_type& _cmplr, input_type& _token)
	{
		PSNCMPLR_TRACE_LOG("%s  : [%p] %s, phase=%d", _token.str().c_str(), this, typeid(*this).name(), mPhase);
		result_type	next	= on_parse(mPhase, _cmplr, _token);
		if (next.phase == -1)
		{
			return	this;
		}
		mPhase	= (int)next.phase;
		if (next.node)
		{
			return	next.node;
		}
		if (mPhase == (int)phase_end)
		{
			return	this->parent();
		}
		if (mPhase == (int)phase_pass)
		{
			mPhase	=  (int)phase_end;
			return	this->parent()
					? this->parent()->lex(_cmplr, _token)
					: nullptr;
		}
		return	this;
	}


	compute_type get_compute_type() const noexcept { return on_compute_type(); }


	vmvariant get_static_variable() const noexcept { return on_constexpr_value(); }


	value_mode get_value_type() const noexcept { return mReturnMode; }


	opecode_priority get_priority() const noexcept { return mPriority; }


	void set_priority(opecode_priority _priority) noexcept { mPriority = _priority; }


	template<class Expr, class... Opts>
	ref<Expr> create(Opts&&... _opts)
	{
		return	create<Expr>(this->get_value_type(), std::forward<Opts>(_opts)...);
	}


	template<class Expr, class... Opts>
	ref<Expr> create(value_mode _vtype, Opts&&... _opts)
	{
		ref<Expr>		thiz	= new Expr(std::forward<Opts>(_opts)...);
		thiz->mParent		= this;
		thiz->mReturnMode	= _vtype;
		return	thiz;
	}


	template<class Expr, class... Opts>
	ref<Expr> create_if(value_mode _vtype, compiler_type& _cmplr, const Token& _token, Opts&&... _opts)
	{
		ref<Expr>		thiz;
		if (Expr::is_start(_cmplr, _token, std::forward<Opts>(_opts)...))
		{
			thiz	= new Expr(std::forward<Opts>(_opts)...);
			thiz->mParent		= this;
			thiz->mReturnMode	= _vtype;
		}
		return	thiz;
	}


	template<class Expr, class... Opts>
	ref<Expr> create_if(compiler_type& _cmplr, const Token& _token, Opts&&... _opts)
	{
		return	create_if<Expr>(this->get_value_type(), _cmplr, _token, std::forward<Opts>(_opts)...);
	}


protected:
	virtual void on_compile(compiler_type& _cmplr, assembler& _asmblr) {}
	virtual compute_type on_compute_type() const noexcept { return compute_type::dynamics; }
	virtual vmstring on_type() const = 0;
	virtual vmvariant on_constexpr_value() const noexcept { return vmvariant(); }
	virtual result_type on_parse(int _phase, compiler_type& _cmplr, input_type& _token) = 0;


private:
	node_type*			mParent			= nullptr;
	opecode_priority	mPriority		= opecode_priority::none;
	value_mode			mReturnMode		= value_mode::void_mode;
	mutable vmstring	mType;
	int					mPhase			= phase_start;
};


#define	PSNLEX_ERROR(_code, ...)	PSNCMPLR_SYNTAX_ERROR(_cmplr, _lex.location(), _code, __VA_ARGS__)

#ifdef	PASSION_COMPILER_SHOW_ERRORPOINT
	#define	PSNCMPLR_SYNTAX_ERROR(_cmplr, _locate, _code, ...)	\
				_cmplr.set_error(new typename std::remove_reference<decltype(_cmplr)>::type::error(\
					_cmplr.location().name(),\
					_cmplr.location().line(),\
					error_code::_code,\
					format<std::string>(get_errorcode_name(error_code::_code), __VA_ARGS__),\
					__FILE__, __LINE__\
				));\
				return	-1;
#else
	#define	PSNCMPLR_SYNTAX_ERROR(_cmplr, _locate, _code, ...)	\
				_cmplr.set_error(new typename std::remove_reference<decltype(_cmplr)>::type::error(\
					_cmplr.location().name(),\
					_cmplr.location().line(),\
					error_code::_code,\
					format<std::string>(get_errorcode_name(error_code::_code), __VA_ARGS__)\
				));\
				return	-1;
#endif




PASSION_NAMESPACE_END
#endif

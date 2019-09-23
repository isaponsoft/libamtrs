#ifndef	__passion__compiler__types__hpp
#define	__passion__compiler__types__hpp
#include "../err/error.hpp"
#include "def.hpp"
PASSION_NAMESPACE_BEGIN

using namespace asmblr;


enum class value_mode
{
	rvalue,			//!< 右辺値であることを要求する
	lvalue,			//!< 左辺値になれることを要求する。左辺値への参照を返す。
	void_mode,		//!< 価を返す必要が無い
};



enum class compute_type
{
	end				= -2,		// returnやbreakなどでこれ以降のコードは無効
	none			= -1,		// 値を返さない
	dynamics		= 0,		// 動的な値
	static_integer	= 1,		// 静的に求められる整数値
	static_string	= 2,		// 静的に求められる文字列値
};



enum class opecode_priority
{
	none,						//!< 0: 値(優先順位には影響しない)
	assign,						//!< 1: = += -= *= /= %= &= |= ^= <<= >>=
	conditions = assign,		//!< 1: ?:
	logic_and,					//!< 2: && ||
	compare,					//!< 3: == != <= >= < >
	bitwise,					//!< 4: & | ^
	shift,						//!< 5: << >>
	add_sub,					//!< 6: + -
	mul_div,					//!< 7: * / %
	single_operation,			//!< 8: ! ~ - *pointer &pointer
	primary,					//!< 9: () や [] など最優先のもの
};





// ****************************************************************************
//! 型を返す。エイリアスを指定した場合はエイリアスを展開する。
// ----------------------------------------------------------------------------
//! 型名ではない場合は empty() を返す。
// ----------------------------------------------------------------------------
template<class Compiler>
vmstring add_reference(Compiler& _cmplr, vmstring _typename);


// ****************************************************************************
//! 型を返す。エイリアスを指定した場合はエイリアスを展開する。
// ----------------------------------------------------------------------------
//! 型名ではない場合は empty() を返す。
// ----------------------------------------------------------------------------
template<class Compiler>
vmstring extract_typename(Compiler& _cmplr, amtrs_string_view _typename);


// ****************************************************************************
//! 型の代入が可能か問い合わせる。
// ----------------------------------------------------------------------------
template<class Compiler>
bool is_assign_type(Compiler& _cmplr, amtrs_string_view _to, amtrs_string_view _from);


// ****************************************************************************
//! 型エイリアスかどうか調べエイリアスなら元の型をバッファへ格納する
// ----------------------------------------------------------------------------
//! エイリアスでない場合は false を返し何もしない。
// ----------------------------------------------------------------------------
template<class Compiler>
bool is_type_alias(vmstring* _buffer, Compiler& _cmplr, amtrs_string_view _type);


template<class Compiler>
inline vmstring add_reference(Compiler& _cmplr, vmstring _typename)
{
	return	passion_is_reference(extract_typename(_cmplr, _typename)) ? _typename : vmstring(_typename + "&");
}



template<class Compiler>
inline vmstring extract_typename(Compiler& _cmplr, amtrs_string_view _typename)
{
	vmstring		type	= _typename;
	for (;;)
	{
		if (passion_is_reference(type))
		{
			return	extract_typename(_cmplr, passion_trim_reference(type)) + "&";
		}
		if (passion_is_pointer(type))
		{
			return	extract_typename(_cmplr, passion_trim_pointer(type)) + "*";
		}
		if (passion_is_gcpointer(type))
		{
			return	extract_typename(_cmplr, passion_trim_gcpointer(type)) + "^";
		}
		struct passion_functiontype_info	info;
		if (passion_is_function(&info, type))
		{
			shared_buffer<amtrs_string_view>	params	= shared_buffer<amtrs_string_view>(info.parameter_size);
			passion_split_parameter_types(params.data(), &info);
			vmstring	retval	=  extract_typename(_cmplr, info.result_type) + "(";
			bool		first	= true;
			for (auto& p : params)
			{
				if (!first)
				{
					retval	+= ",";
				}
				first	= false;
				retval += extract_typename(_cmplr, extract_typename(_cmplr, p));
			}
			retval += ")";
			return	retval;
		}

		passion_symbole*	s	= _cmplr.find_symbole(type);
		if (s && s->type == passion_symbole_type_alias)
		{
			type	= vmstring(*(s->data.alias));
			continue;
		}
		return	type;
	}
}



template<class Compiler>
inline bool is_assign_type(Compiler& _cmplr, amtrs_string_view _to, amtrs_string_view _from)
{
	vmstring	toType	= extract_typename(_cmplr, _to);


	// nullptr はポインタ、参照型に対して常に代入可能。
	if ((passion_is_pointer(toType) || passion_is_gcpointer(toType) || passion_is_reference(toType)) && (_from == "nullptr_t"))
	{
		return	true;
	}

	auto	to		= passion_remove_reference(toType);
	auto	from	= passion_remove_reference(extract_typename(_cmplr, _from));
	if (to == from)
	{
		return	true;
	}

	// 左辺が非参照／ポインタの時は右辺のconstは外せる
	if (!passion_is_reference(toType) && !passion_is_pointerable(toType))
	{
		if (to == vmstring(passion_trim_const(from)))
		{
			return	true;
		}
	}


	// ポインタに対しては gc pointer と 配列が代入可能
	if (passion_is_pointer(to) && passion_is_gcpointer(from))
	{
		vmclass*	tc	= _cmplr.find_class(to.remove_pointer());
		vmclass*	fc	= _cmplr.find_class(from.remove_pointer());
		if (fc->is_base_of(tc->name()))
		{
			return	true;
		}
	}
	if (passion_is_pointer(to) && passion_is_array(from))
	{
		vmclass*	tc	= _cmplr.find_class(to.remove_pointer());
		vmclass*	fc	= _cmplr.find_class(vmstring(passion_trim_array_last_dimention(from)));
		if (fc->is_base_of(tc->name()))
		{
			return	true;
		}
	}
	if (passion_is_gcpointer(to) && passion_is_gcpointer(from))
	{
		vmclass*	tc	= _cmplr.find_class(to.remove_pointer());
		vmclass*	fc	= _cmplr.find_class(from.remove_pointer());
		if (fc->is_base_of(tc->name()))
		{
			return	true;
		}
	}
	if (toType == "auto")
	{
		return	true;
	}

	return	false;
}


template<class Compiler>
bool is_type_alias(vmstring* _buffer, Compiler& _cmplr, amtrs_string_view _typename)
{
	amtrs_string_view		type	= _typename;
	bool					alias	= false;
	for (;;)
	{
		passion_symbole*	s	= _cmplr.find_symbole(type);
		if (s && s->type == passion_symbole_type_alias)
		{
			if (!_buffer)
			{
				return	true;
			}
			type		= amtrs_string_to_string_view(s->data.alias);
			*_buffer	= type;
			alias		= true;
		}
		else
		{
			return	alias;
		}
	}
}





PASSION_NAMESPACE_END
#endif

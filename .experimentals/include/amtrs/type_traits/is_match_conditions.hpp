/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef __libamtrs__type_utils__is_match_conditions__hpp
#define __libamtrs__type_utils__is_match_conditions__hpp
#include "def.hpp"
AMTRS_NAMESPACE_BEGIN

// ==================================================================
//!	@brief	調査したいクラスを、条件関数クラスを用いて条件にマッチ
//!			するか調査します。
// ------------------------------------------------------------------
//!	このテンプレートは
//!
//!		is_match_conditions<TARGET-CLASS, CONDITIONS-CLASS>::value
//!
//!	のように使用し、対象となるクラスが指定された条件にマッチするか
//!	どうかをコンパイル時に静的に調査することができます。
//!	static_assert() の条件にしたり、や std::enable_if<> と組み合わせ
//!	Sfinae の条件にすると便利です。
//!
//!	is_match_conditions<> は、次のような処理を行います。
//!
//!		------------------------------------------------------------
//!		is_match_conditions<TARGET-CLASS, CONDITIONS-CLASS> = 
//!		[](){
//!			RESULT-TYPE = CONDITIONS-CLASS{}(std::in_place_type_t<TARGET-CLASS>{})
//!			for (;;) {
//!				// No match!!
//!				if (RESULT_TYPE is no-avaibe || RESULT-TYPE == std::fale_type) {
//!					return	std::true_false;
//!				}
//!				// Match!!
//!				if (RESULT-TYPE == std::true_type) {
//!					return	std::true_type;
//!				}
//!				// Next conditions.
//!				RESULT-TYPE = CONDITIONS-CLASS{}(RESULT-TYPE);
//!			}
//!		};
//!		------------------------------------------------------------
//!
//!	　つまり、最初に auto operator () (std::in_place_type_t<TARGET-CLASS>) を
//!	呼び出し可能か調べ、呼び出し不可能であれば std::false_type となります。
//!	呼び出し可能であればその戻り値の型を取得し、 std::true_type であれば、
//!	自身を std::true_type とします。
//!	
//!	　そうでない場合は戻り値の型を再び関数オブジェクトに渡し、再度
//!	std::true_type になるかどうか調べ、それ以降は std::true_type になるか
//!	呼び出し不可能になるまで調査します。
// ------------------------------------------------------------------
//!	exsample)	unit_test/type_utils/is_match_conditions.cpp
// ------------------------------------------------------------------
template<class T, class Conditions>
struct	is_match_conditions;			// Prototype.


namespace detail { namespace is_match_conditions_impl {

	template<class T, class Conditions>
	struct call_conditions;


	// Step.1)  T がクラス以外の場合はメンバを持ちようがないので常に false_type となる。
	template<class T, class Conditions, class SFINES = void>
	struct	is_match_conditions_impl
			: call_conditions<std::in_place_type_t<T>, Conditions>::type
	{
	};

	// Step.2-a)  T がクラスの場合は call_conditions<> を用いて再帰的に調査する
	template<class T, class Conditions>
	struct	is_match_conditions_impl<T, Conditions, typename std::enable_if<std::is_class<T>::value>::type>
			: call_conditions<std::in_place_type_t<T>, Conditions>::type
	{
	};

	// Step.2-b)  T がメソッドの場合は call_conditions<> を用いて再帰的に調査する
	template<class T, class Conditions>
	struct	is_match_conditions_impl<T, Conditions, typename std::enable_if<std::is_function<T>::value>::type>
			: call_conditions<T, Conditions>::type
	{
	};

	// Step.3)  T が true_type か false_type になるまで Conditions が呼び出せるかどうか調査します。
	template<class T, class Conditions>
	struct	call_conditions
	{
	private:
		template<class V>
		static auto confirm(V v) -> decltype(Conditions{}(v));
		static auto confirm(...) -> decltype(std::false_type{});
	public:
		using	type = typename call_conditions<decltype(confirm(std::declval<T>())), Conditions>::type;
	};

	// Step.4-Ttue) Step.3 で Conditions が std::true_type を返した場合は再帰呼び出しを終了します。
	template<class Conditions>
	struct	call_conditions<std::true_type, Conditions>
	{
		using	type = std::true_type;
	};

	// Step.4-False) Step.3 で Conditions が std::false_type を返した場合は再帰呼び出しを終了します。
	template<class Conditions>
	struct	call_conditions<std::false_type, Conditions>
	{
		using	type = std::false_type;
	};

} }

template<class T, class Conditions>
struct	is_match_conditions
		: detail::is_match_conditions_impl::is_match_conditions_impl<T, Conditions>
{
};


AMTRS_NAMESPACE_END
#endif

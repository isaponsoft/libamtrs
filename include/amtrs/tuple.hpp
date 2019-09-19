#ifndef	__libamtrs__tuple__hpp
#define	__libamtrs__tuple__hpp
#include <tuple>
#include <utility>
#include "amtrs.hpp"
AMTRS_NAMESPACE_BEGIN

//! T で指定する型が std::tuple<> であるかどうか調べます。
//! std::tuple<> であれば std::true_type そうでなけれb std::false_type と道義になります。
//! example)
//!		std::tuple<int, float>	t;
//! 	static_assert(is_tuple<decltype(t)>::value);		// ok
template<class T>
struct is_tuple;		// prototype.


//! ラムダ式に std::tuple<> が持つ値の参照を渡します。
//! example)
//! 	struct my_data1 { void update(); };
//! 	struct my_data2 { void update(); };
//!		std::tuple<my_data1, my_data2>	t;
//!		for_each (t, [](auto& s) { s.update(); }
template<class TupleT, class FuncT>
void for_each(TupleT&& _tuple, FuncT&& _func, typename std::enable_if<is_tuple<TupleT>::value>::type* = 0);


// ******************************************************************
// 以下内部実装
// ******************************************************************

namespace tuple_impliments {

template<std::size_t I, class TupleT, class FuncT>
int invoke(TupleT&& _tuple, FuncT&& _func)
{
	_func(std::get<I>(std::forward<TupleT>(_tuple)));
	return	0;
}

template<class... Args>
void unpack_dummy(Args&&...){}

template<class TupleT, class FuncT, std::size_t... I>
void for_each(TupleT&& _tuple, FuncT&& _func, std::index_sequence<I...>)
{
	unpack_dummy(invoke<I>(std::forward<TupleT>(_tuple), std::forward<FuncT>(_func))...);
}

template<class...>		struct _is_tuple : std::false_type {};
template<class... T>	struct _is_tuple<std::tuple<T...>> : std::true_type {}; 

} // tuple_impliments

template<class T>
struct is_tuple : tuple_impliments::_is_tuple<typename std::remove_reference<typename std::remove_cv<T>::type>::type>
{};


template<class TupleT, class FuncT>
void for_each(TupleT&& _tuple, FuncT&& _func, typename std::enable_if<is_tuple<TupleT>::value>::type*)
{
	constexpr auto	ts	= std::tuple_size<typename std::remove_reference<TupleT>::type>::value;
	tuple_impliments::for_each(std::forward<TupleT>(_tuple), std::forward<FuncT>(_func), std::make_index_sequence<ts>() );
}


AMTRS_NAMESPACE_END
#endif

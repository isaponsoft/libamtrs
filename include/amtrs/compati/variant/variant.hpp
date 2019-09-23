/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__variant__variant__hpp
#define	__libamtrs__variant__variant__hpp
#include "types.hpp"
#include "unsafed_variant.hpp"
#include "variant-stl.hpp"
AMTRS_NAMESPACE_BEGIN

template<class... Types>
class	variant;					// Prototype.


template<class... Types>
struct	variant_size;				// Prototype.


constexpr std::size_t	variant_npos	= static_cast<std::size_t>(-1);

#include "variant-alternative.hpp-inc"
#include "variant-bad_variant_access.hpp-inc"
#include "variant-holds_alternative.hpp-inc"

template<class... Types>
class	variant
{
public:
	using	union_t	= unsafed_variant<Types...>;

	constexpr variant() : m_typeIndex(variant_npos) {}
	variant(const variant& );
	variant(      variant&&) noexcept;
	
	template<class Arg, class = typename std::enable_if<!std::is_same<std::tuple<variant>, std::tuple<typename std::remove_reference<Arg>::type>>::value>::type>
	constexpr explicit variant(Arg&& _args);
	
	template<class OrderT, class... Args>
	constexpr explicit variant(std::in_place_type_t<OrderT>, Args&&... _args);
	
	template<class OrderT, class U, class... Args>
	constexpr explicit variant(std::in_place_type_t<OrderT>, std::initializer_list<U> _il, Args&&... _args);

	template <std::size_t I, class... Args>
	constexpr explicit variant(std::in_place_index_t<I>, Args&&... _args);

	template <std::size_t I, class U, class... Args>
	constexpr explicit variant(std::in_place_index_t<I>, std::initializer_list<U> _il, Args&&... _args);

#if 0

	// allocator-extended constructors
	template <class Alloc>
	variant(allocator_arg_t, const Alloc&);

	template <class Alloc>
	variant(allocator_arg_t, const Alloc&, const variant&);

	template <class Alloc>
	variant(allocator_arg_t, const Alloc&, variant&&);

	template <class Alloc, class T>
	variant(allocator_arg_t, const Alloc&, T&&);

	template <class Alloc, class T, class... Args>
	variant(allocator_arg_t, const Alloc&, in_place_type_t<T>, Args&&...);

	template <class Alloc, class T, class U, class... Args>
	variant(allocator_arg_t, const Alloc&, in_place_type_t<T>, initializer_list<U>, Args&&...);

	template <class Alloc, size_t I, class... Args>
	variant(allocator_arg_t, const Alloc&, in_place_index_t<I>, Args&&...);

	template <class Alloc, size_t I, class U, class... Args>
	variant(allocator_arg_t, const Alloc&, in_place_index_t<I>, initializer_list<U>, Args&&...);
#endif

	~variant();

	variant& operator = (const variant&  _r);
	variant& operator = (      variant&& _r) noexcept;

	template<class Arg, class = typename std::enable_if<!std::is_same<std::tuple<variant>, std::tuple<typename std::remove_reference<Arg>::type>>::value>::type>
	variant& operator = (Arg&& _r);

	// Observers

	constexpr std::size_t index() const noexcept { return m_typeIndex; }

	constexpr bool valueless_by_exception() const noexcept;


	// Modifiers

	// ====================================================================
	//!	@brief	OrderT で指定する型に対して、コンストラクタを呼び出します。
	// --------------------------------------------------------------------
	template <class OrderT, class... Args>
	OrderT& emplace(Args&&... _args);

	// ====================================================================
	//!	@brief	OrderT で指定する型に対して、コンストラクタを呼び出します。
	// --------------------------------------------------------------------
	template <class OrderT, class U, class... Args>
	OrderT& emplace(std::initializer_list<U> _il, Args&&... _args);

	// ====================================================================
	//!	@brief	I で指定するインデックスの型に対してコンストラクタを呼び出します。
	// --------------------------------------------------------------------
	//!	decltype(variant<int, short, double>{}.emplace<1>(1)) == short
	// --------------------------------------------------------------------
	template <size_t I, class... Args>
	auto emplace(Args&&... _args) -> variant_alternative_t<I, variant>&
	{ return emplace<variant_alternative_t<I, variant>>(std::forward<Args>(_args)...); }

	// ====================================================================
	//!	@brief	I で指定するインデックスの型に対してコンストラクタを呼び出します。
	// --------------------------------------------------------------------
	//!	decltype(variant<int, short, double>{}.emplace<1>(1)) == short
	// --------------------------------------------------------------------
	template <size_t I, class U, class... Args>
	auto emplace(std::initializer_list<U> _il, Args&&... _args) -> variant_alternative_t<I, variant>&
	{ return emplace<variant_alternative_t<I, variant>>(_il, std::forward<Args>(_args)...); }


	void swap(variant& _other);


	constexpr bool operator == (const variant& _r);
	constexpr bool operator != (const variant& _r);
	constexpr bool operator <  (const variant& _r);
	constexpr bool operator <= (const variant& _r);
	constexpr bool operator >  (const variant& _r);
	constexpr bool operator >= (const variant& _r);


//private:
	void reset();

	union_t			m_value;
	std::size_t		m_typeIndex;

	template<class Arg>
	struct	constract_operation
	{
		template<class T>
		auto operator () (std::in_place_type_t<T>) -> decltype(new T (std::declval<const Arg&>()), std::true_type{});
	};
};


template<class... Types>
struct	variant_size<variant<Types...>>
		: std::integral_constant<std::size_t, sizeof...(Types)>
{};


template<class... Types>
variant<Types...>::variant(const variant& _r)
	: variant()
{
	if (_r.m_typeIndex != variant_npos)
	{
		m_value.constract(_r.m_typeIndex, _r.m_value);
		m_typeIndex	= _r.m_typeIndex;
	}
}


template<class... Types>
variant<Types...>::variant(variant&& _r) noexcept
{
	if (_r.m_typeIndex != variant_npos)
	{
		m_value.constract(_r.m_typeIndex, std::move(_r.m_value));
		m_typeIndex		= _r.m_typeIndex;
		_r.m_typeIndex	= variant_npos;
	}
}


template<class... Types>
template<class Arg, class>
constexpr variant<Types...>::variant(Arg&& _r)
	: variant()
{
	using					arg_type	= typename std::remove_cv<typename std::remove_reference<decltype(_r)>::type>::type;
//	static_assert(union_t::template enable_operation_type<constract_operation<arg_type>>::count <= 1, "\"operator = ()\" is many matches.");
//	static_assert(union_t::template enable_operation_type<constract_operation<arg_type>>::count >= 1, "Can't find \"operator = ()\" it is many.");
//	using					match_type	= typename union_t::template enable_operation_type<constract_operation<arg_type>>::type;
	constexpr std::size_t	src_index	= union_t::index(std::in_place_type_t<arg_type>{});
	m_value.constract(std::in_place_type_t<arg_type>(), std::forward<Arg>(_r));
	m_typeIndex	= src_index;
}


template<class... Types>
template<class OrderT, class... Args>
constexpr variant<Types...>::variant(std::in_place_type_t<OrderT>, Args&&... _args)
	: variant()
{
	m_value.template constract<OrderT>(std::forward<Args>(_args)...);
	m_typeIndex	= m_value.index(std::in_place_type_t<OrderT>{});
}


template<class... Types>
template<class OrderT, class U, class... Args>
constexpr variant<Types...>::variant(std::in_place_type_t<OrderT>, std::initializer_list<U> _il, Args&&... _args)
	: variant()
{
	m_value.template constract<OrderT>(_il, std::forward<Args>(_args)...);
	m_typeIndex	= m_value.index(std::in_place_type_t<OrderT>{});
}


template<class... Types>
template <size_t I, class... Args>
constexpr variant<Types...>::variant(std::in_place_index_t<I>, Args&&... _args)
	: variant(std::in_place_type_t<type_list_type<I, Types...>>{}, std::forward<Args>(_args)...)
{
}


template<class... Types>
template <size_t I, class U, class... Args>
constexpr variant<Types...>::variant(std::in_place_index_t<I>, std::initializer_list<U> _il, Args&&... _args)
	: variant(std::in_place_type_t<type_list_type<I, Types...>>{}, _il, std::forward<Args>(_args)...)
{
}


template<class... Types>
variant<Types...>::~variant()
{
	reset();
}


template<class... Types>
auto variant<Types...>::operator = (const variant& _r) -> variant&
{
	if (_r.m_typeIndex != m_typeIndex)
	{
		reset();
		if (_r.m_typeIndex != variant_npos)
		{
			m_value.constract(std::in_place_type_t<void>(), _r.m_typeIndex);
		}
		m_typeIndex	= _r.m_typeIndex;
	}
	else
	{
		if (_r.m_typeIndex != variant_npos)
		{
			m_value.assign(m_typeIndex, _r.m_value);
		}
	}
	return	*this;
}


template<class... Types>
auto variant<Types...>::operator = (variant&& _r) noexcept -> variant&
{
	if (_r.m_typeIndex != m_typeIndex)
	{
		reset();
		if (_r.m_typeIndex != variant_npos)
		{
			m_value.constract(_r.m_typeIndex, std::move(_r.m_value));
		}
		m_typeIndex	= _r.m_typeIndex;
		_r.m_typeIndex	= variant_npos;
	}
	else
	{
		if (_r.m_typeIndex != variant_npos)
		{
			m_value.assign(m_typeIndex, std::move(_r.m_value));
		}
		_r.m_typeIndex	= variant_npos;
	}
	return	*this;
}


template<class... Types>
template<class Arg, class>
auto variant<Types...>::operator = (Arg&& _r) -> variant&
{
	*this = variant(std::forward<Arg>(_r));
	return	*this;
}

template<class... Types>
void variant<Types...>::reset()
{
	if (m_typeIndex != variant_npos)
	{
		m_value.destract(m_typeIndex);
		m_typeIndex	= variant_npos;
	}
}


template<class... Types>
template <class OrderT, class... Args>
OrderT& variant<Types...>::emplace(Args&&... _args)
{
	constexpr std::size_t	src_index	= union_t::index(std::in_place_type_t<OrderT>{});
	if (src_index != m_typeIndex)
	{
		reset();
	}
	OrderT&					retval		= m_value.constract(std::in_place_type_t<OrderT>(), std::forward<Args>(_args)...);
	m_typeIndex	= src_index;
	return	retval;
}

template<class... Types>
template <class OrderT, class U, class... Args>
OrderT& variant<Types...>::emplace(std::initializer_list<U> _il, Args&&... _args)
{
	constexpr std::size_t	src_index	= union_t::index(std::in_place_type_t<OrderT>{});
	if (src_index != m_typeIndex)
	{
		reset();
	}
	OrderT&					retval		= m_value.template constract<OrderT>(_il, std::forward<Args>(_args)...);
	m_typeIndex	= src_index;
}


template<class... Types>
void variant<Types...>::swap(variant& _other)
{
	if (index() == _other.index())
	{
		m_value.swap(index(), _other.m_value);
		std::swap(m_typeIndex, _other.m_typeIndex);
	}
	else
	{
		variant	tmp(std::move(*this));
		*this	= std::move(_other);
		_other	= std::move(tmp);
	}
}


template<class... Types>
constexpr bool variant<Types...>::operator == (const variant& _r)
{
	if (index() != _r.index())
	{
		return	false;
	}
#if 0
	if (valueless_by_exception())
	{
		return	true;
	}
#endif
	return	m_value == std::pair<std::size_t, const union_t&>{index(), _r.m_value};
}


template<class... Types>
constexpr bool variant<Types...>::operator != (const variant& _r)
{
	if (index() != _r.index())
	{
		return	false;
	}
#if 0
	if (valueless_by_exception())
	{
		return	true;
	}
#endif
	return	m_value != std::pair<std::size_t, const union_t&>{index(), _r.m_value};
}


template<class... Types>
constexpr bool variant<Types...>::operator <  (const variant& _r)
{
	if (index() != _r.index())
	{
		return	false;
	}
#if 0
	if (valueless_by_exception())
	{
		return	true;
	}
#endif
	return	m_value < std::pair<std::size_t, const union_t&>{index(), _r.m_value};
}


template<class... Types>
constexpr bool variant<Types...>::operator <= (const variant& _r)
{
	if (index() != _r.index())
	{
		return	false;
	}
#if 0
	if (valueless_by_exception())
	{
		return	true;
	}
#endif
	return	m_value <= std::pair<std::size_t, const union_t&>{index(), _r.m_value};
}


template<class... Types>
constexpr bool variant<Types...>::operator >  (const variant& _r)
{
	if (index() != _r.index())
	{
		return	false;
	}
#if 0
	if (valueless_by_exception())
	{
		return	true;
	}
#endif
	return	m_value > std::pair<std::size_t, const union_t&>{index(), _r.m_value};
}


template<class... Types>
constexpr bool variant<Types...>::operator >= (const variant& _r)
{
	if (index() != _r.index())
	{
		return	false;
	}
#if 0
	if (valueless_by_exception())
	{
		return	true;
	}
#endif
	return	m_value >= std::pair<std::size_t, const union_t&>{index(), _r.m_value};
}


AMTRS_NAMESPACE_END
namespace std {

template<class OrderT, class... Types>
    OrderT& get(::amtrs::variant<Types...>& _v)
{
    if (_v.m_typeIndex != ::amtrs::variant<Types...>::union_t::index(std::in_place_type_t<OrderT>{}))
	{
		AMTRS_DEBUG_LOG("get<%s>(variant) %zd %zd", typeid(OrderT).name(), _v.m_typeIndex, ::amtrs::variant<Types...>::union_t::index(std::in_place_type_t<OrderT>{}));
		throw ::amtrs::bad_variant_access();
	}
	return	_v.m_value.template get<OrderT>();
}

template<class OrderT, class... Types>
const OrderT& get(const ::amtrs::variant<Types...>& _v)
{
	if (_v.m_typeIndex != ::amtrs::variant<Types...>::union_t::index(std::in_place_type_t<OrderT>{}))
	{
		AMTRS_DEBUG_LOG("get<%s>(variant) %zd %zd", typeid(OrderT).name(), _v.m_typeIndex, ::amtrs::variant<Types...>::union_t::index(std::in_place_type_t<OrderT>{}));
		throw ::amtrs::bad_variant_access();
	}
	return	_v.m_value.template get<OrderT>();
}


template<class OrderT, class... Types>
    OrderT* get_if(::amtrs::variant<Types...>* _v)
{
    if (_v->m_typeIndex != ::amtrs::variant<Types...>::union_t::index(std::in_place_type_t<OrderT>{}))
	{
		return	nullptr;
	}
	return	&_v->m_value.template get<OrderT>();
}

template<class OrderT, class... Types>
const OrderT* get_if(const ::amtrs::variant<Types...>* _v)
{
	if (_v->m_typeIndex != ::amtrs::variant<Types...>::union_t::index(std::in_place_type_t<OrderT>{}))
	{
		return	nullptr;
	}
	return	&_v->m_value.template get<OrderT>();
}


template<class... Types>
void swap(::amtrs::variant<Types...>& _v1, ::amtrs::variant<Types...>& _v2)
{
	_v1.swap(_v2);
}


} // namespace std
#endif

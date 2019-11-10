/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__compati__variant__unsafed_variant__hpp
#define	__libamtrs__compati__variant__unsafed_variant__hpp
#include "../../amtrs/type_traits/@"
#include "types.hpp"
AMTRS_NAMESPACE_BEGIN
template<class... Types>
struct	unsafed_variant;
AMTRS_NAMESPACE_END
namespace std
{
	template<class OrderT, class... Types> OrderT& get(::amtrs::unsafed_variant<Types...>& _v);
	template<class OrderT, class... Types> const OrderT& get(const ::amtrs::unsafed_variant<Types...>& _v);
}

AMTRS_NAMESPACE_BEGIN


// ==================================================================
//!	@brief	型セーフではない variant、またはジェネリックな union
//!			このクラスは安全ではありません。
//!			Types にクラスを指定した場合、コンストラクタやデストラクタ
//!			の呼び出しを行いません。利用者側が判定する必要があります。
// ------------------------------------------------------------------
//!	unsafed_variant<int, short>	v;
//!	v.get<int>() = v;
// ------------------------------------------------------------------
template<class... Types>
struct	unsafed_variant;


AMTRS_IMPLEMENTS_BEGIN(unsafed_variant)
template<class... Types>
union	union_val;				// Prototype.


template<class Type, class... Types>
union	union_val<Type, Types...>
{
	union_val(){}
	~union_val(){}
	template<class OrderT>       OrderT& get(typename std::enable_if< std::is_same<OrderT, Type>::value>::type* = 0)       noexcept { return m_value;                        }
	template<class OrderT>       OrderT& get(typename std::enable_if<!std::is_same<OrderT, Type>::value>::type* = 0)       noexcept { return m_other.template get<OrderT>(); }
	template<class OrderT> const OrderT& get(typename std::enable_if< std::is_same<OrderT, Type>::value>::type* = 0) const noexcept { return m_value;                        }
	template<class OrderT> const OrderT& get(typename std::enable_if<!std::is_same<OrderT, Type>::value>::type* = 0) const noexcept { return m_other.template get<OrderT>(); }
	template<class F> void execute(std::size_t _index, F _callback)
	{
		if (_index == 0)
		{
			_callback(m_value);
		}
		else
		{
			m_other.execute(_index - 1, std::forward<F>(_callback));
		}
	}

	template<class F> void execute(std::size_t _index, F _callback) const
	{
		if (_index == 0)
		{
			_callback(m_value);
		}
		else
		{
			m_other.execute(_index - 1, std::forward<F>(_callback));
		}
	}

	template<class R, class F> R compare(std::size_t _index, F _callback) const
	{
		if (_index == 0)
		{
			return	_callback(m_value);
		}
		else
		{
			return	m_other.template compare<R>(_index - 1, std::forward<F>(_callback));
		}
	}

	Type					m_value;
	union_val<Types...>		m_other;
};

template<class Type>
union	union_val<Type>
{
	union_val(){}
	~union_val(){}
	template<class OrderT>       Type& get()       noexcept { return m_value; }
	template<class OrderT> const Type& get() const noexcept { return m_value; }
	template<class F> void execute(std::size_t _index, F _callback)
	{
		if (_index == 0)
		{
			_callback(m_value);
		}
	}

	template<class F> void execute(std::size_t _index, F _callback) const
	{
		if (_index == 0)
		{
			_callback(m_value);
		}
	}

	template<class R, class F> R compare(std::size_t _index, F _callback) const
	{
		if (_index == 0)
		{
			return	_callback(m_value);
		}
		return	{};
	}

	Type					m_value;
};





template<class Operation>
struct	conditions__can_operation
{
	template<class T> auto operator () (std::in_place_type_t<T>)
		-> decltype(std::declval<Operation>()(), std::true_type{});
};

template<class...>
struct	enable_operation_type;

template<class Ope, class Type, class... Types>
struct	enable_operation_type<Ope, Type, Types...>
{
	using	type	= typename std::conditional<
						is_match_conditions<Type, Ope>::value,
						Type,
						typename enable_operation_type<Ope, Types...>::type
					>::type;
	static constexpr std::size_t	count	= (is_match_conditions<Type, Ope>::value ? 1 : 0)
											+ enable_operation_type<Ope, Types...>::count;
};

template<class Ope, class Type>
struct	enable_operation_type<Ope, Type>
{
	using	type	= typename std::conditional<
						is_match_conditions<Type, Ope>::value,
						Type,
						void
					>::type;
	static constexpr std::size_t	count = is_match_conditions<Type, Ope>::value ? 1 : 0;

};


AMTRS_IMPLEMENTS_END(unsafed_variant)


// ************************************************************************
// 実装本体
// ************************************************************************
template<class... Types>
struct	unsafed_variant
{
private:
	using	impl_type		= AMTRS_IMPLEMENTS(unsafed_variant)::union_val<Types...>;
public:
	template<std::size_t N>
	using	element_type	= typename std::tuple_element<N, std::tuple<Types...>>::type;

	template<class T>
	using	index_type		= typename type_list<Types...>::template index_type<T>;

	template<class OrderT>
	static constexpr std::size_t index() noexcept { return index_type<OrderT>::value; }

	template<class OrderT>
	static constexpr std::size_t index(std::in_place_type_t<OrderT>) noexcept { return index_type<OrderT>::value; }

	//!	@brief	指定した型にアクセスします。
	template<class OrderT>
	OrderT& get() noexcept;

	//!	@brief	指定した型にアクセスします。
	template<class OrderT>
	const OrderT& get() const noexcept;

	//!	@brief	指定した型に代入します。
	void assign(std::size_t _index, const unsafed_variant& _src);

	//!	@brief	指定した型にムーブします。
	void assign(std::size_t _index, unsafed_variant&& _src);

	//! @brief	指定した型のコピーコンストラクタを呼び出します。
	template<class OrderT>
	OrderT& constract(const unsafed_variant& _src);

	//! @brief	指定した型のコンストラクタを呼び出します。
	template<class OrderT, class... Args>
	OrderT& constract(std::in_place_type_t<OrderT>, Args&&... _args);

	//! @brief	指定したIndexのコピーコンストラクタを呼び出します。
	//!	インデックスが無効な場合は何もしません。
	void constract(std::in_place_type_t<void>, std::size_t _index);

	//! @brief	指定した型のコピーコンストラクタを呼び出します。
	template<class OrderT>
	OrderT& constract(const OrderT& _src);

	//! @brief	指定したIndexのコピーコンストラクタを呼び出します。
	//!	インデックスが無効な場合は何もしません。
	void constract(std::size_t _index, const unsafed_variant& _src);

	//! @brief	指定した型のムーブコンストラクタを呼び出します。
	template<class OrderT>
	OrderT& constract(OrderT&& _src);

	//! @brief	指定した型のムーブコンストラクタを呼び出します。
	template<class OrderT>
	OrderT& constract(unsafed_variant&& _src);

	//! @brief	指定したIndexのムーブコンストラクタを呼び出します。
	//!	インデックスが無効な場合は何もしません。
	void constract(std::size_t _index, unsafed_variant&& _src);

	//! @brief	指定した型のデストラクタを呼び出します。
	template<class OrderT>
	void destract();

	//! @brief	指定したIndexのデストラクタを呼び出します。
	//!	インデックスが無効な場合は何もしません。
	void destract(std::size_t _index);

	//! @brief	指定したIndexに対してコールバックを呼び出します。
	//!	インデックスが無効な場合は何もしません。
	template<class F> void execute(std::size_t _index, F _callback) { m_value.execute(_index, std::forward<F>(_callback)); }

	//! @brief	指定したIndexに対してコールバックを呼び出します。
	//!	インデックスが無効な場合は何もしません。
	template<class F> void execute(std::size_t _index, F _callback) const { m_value.execute(_index, std::forward<F>(_callback)); }


	//!	@brief	Opeで指定する操作が可能な最初の型を返します。
	template<class Ope>
	struct	enable_operation_type
			: AMTRS_IMPLEMENTS(unsafed_variant)::enable_operation_type<Ope, Types...>
	{};

	void swap(std::size_t _index, unsafed_variant& _other);

	bool operator == (std::pair<std::size_t, const unsafed_variant&> _r) const;
	bool operator != (std::pair<std::size_t, const unsafed_variant&> _r) const;
	bool operator <  (std::pair<std::size_t, const unsafed_variant&> _r) const;
	bool operator <= (std::pair<std::size_t, const unsafed_variant&> _r) const;
	bool operator >  (std::pair<std::size_t, const unsafed_variant&> _r) const;
	bool operator >= (std::pair<std::size_t, const unsafed_variant&> _r) const;


private:
	impl_type		m_value;

	struct	exec_constract
	{
		template<class T>
		void operator () (T& _value)
		{
			new (&_value) T ();
		}
	};

	struct	exec_copy
	{
		const unsafed_variant&	m_right;

		exec_copy(const unsafed_variant& _r) : m_right(_r) {}

		template<class T>
		void operator () (T& _value)
		{
			new (&_value) T (m_right.get<T>());
		}
	};

	struct	exec_move
	{
		unsafed_variant&	m_right;

		exec_move(unsafed_variant& _r) : m_right(_r) {}

		template<class T>
		void operator () (T& _value)
		{
			new (&_value) T (std::move(m_right.get<T>()));
		}
	};

	struct	exec_destract
	{
		template<class T>
		void operator () (T& _value)
		{
			_value.~T();
		}
	};

	struct	exec_assign
	{
		const unsafed_variant&	m_right;

		exec_assign(const unsafed_variant& _r) : m_right(_r) {}

		template<class T>
		void operator () (T& _value)
		{
			_value = m_right.get<T>();
		}
	};

	struct	exec_assign_move
	{
		unsafed_variant&	m_right;

		exec_assign_move(unsafed_variant& _r) : m_right(_r) {}

		template<class T>
		void operator () (T& _value)
		{
			_value = std::move(m_right.get<T>());
		}
	};

	struct	exec_swap
	{
		unsafed_variant&	m_right;

		exec_swap(unsafed_variant& _r) : m_right(_r) {}

		template<class T>
		void operator () (T& _value)
		{
			std::swap(_value, m_right.get<T>());
		}
	};

	struct	exec_eq
	{
		const unsafed_variant&	m_right;
		exec_eq(const unsafed_variant& _r) : m_right(_r) {}
		template<class T>
		bool operator () (T& _value)
		{
			return	_value == m_right.get<typename std::remove_cv<T>::type>();
		}
	};

	struct	exec_not_eq
	{
		const unsafed_variant&	m_right;
		exec_not_eq(const unsafed_variant& _r) : m_right(_r) {}
		template<class T>
		bool operator () (T& _value)
		{
			return	_value != m_right.get<typename std::remove_cv<T>::type>();
		}
	};

	struct	exec_latter
	{
		const unsafed_variant&	m_right;
		exec_latter(const unsafed_variant& _r) : m_right(_r) {}
		template<class T>
		bool operator () (T& _value)
		{
			return	_value < m_right.get<typename std::remove_cv<T>::type>();
		}
	};

	struct	exec_latter_eq
	{
		const unsafed_variant&	m_right;
		exec_latter_eq(const unsafed_variant& _r) : m_right(_r) {}
		template<class T>
		bool operator () (T& _value)
		{
			return	_value <= m_right.get<typename std::remove_cv<T>::type>();
		}
	};

	struct	exec_grater
	{
		const unsafed_variant&	m_right;
		exec_grater(const unsafed_variant& _r) : m_right(_r) {}
		template<class T>
		bool operator () (T& _value)
		{
			return	_value > m_right.get<typename std::remove_cv<T>::type>();
		}
	};

	struct	exec_grater_eq
	{
		const unsafed_variant&	m_right;
		exec_grater_eq(const unsafed_variant& _r) : m_right(_r) {}
		template<class T>
		bool operator () (T& _value)
		{
			return	_value >= m_right.get<typename std::remove_cv<T>::type>();
		}
	};

	template<class _OrderT, class... _Types> friend	      _OrderT& std::get(      unsafed_variant<_Types...>&);
	template<class _OrderT, class... _Types> friend	const _OrderT& std::get(const unsafed_variant<_Types...>&);
};



template<class... Types>
template<class OrderT>
OrderT& unsafed_variant<Types...>::get() noexcept
{
    static_assert(::amtrs::has_type_list<OrderT, Types...>::value, "unsafed_variant has not order types.");
	return	m_value.template get<OrderT>();
}


template<class... Types>
template<class OrderT>
const OrderT& unsafed_variant<Types...>::get() const noexcept
{
	static_assert(::amtrs::has_type_list<OrderT, Types...>::value, "unsafed_variant has not order types.");
	return	m_value.template get<OrderT>();
}


template<class... Types>
void unsafed_variant<Types...>::assign(std::size_t _index, const unsafed_variant& _src)
{
	m_value.execute(_index, exec_assign{_src});
}


template<class... Types>
void unsafed_variant<Types...>::assign(std::size_t _index, unsafed_variant&& _src)
{
	m_value.execute(_index, exec_assign_move{_src});
}


template<class... Types>
void unsafed_variant<Types...>::constract(std::in_place_type_t<void>, std::size_t _index)
{
	m_value.execute(_index, exec_constract{});
}


template<class... Types>
template<class OrderT>
OrderT& unsafed_variant<Types...>::constract(const unsafed_variant& _src)
{
	static_assert(::amtrs::has_type_list<OrderT, Types...>::value, "unsafed_variant has not order types.");
	OrderT&	o = get<OrderT>();
	new (&o) OrderT(_src.get<OrderT>());
	return	o;
}


template<class... Types>
template<class OrderT, class... Args>
OrderT& unsafed_variant<Types...>::constract(std::in_place_type_t<OrderT>, Args&&... _args)
{
	static_assert(::amtrs::has_type_list<OrderT, Types...>::value, "unsafed_variant has not order types.");
	OrderT&	o = get<OrderT>();
	new (&o) OrderT(std::forward<Args>(_args)...);
	return	o;
}



template<class... Types>
template<class OrderT>
OrderT& unsafed_variant<Types...>::constract(const OrderT& _src)
{
	static_assert(::amtrs::has_type_list<OrderT, Types...>::value, "unsafed_variant has not order types.");
	OrderT&	o = get<OrderT>();
	new (&o) OrderT(_src);
	return	o;
}



template<class... Types>
void unsafed_variant<Types...>::constract(std::size_t _index, const unsafed_variant& _src)
{
	m_value.execute(_index, exec_copy{_src});
}


template<class... Types>
template<class OrderT>
OrderT& unsafed_variant<Types...>::constract(OrderT&& _src)
{
	static_assert(::amtrs::has_type_list<OrderT, Types...>::value, "unsafed_variant has not order types.");
	OrderT&	o = get<OrderT>();
	new (&o) OrderT(std::move(_src));
	return	o;
}


template<class... Types>
template<class OrderT>
OrderT& unsafed_variant<Types...>::constract(unsafed_variant&& _src)
{
	static_assert(::amtrs::has_type_list<OrderT, Types...>::value, "unsafed_variant has not order types.");
	OrderT&	o = get<OrderT>();
	new (&o) OrderT(std::move(_src.get<OrderT>()));
	return	o;
}


template<class... Types>
void unsafed_variant<Types...>::constract(std::size_t _index, unsafed_variant&& _src)
{
	m_value.execute(_index, exec_move{_src});
}


template<class... Types>
template<class OrderT>
void unsafed_variant<Types...>::destract()
{
	static_assert(::amtrs::has_type_list<OrderT, Types...>::value, "unsafed_variant has not order types.");
	OrderT&	o = get<OrderT>();
	o.~OrderT();
}


template<class... Types>
void unsafed_variant<Types...>::destract(std::size_t _index)
{
	m_value.execute(_index, exec_destract{});
}


template<class... Types>
void unsafed_variant<Types...>::swap(std::size_t _index, unsafed_variant& _other)
{
	m_value.execute(_index, exec_swap{_other});
}


template<class... Types>
bool unsafed_variant<Types...>::operator == (std::pair<std::size_t, const unsafed_variant<Types...>&> _r) const
{
	return	m_value.template compare<bool>(_r.first, exec_eq{_r.second});
}


template<class... Types>
bool unsafed_variant<Types...>::operator != (std::pair<std::size_t, const unsafed_variant<Types...>&> _r) const
{
	return	m_value.template compare<bool>(_r.first, exec_not_eq{_r.second});
}


template<class... Types>
bool unsafed_variant<Types...>::operator <  (std::pair<std::size_t, const unsafed_variant<Types...>&> _r) const
{
	return	m_value.template compare<bool>(_r.first, exec_latter{_r.second});
}


template<class... Types>
bool unsafed_variant<Types...>::operator <= (std::pair<std::size_t, const unsafed_variant<Types...>&> _r) const
{
	return	m_value.template compare<bool>(_r.first, exec_latter_eq{_r.second});
}


template<class... Types>
bool unsafed_variant<Types...>::operator >  (std::pair<std::size_t, const unsafed_variant<Types...>&> _r) const
{
	return	m_value.template compare<bool>(_r.first, exec_grater{_r.second});
}


template<class... Types>
bool unsafed_variant<Types...>::operator >= (std::pair<std::size_t, const unsafed_variant<Types...>&> _r) const
{
	return m_value.template compare<bool>(_r.first, exec_grater_eq{_r.second});
}


AMTRS_NAMESPACE_END
namespace std
{

	template<class OrderT, class... Types> OrderT& get(::amtrs::unsafed_variant<Types...>& _v)
	{
		static_assert(::amtrs::has_type_list<OrderT, Types...>::value, "unsafed_variant has not order types.");
		return	_v.template get<OrderT>();
	}

	template<class OrderT, class... Types> const OrderT& get(const ::amtrs::unsafed_variant<Types...>& _v)
	{
		static_assert(::amtrs::has_type_list<OrderT, Types...>::value, "unsafed_variant has not order types.");
		return	_v.template get<OrderT>();
	}
}
#endif

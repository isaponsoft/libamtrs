#ifndef	__passion__vm__vmvariable_trits__hpp
#define	__passion__vm__vmvariable_trits__hpp
#include <amtrs/type_traits.hpp>
#include "vmfunction.hpp"
PASSION_NAMESPACE_BEGIN


template<class...>
struct	variable_traits;
#if	0
{
	using	vmtype						VM内で使用する型
	static constexpr auto signature() 	型名を示すシグネチャ
}
#endif


template<>
struct	variable_traits<void>
{
	using	vmtype	= void;
	static constexpr auto signature() noexcept { return make_constring("void"); }
};


template<>
struct	variable_traits<vmint_t>
{
	using	vmtype	= vmint_t;
	static constexpr auto signature() noexcept { return make_constring("int"); }
};


template<>
struct	variable_traits<char>
{
	using	vmtype	= char;
	static constexpr auto signature() noexcept { return make_constring("char"); }
};


template<>
struct	variable_traits<int8_t>
{
	using	vmtype	= int8_t;
	static constexpr auto signature() noexcept { return make_constring("int8_t"); }
};

template<>
struct	variable_traits<int16_t>
{
	using	vmtype	= int16_t;
	static constexpr auto signature() noexcept { return make_constring("int16_t"); }
};


template<>
struct	variable_traits<int32_t>
{
	using	vmtype	= int32_t;
	static constexpr auto signature() noexcept { return make_constring("int32_t"); }
};


template<>
struct	variable_traits<int64_t>
{
	using	vmtype	= int64_t;
	static constexpr auto signature() noexcept { return make_constring("int64_t"); }
};


template<>
struct	variable_traits<vmstrobj*>
{
	using	vmtype	= ref<vmobject>;
	static constexpr auto signature() { return make_constring("psn::string^"); }
};


template<>
struct	variable_traits<char const*>
{
	using	vmtype	= ref<vmobject>;
	static constexpr auto signature() noexcept { return make_constring("char const*"); }
};


template<>
struct	variable_traits<vmstring>
{
	using	vmtype	= ref<vmobject>;
	static constexpr auto signature() noexcept { return make_constring("psn::string^"); }
};


template<>
struct	variable_traits<std::string>
{
	using	vmtype	= ref<vmobject>;
	static constexpr auto signature() noexcept { return make_constring("psn::string^"); }
};



template<>
struct	variable_traits<std::string_view>
{
	using	vmtype	= ref<vmobject>;
	static constexpr auto signature() noexcept { return make_constring("psn::string^"); }
};


//! ネイティブクラス型
template<class T>
struct	variable_traits<T*>
{
	using	vmtype	= ref<vmobject>;
	static constexpr auto signature() { return T::signature(); }
};


//! ネイティブクラス型
template<class T>
struct	variable_traits<ref<T>>
{
	using	vmtype	= ref<vmobject>;
	static constexpr auto signature() { return T::signature() + make_constring("^"); }
};



//! 引数リスト
template<class F, class... Args>
struct	variable_traits<F, Args...>
{
	static constexpr auto signature() noexcept
	{
		return	variable_traits<F>::signature() + make_constring(",") + variable_traits<Args...>::signature();
	}
};

//! 関数
template<class R, class... Args>
struct	variable_traits<R(Args...)>
{
	using	vmtype	= ref<vmfunction>;
	static constexpr auto signature() noexcept
	{
		return	make_constring("(") + variable_traits<Args...>::signature() + make_constring(")") + variable_traits<R>::signature();
	}
};

//! 関数（引数なし）
template<class R>
struct	variable_traits<R()>
{
	using	vmtype	= ref<vmfunction>;
	static constexpr auto signature() noexcept
	{
		return	make_constring("()") + variable_traits<R>::signature();
	}
};








PASSION_NAMESPACE_END
#endif

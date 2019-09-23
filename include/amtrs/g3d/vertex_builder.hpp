/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__g3d__vertex_builder__hpp
#define	__libamtrs__g3d__vertex_builder__hpp
#include "device.hpp"
AMTRS_G3D_NAMESPACE_BEGIN


template<g3d::device::primitive_type Prim, class T, std::size_t S = 0>
struct	vertex_builder;

template<class T, std::size_t S>
struct	vertex_builder<g3d::device::primitive_type::triangle_strip, T, S>
{
	static constexpr std::size_t					count 		= S;
	static constexpr g3d::device::primitive_type	prim_type	= g3d::device::primitive_type::triangle_strip;

	template<std::size_t OS>
	using	other_t					= vertex_builder<prim_type, T, OS>;

	using	value_type				= float;


	const T* data() const noexcept { return items; }
	constexpr std::size_t size() const noexcept { return count; }


	template<std::size_t RS>
	constexpr auto operator + (const other_t<RS>& _rhs) const noexcept -> other_t<S + 2 + RS>
	{
		return	other_t<S + 2 + RS>(*this, _rhs, std::make_index_sequence<S>(), std::make_index_sequence<RS>());
	}

private:
	template<class... Args>
	constexpr vertex_builder(Args&&... _args)
		: items{std::forward<Args>(_args)...}
	{}

	template<std::size_t... I>
	constexpr vertex_builder(const T* _s, std::index_sequence<I...>) noexcept
		: items{getval<I>(_s)...}
	{}

	template<class L, class R, std::size_t... I1, std::size_t... I2>
	constexpr vertex_builder(L _a, R _b, std::index_sequence<I1...>, std::index_sequence<I2...>) noexcept
		: items{getval<I1>(_a)..., getval<sizeof...(I1)-1>(_a), getval<0>(_b), getval<I2>(_b)...}
	{}

	template<std::size_t I, std::size_t VS>
	static constexpr T getval(const other_t<VS>& _v) noexcept { return _v.items[I]; }


	T		items[S];

	template<g3d::device::primitive_type, class, std::size_t>
	friend	struct	vertex_builder;
};


template<class T>
struct	vertex_builder<g3d::device::primitive_type::triangle_strip, T, 0>
{
	using	value_type	= float;
	static constexpr g3d::device::primitive_type	prim_type	= g3d::device::primitive_type::triangle_strip;

	template<std::size_t OS>
	using	other_t					= vertex_builder<prim_type, T, OS>;


	template<class... VtxParams>
	static constexpr auto rect(vec3<value_type> _pos, size3<value_type> _size, VtxParams&&... _params) noexcept -> other_t<4>
	{
		return	other_t<4>
				(
					T{{_pos.x              , _pos.y               , _pos.z, 1}, std::forward<VtxParams>(_params)... },
					T{{_pos.x + _size.width, _pos.y               , _pos.z, 1}, std::forward<VtxParams>(_params)... },
					T{{_pos.x              , _pos.y + _size.height, _pos.z, 1}, std::forward<VtxParams>(_params)... },
					T{{_pos.x + _size.width, _pos.y + _size.height, _pos.z, 1}, std::forward<VtxParams>(_params)... }
				);
	}
};



AMTRS_G3D_NAMESPACE_END
#endif

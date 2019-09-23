#ifndef	__libamtrs__graphics__box__hpp
#define	__libamtrs__graphics__box__hpp
#include "def.hpp"
#include "size.hpp"
#include "rect.hpp"
#include "vec.hpp"
AMTRS_NAMESPACE_BEGIN

template<class ValueT>
struct	box
		: vec3<ValueT>
		, size3<ValueT>
{
	using	value_type		= ValueT;
	using	position_type	= vec3<value_type>;
	using	size_type		= size3<value_type>;

	box() = default;
	box(const box&) = default;
	box(box&&) = default;
	box& operator = (const box&) = default;
	box& operator = (box&&) = default;

	box(position_type _pos, size_type _size) : position_type(_pos), size_type(_size) {}

	constexpr value_type left() const noexcept { return position_type::x; }
	constexpr value_type top() const noexcept  { return position_type::y; }
	constexpr value_type front() const noexcept  { return position_type::z; }
	constexpr value_type right() const noexcept { return position_type::x + size_type::width; }
	constexpr value_type bottom() const noexcept  { return position_type::y + size_type::height; }
	constexpr value_type back() const noexcept  { return position_type::z + size_type::depth; }

	//! Left Top Front
	constexpr position_type ltf() const noexcept { return {position_type::x, position_type::y, position_type::z}; }

	//! Left Top Back
	constexpr position_type ltb() const noexcept { return {position_type::x, position_type::y, position_type::z + size_type::depth}; }

	//! Left Bottom Front
	constexpr position_type lbf() const noexcept { return {position_type::x, position_type::y + size_type::height, position_type::z}; }

	//! Left Bottom Back
	constexpr position_type lbb() const noexcept { return {position_type::x, position_type::y + size_type::height, position_type::z + size_type::depth}; }

	//! Right Top Front
	constexpr position_type rtf() const noexcept { return {position_type::x + size_type::width, position_type::y, position_type::z}; }

	//! Right Top Back
	constexpr position_type rtb() const noexcept { return {position_type::x + size_type::width, position_type::y, position_type::z + size_type::depth}; }

	//! Right Bottom Front
	constexpr position_type rbf() const noexcept { return {position_type::x + size_type::width, position_type::y + size_type::height, position_type::z}; }

	//! Right Bottom Back
	constexpr position_type rbb() const noexcept { return {position_type::x + size_type::width, position_type::y + size_type::height, position_type::z + size_type::depth}; }

	constexpr       position_type& position()       noexcept { return *this; }
	constexpr const position_type& position() const noexcept { return *this; }
	constexpr       size_type& size()       noexcept { return *this; }
	constexpr const size_type& size() const noexcept { return *this; }


	template<class POS>
	constexpr bool is_inner(POS _pos) const noexcept { return size_type::is_inner(_pos - POS(position)); }


	constexpr operator rect<value_type> () const noexcept
	{
		return	rect<value_type>(vec2<value_type>(position()), size2<float>(size()));
	}

	constexpr box  operator +  (const box& _r) const noexcept { return {position() + _r.position(), size() + _r.size()}; }
	constexpr box operator - (const box& _r) const noexcept { return {position() - _r.position(), size() - _r.size()}; }
	constexpr box operator * (value_type _scale) const noexcept { return {position() * _scale, size() * _scale}; }
	constexpr box operator / (value_type _scale) const noexcept { return {position() / _scale, size() / _scale}; }
	constexpr box& operator += (const box& _r) const noexcept { *this = *this + _r; return *this; }
	constexpr box& operator -= (const box& _r) const noexcept { *this = *this - _r; return *this; }
};



AMTRS_NAMESPACE_END
#endif

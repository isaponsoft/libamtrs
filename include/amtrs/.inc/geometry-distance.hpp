/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__graphics__distance__hpp
#define	__libamtrs__graphics__distance__hpp
AMTRS_NAMESPACE_BEGIN

enum class distance_type
{
	meter,
	millmeter,
	centimeter,
	kilometer,
	inch,
	pixel,
	dpi
};

template<distance_type DistanceT, class ValueT = float>
struct	basic_distance;

/*
	basic_distance<distance_type::millmeter>	mm(5.0f);
	auto	inch	= basic_distance<distance_type::inch>(mm);
	auto	cm		= basic_distance<distance_type::centimeter>(mm);
	auto	km		= basic_distance<distance_type::kilometer>(inch);
	auto	px		= basic_distance<distance_type::pixel>(basic_distance<distance_type::meter>(cm), basic_distance<distance_type::dpi>(448));
	auto	px2		= basic_distance<distance_type::pixel>(cm, basic_distance<distance_type::dpi>(448));

	std::cout << "millmeter : " << mm.get() << std::endl;
	std::cout << "inch      : " << inch.get() << std::endl;
	std::cout << "cm        : " << cm.get() << std::endl;
	std::cout << "km        : " << km.get() << std::endl;
	std::cout << "px        : " << (int)px.get() << std::endl;
 */
template<class T = float>	using	meters_t		= basic_distance<distance_type::meter, T>;
template<class T = float>	using	millmeters_t	= basic_distance<distance_type::millmeter, T>;
template<class T = float>	using	centimeters_t	= basic_distance<distance_type::centimeter, T>;
template<class T = float>	using	killometers_t	= basic_distance<distance_type::kilometer, T>;
template<class T = float>	using	inch_t			= basic_distance<distance_type::inch, T>;
template<class T = float>	using	pixel_t			= basic_distance<distance_type::pixel, T>;
template<class T = float>	using	dpi_t			= basic_distance<distance_type::dpi, T>;


template<distance_type DistanceT, class T>
struct	distance_impl
{
	distance_impl(T _value) : m_value(_value) {}
	T	m_value;
};

template<class T>
struct	distance_impl<distance_type::meter, T>
{
	distance_impl(T _value) : m_value(_value) {}
	distance_impl(const distance_impl<distance_type::inch, T>& _inch) : m_value(_inch.m_value * (0.0254)) {}
	distance_impl(const distance_impl<distance_type::millmeter, T>& _mill) : m_value(_mill.m_value / (1000)) {}
	distance_impl(const distance_impl<distance_type::centimeter, T>& _centi) : m_value(_centi.m_value / (10)) {}
	distance_impl(const distance_impl<distance_type::kilometer, T>& _kilo) : m_value(_kilo.m_value * (1000)) {}
	T	m_value;
};

template<class T>
struct	distance_impl<distance_type::millmeter, T>
{
	distance_impl(T _value) : m_value(_value) {}
	distance_impl(const distance_impl<distance_type::meter, T>& _meter) : m_value(_meter.m_value * (1000)) {}
	T	m_value;
};

template<class T>
struct	distance_impl<distance_type::centimeter, T>
{
	distance_impl(T _value) : m_value(_value) {}
	distance_impl(const distance_impl<distance_type::meter, T>& _meter) : m_value(_meter.m_value * (10)) {}
	T	m_value;
};

template<class T>
struct	distance_impl<distance_type::kilometer, T>
{
	distance_impl(T _value) : m_value(_value) {}
	distance_impl(const distance_impl<distance_type::meter, T>& _meter) : m_value(_meter.m_value / (1000)) {}
	T	m_value;
};

template<class T>
struct	distance_impl<distance_type::pixel, T>
{
	distance_impl(T _value) : m_value(_value) {}
	distance_impl(const distance_impl<distance_type::inch, T>& _inch, const distance_impl<distance_type::dpi, T>& _dpi)
		 : m_value(_inch.m_value * _dpi.m_value) {}
	distance_impl(const distance_impl<distance_type::meter, T>& _meter, const distance_impl<distance_type::dpi, T>& _dpi)
		 : distance_impl(distance_impl<distance_type::inch, T>(_meter), _dpi) {}

	T	m_value;
};

template<class T>
struct	distance_impl<distance_type::inch, T>
{
	distance_impl(T _value) : m_value(_value) {}
	distance_impl(const distance_impl<distance_type::meter, T>& _meter)
		 : m_value(_meter.m_value * (1.0 / 0.0254)) {}

	T	m_value;
};

template<distance_type DistanceT, class ValueT>
struct	basic_distance
		: private distance_impl<DistanceT, ValueT>
{
	using	value_type	= ValueT;
	using	base_type	= distance_impl<DistanceT, ValueT>;

	constexpr basic_distance(value_type _v)
		: base_type(_v)
	{}

	template<distance_type FromDistanceT>
	constexpr basic_distance(basic_distance<FromDistanceT, ValueT> _from)
		: base_type(_from)
	{}

	template<distance_type FromDistanceT, class ScaleT>
	constexpr basic_distance(basic_distance<FromDistanceT, ValueT> _from, ScaleT _scale)
		: base_type(_from, _scale)
	{}

	constexpr value_type get() const noexcept { return distance_impl<DistanceT, ValueT>::m_value; }
	constexpr explicit operator value_type () const noexcept { return get(); }

	template<distance_type ToDistanceT>
	constexpr basic_distance<ToDistanceT, ValueT> to() const noexcept
	{
		return	basic_distance<ToDistanceT, ValueT>(*this);
	}

	template<distance_type ToDistanceT, class ScaleT>
	constexpr basic_distance<ToDistanceT, ValueT> to(ScaleT _scale) const noexcept
	{
		return	basic_distance<ToDistanceT, ValueT>(*this, _scale);
	}

	template<distance_type, class>
	friend	struct	basic_distance;
};


AMTRS_NAMESPACE_END
#endif
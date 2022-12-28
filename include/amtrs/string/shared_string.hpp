/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__shared_string__hpp
#define	__libamtrs__shared_string__hpp
#include "../memory/shared_buffer.hpp"
#include "text.hpp"
AMTRS_NAMESPACE_BEGIN
AMTRS_IMPLEMENTS_BEGIN(shared_string)
struct	header
{
	size_t	hash;
};
AMTRS_IMPLEMENTS_END(shared_string)


class	shared_string
		: protected shared_buffer<char, AMTRS_IMPLEMENTS(shared_string)::header>
{
	using	super_type	= shared_buffer<char, AMTRS_IMPLEMENTS(shared_string)::header>;
public:
	using	char_type		= super_type::value_type;
	using	value_type		= super_type::value_type;
	using	size_type		= super_type::size_type;
	using	pointer			= super_type::pointer;
	using	const_pointer	= super_type::const_pointer;
	using	reference		= super_type::reference;
	using	const_reference	= super_type::const_reference;
	using	iterator		= super_type::iterator;
	using	const_iterator	= super_type::const_iterator;
	using	text_type		= text;

	shared_string() noexcept = default;
	shared_string(shared_string const&) noexcept = default;
	shared_string(shared_string&&) noexcept = default;
	shared_string& operator = (shared_string const&) noexcept = default;
	shared_string& operator = (shared_string&&) noexcept = default;


	template<class E>
	shared_string(std::initializer_list<E> _init)
		: super_type(std::move(_init))
	{
		header().hash	= text(*this).hash();
	}


	template<class It>
	shared_string(It _first, It _last)
		: super_type(std::move(_first), std::move(_last))
	{
		header().hash	= text(*this).hash();
	}


	template<class It>
	shared_string(It _first, size_t _size)
		: super_type(std::move(_first), std::move(_size))
	{
		header().hash	= text(*this).hash();
	}


	shared_string(const_pointer _src)
		: super_type(text_type(_src))
	{
		header().hash	= text_type(*this).hash();
	}

	shared_string(text_type _src)
		: super_type(_src.begin(), _src.end())
	{
		header().hash	= text_type(*this).hash();
	}


	auto data() const noexcept -> const_pointer { return super_type::data(); }
	auto begin() const noexcept -> const_iterator { return super_type::cbegin(); }
	auto end() const noexcept -> const_iterator { return super_type::cend(); }
	auto cbegin() const noexcept -> const_iterator { return super_type::cbegin(); }
	auto cend() const noexcept -> const_iterator { return super_type::cend(); }
	auto size() const noexcept -> size_type { return super_type::size(); }
	auto empty() const noexcept -> bool { return size() == 0; }
	auto hash() const noexcept -> size_type { return empty() ? 0 : header().hash; }
	operator text_type () const noexcept { return {data(), data() + size()}; }

	template<class R>	auto operator == (R&& _r) const noexcept -> bool { return  _compare(_r); }
	template<class R>	auto operator != (R&& _r) const noexcept -> bool { return !_compare(_r); }

	auto operator + (text_type _t) const -> shared_string
	{
		if (_t.empty())
		{
			return	*this;
		}
		shared_string	ret;
		((super_type&)ret)	= super_type(size() + _t.size());
		auto			dst	= ((super_type&)ret).data();
		std::copy_n(data(), size(), dst);
		dst += size();
		std::copy_n(_t.data(), _t.size(), dst);
		ret.header().hash	= text_type(ret).hash();
		return	ret;
	}

	auto operator += (text_type _t) -> shared_string&
	{
		*this	= *this + _t;
		return	*this;
	}

protected:
	bool _compare(shared_string const& _r) const noexcept
	{
		return	size() == _r.size() && hash() == _r.hash() && std::equal(begin(), end(), _r.begin());
	}

	template<class R>
	bool _compare(R&& _r) const noexcept
	{
		text_type	t(_r);
		return	size() == t.size() && std::equal(begin(), end(), t.begin());
	}

};


AMTRS_NAMESPACE_END
namespace std
{
	template<>
	struct	hash<amtrs::shared_string>
	{
	public:
		size_t operator() (amtrs::shared_string const& data) const noexcept
		{
			return	data.hash();
		}
	};
}
#endif

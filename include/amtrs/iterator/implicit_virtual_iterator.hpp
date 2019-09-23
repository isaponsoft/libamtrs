#ifndef	__libamtrs__iterator__implicit_virtual_iterator__hpp
#define	__libamtrs__iterator__implicit_virtual_iterator__hpp
#include "def.hpp"
AMTRS_NAMESPACE_BEGIN

/*!
 * 暗黙的な型変換を行う仮想イテレータ
 * ----------------------------------------------------------------------------
 *
 *
 * ----------------------------------------------------------------------------
 *	example)
 *		void each(implicit_virtual_iterator<std::string_view>& _first, implicit_virtual_iterator<std::string_view>& _last)
 *		{
 *			for (; _first != _last; ++_first)
 *			{
 *				std::cout << *_first << std::endl;
 *			}
 *		}
 *	
 *		template<class It>
 *		void each_wrapper(It _first, It _last)
 *		{
 *			auto	first	= make_implicit_virtual_iterator<std::string_view>(_first);
 *			auto	last	= make_implicit_virtual_iterator<std::string_view>(_last);
 *			each(first, last);
 *		}
 *
 *
 *		std::vector<std::string>	a;
 *		each_wrapper(a.begin(), a.end());
 *
 */
template<class T>
class	implicit_virtual_iterator
{
public:
	using	value_type		= T;

	virtual value_type operator * () const noexcept = 0;
	virtual bool operator == (const implicit_virtual_iterator<T>& _r) const noexcept = 0;
	virtual implicit_virtual_iterator<T>& operator ++ () noexcept = 0;

	bool operator != (const implicit_virtual_iterator<T>& _r) const noexcept { return !(*this == _r); }
};


template<class T, class S>
class	implicit_virtual_iterator__
		: public implicit_virtual_iterator<T>
{
public:
	using	super_type		= implicit_virtual_iterator<T>;
	using	source_iterator	= S;
	using	value_type		= typename super_type::value_type;

	implicit_virtual_iterator__(source_iterator _r)
		: mIterator(std::move(_r))
	{}

	implicit_virtual_iterator__(const implicit_virtual_iterator__&) = default;
	implicit_virtual_iterator__(implicit_virtual_iterator__&&) = default;
	implicit_virtual_iterator__& operator = (const implicit_virtual_iterator__&) = default;
	implicit_virtual_iterator__& operator = (implicit_virtual_iterator__&&) = default;


	virtual value_type operator * () const noexcept override
	{
		return	value_type(*mIterator);
	}

	virtual bool operator == (const implicit_virtual_iterator<T>& _r) const noexcept override
	{
		return	mIterator == static_cast<const implicit_virtual_iterator__&>(_r).mIterator;
	}

	virtual implicit_virtual_iterator<T>& operator ++ () noexcept override
	{
		++mIterator;
		return	*this;
	}

private:
	source_iterator	mIterator;
};




template<class T, class S>
inline implicit_virtual_iterator__<T, S> make_implicit_virtual_iterator(S _iterator)
{
	return	implicit_virtual_iterator__<T, S>(std::move(_iterator));
}

AMTRS_NAMESPACE_END
#endif

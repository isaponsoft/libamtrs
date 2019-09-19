#ifndef	__libamtrs__string__string_view__hpp
#define	__libamtrs__string__string_view__hpp
#ifdef	__cplusplus
#include <string_view>
#include "string_view.h"
AMTRS_NAMESPACE_BEGIN


class	string_view
		: public amtrs_string_view	// hidden 'txt' and 'len'.
{
public:
	using	super_type	= amtrs_string_view;
	using	super_type::super_type;
	using	super_type::operator =;
	using	super_type::operator ==;
	using	super_type::operator !=;
	using	super_type::size;
	using	super_type::length;
	using	super_type::empty;
	using	super_type::begin;
	using	super_type::end;
	using	super_type::cbegin;
	using	super_type::cend;
	using	super_type::data;
	using	super_type::swap;

private:
	using	super_type::txt;
	using	super_type::len;
};


AMTRS_NAMESPACE_END
#endif	// __cplusplus
#endif

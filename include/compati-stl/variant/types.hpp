/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__compati__variant__types__hpp
#define	__libamtrs__compati__variant__types__hpp
AMTRS_NAMESPACE_BEGIN

struct	bad_variant_access
		:  public std::exception
{
	virtual const char* what() const noexcept override { return "bad_variant_access"; }
};


AMTRS_NAMESPACE_END
#endif

/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__string__join__hpp
#define	__libamtrs__string__join__hpp
AMTRS_NAMESPACE_BEGIN


template<class T, class First, class Last, class Dem>
T join(First _first, Last _last, Dem _dem)
{
	std::string_view	d(_dem);

	size_t	total	= 0;
	if (_first != _last)
	{
		total	= std::string_view(*_first).size();
		auto it = _first;
		for (++it; it != _last; ++it)
		{
			total += d.size() + std::string_view(*it).size();
		}
	}

	T	retval;
	if (total > 0)
	{
		retval.resize(total);

		auto*	c	= retval.data();
		if (_first != _last)
		{
			std::copy_n(std::string_view(*_first).data(), std::string_view(*_first).size(), c);
			c += std::string_view(*_first).size();
			auto it = _first;
			for (++it; it != _last; ++it)
			{
				std::copy_n(d.data(), d.size(), c);
				c += d.size();

				std::copy_n(std::string_view(*it).data(), std::string_view(*it).size(), c);
				c += std::string_view(*it).size();
			}
		}
	}
	return	retval;
}



AMTRS_NAMESPACE_END
#endif

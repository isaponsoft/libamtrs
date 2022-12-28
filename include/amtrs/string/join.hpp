/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__string__join__hpp
#define	__libamtrs__string__join__hpp
AMTRS_NAMESPACE_BEGIN


template<class T, class First, class Last, class Dem>
T join(First _first, Last _last, Dem _dem)
{
	text	d(_dem);

	size_t	total	= 0;
	if (_first != _last)
	{
		total	= text(*_first).size();
		auto it = _first;
		for (++it; it != _last; ++it)
		{
			total += d.size() + text(*it).size();
		}
	}

	T	retval;
	if (total > 0)
	{
		retval.resize(total);

		auto*	c	= retval.data();
		if (_first != _last)
		{
			std::copy_n(text(*_first).data(), text(*_first).size(), c);
			c += text(*_first).size();
			auto it = _first;
			for (++it; it != _last; ++it)
			{
				std::copy_n(d.data(), d.size(), c);
				c += d.size();

				std::copy_n(text(*it).data(), text(*it).size(), c);
				c += text(*it).size();
			}
		}
	}
	return	retval;
}



AMTRS_NAMESPACE_END
#endif

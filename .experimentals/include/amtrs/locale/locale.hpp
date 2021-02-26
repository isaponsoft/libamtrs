/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__locale__locale__hpp
#define	__libamtrs__locale__locale__hpp
#include <string>
#include "../amtrs.hpp"
AMTRS_NAMESPACE_BEGIN

class	locale
{
public:
	locale();
	locale(locale&&);
	locale& operator = (locale&&);
	~locale();


	std::string get_language() const;


protected:
	struct	instance : ref_object {};
	ref<instance>	mInstance;
private:
	locale(locale const&) = delete;
	locale& operator = (locale const&) = delete;
};

AMTRS_NAMESPACE_END
#endif

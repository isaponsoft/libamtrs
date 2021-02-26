/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/locale/locale.hpp>
#include <amtrs/java/java/util/Locale.hpp>
AMTRS_NAMESPACE_BEGIN
#if	0
class	locale
{
public:
	locale();
	locale(locale&&);
	locale& operator = (locale&&);
	~locale();


	std::string get_locale() const;


protected:
	struct	instance : ref_object {};
	ref<instance>	mInstance;
private:
	locale(locale const&) = delete;
	locale& operator = (locale const&) = delete;
};
#endif


struct	locale_impl : locale
{
	struct	instance : locale::instance
	{
		
	};
};

locale::locale()
{
}

locale::locale(locale&& _r)
	: mInstance(std::move(_r.mInstance))
{
}

locale& locale::operator = (locale&& _r)
{
	mInstance	= std::move(_r.mInstance);
	return	*this;
}

locale::~locale()
{
}

std::string locale::get_language() const
{
	auto		language	= java::classes::java::util::Locale::find().getDefault().toString();
	return	std::to_string((jstring)language.get());
}


AMTRS_NAMESPACE_END

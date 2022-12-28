/* Copyright (c) 2022, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__lang__ini__hpp
#define	__libamtrs__lang__ini__hpp
#include <unordered_map>
#include "../amtrs.hpp"
#include "../memory/ref_object.hpp"
#include "../string/format.hpp"
#include "../string/split_iterator.hpp"
#include "../string/sstring.hpp"
#include "../string/trim.hpp"


AMTRS_NAMESPACE_BEGIN
template<class StringT>
struct	inifile_traits
{
	using	string_type		= StringT;
	using	text_type		= basic_text<typename string_type::value_type>;

	template<class V>
	using	containor_type	= std::unordered_map<string_type, V>;
};


// ******************************************************************
template<class StringT = sstring, class TraitsT = inifile_traits<StringT>>
class	inifile
{
public:
	using	traits_type		= TraitsT;
	using	string_type		= typename traits_type::string_type;
	using	text_type		= typename traits_type::text_type;
	using	section_type	= typename traits_type::template containor_type<string_type>;
	using	containor_type	= typename traits_type::template containor_type<section_type>;

	inifile(string_type const& _txt);

	auto begin() noexcept { return mItems.begin(); }
	auto end() noexcept { return mItems.end(); }
	auto begin() const noexcept { return mItems.begin(); }
	auto end() const noexcept { return mItems.end(); }

	auto has(text_type const& _key) const noexcept -> bool;
	auto has(text_type const& _sec, text_type const& _key) const noexcept -> bool;
	auto get(text_type const& _key) const noexcept -> text_type;
	auto get(text_type const& _sec, text_type const& _key) const noexcept -> text_type;
	void set(text_type const& _key, text_type const& _val);
	void set(text_type const& _sec, text_type const& _key, text_type const& _val);

	template<class OutT = string_type>
	auto to_string(text_type const& _eol = "\r\n") -> OutT;


protected:
	containor_type	mItems;
};



// ******************************************************************
template<class StringT = sstring, class TraitsT = inifile_traits<StringT>>
class	iniconf
		: public ref_object
		, public inifile<StringT, TraitsT>
{
public:
	using	super_type	= inifile<StringT, TraitsT>;
	using	string_type	= typename super_type::string_type;
	using	text_type	= typename super_type::text_type;

	iniconf(text_type const& _ini, ref<iniconf> _parent = {})
		: super_type(_ini)
		, mParent(std::move(_parent))
	{}

	auto has(text_type const& _key) const noexcept -> bool;
	auto has(text_type const& _sec, text_type const& _key) const noexcept -> bool;
	auto get(text_type const& _key) const noexcept -> text_type;
	auto get(text_type const& _sec, text_type const& _key) const noexcept -> text_type;
	auto parent() const noexcept;

protected:
	ref<iniconf>	mParent;
};




template<class StringT, class TraitsT>
inifile<StringT, TraitsT>::inifile(string_type const& _txt)
{
	text_type	sec("");
	for (auto line : split_chars(_txt, "\r\n"))
	{
		text_type	l	= trim_first<text_type>(line);
		if (l.empty() || l.front() == '#' || l.front() == ';')
		{
			continue;
		}

		if (l.front() == '[' && l.back() == ']')
		{
			sec		= trim(l.substr(1, l.size()-2));
			continue;
		}

		text_type	key;
		text_type	val;
		auto		pos	= l.find_first_of('=');
		if (pos == text_type::npos)
		{
			key	= l;
			val	= "";
		}
		else
		{
			key	= trim(l.substr(0, pos));
			val	= trim(l.substr(pos+ 1));
		}
		set(sec, key, val);
	}
}



template<class StringT, class TraitsT>
auto inifile<StringT, TraitsT>::has(text_type const& _key) const noexcept -> bool
{
	text_type	sec("");
	text_type	key("");
	auto		pos	= _key.find_first_of('.');
	if (pos == text_type::npos)
	{
		key	= _key;
	}
	else
	{
		sec	= _key.substr(0, pos);
		key	= _key.substr(pos + 1);
	}
	return	has(sec, key);
}


template<class StringT, class TraitsT>
auto inifile<StringT, TraitsT>::has(text_type const& _sec, text_type const& _key) const noexcept -> bool
{
	auto	sec	= mItems.find(_sec);
	if (sec != mItems.end())
	{
		auto	val	= sec->second.find(_key);
		if (val != sec->second.end())
		{
			return	true;
		}
	}
	return	false;
}


template<class StringT, class TraitsT>
auto inifile<StringT, TraitsT>::get(text_type const& _key) const noexcept -> text_type
{
	text_type	sec("");
	text_type	key("");
	auto		pos	= _key.find_first_of('.');
	if (pos == text_type::npos)
	{
		key	= _key;
	}
	else
	{
		sec	= _key.substr(0, pos);
		key	= _key.substr(pos + 1);
	}
	return	get(sec, key);
}


template<class StringT, class TraitsT>
auto inifile<StringT, TraitsT>::get(text_type const& _sec, text_type const& _key) const noexcept -> text_type
{
	auto	sec	= mItems.find(_sec);
	if (sec != mItems.end())
	{
		auto	val	= sec->second.find(_key);
		if (val != sec->second.end())
		{
			return	text_type(val->second);
		}
	}
	return	"";
}


template<class StringT, class TraitsT>
void inifile<StringT, TraitsT>::set(text_type const& _key, text_type const& _val)
{
	text_type	sec("");
	text_type	key("");
	auto		pos	= _key.find_first_of('.');
	if (pos == text_type::npos)
	{
		key	= _key;
	}
	else
	{
		sec	= _key.substr(0, pos);
		key	= _key.substr(pos + 1);
	}
	set(sec, key, _val);
}


template<class StringT, class TraitsT>
void inifile<StringT, TraitsT>::set(text_type const& _sec, text_type const& _key, text_type const& _val)
{
	auto	sec	= mItems.find(_sec);
	if (sec == mItems.end())
	{
		sec	= mItems.emplace(string_type(_sec), section_type()).first;
	}
	sec->second.emplace(_key, _val);
}


template<class StringT, class TraitsT>
template<class OutT>
auto inifile<StringT, TraitsT>::to_string(text_type const& _eol) -> OutT
{
	OutT	o("");
	for (auto& sec : *this)
	{
		if (!sec.first.empty())
		{
			o = o + "[";
			o = o + (text)sec.first;
			o = o + "]";
			o = o + _eol;
		}
		for (auto& val : sec.second)
		{
			o = o + (text)val.first;
			o = o + "=";
			o = o + (text)val.second;
			o = o + _eol;
		}
		o = o + _eol;
	}
	return	o;
}



template<class StringT, class TraitsT>
auto iniconf<StringT, TraitsT>::has(text_type const& _key) const noexcept -> bool
{
	if (super_type::has(_key) || (mParent && mParent->has(_key)))
	{
		return	true;
	}
	return	false;
}


template<class StringT, class TraitsT>
auto iniconf<StringT, TraitsT>::has(text_type const& _sec, text_type const& _key) const noexcept -> bool
{
	if (super_type::has(_sec, _key) || (mParent && mParent->has(_sec, _key)))
	{
		return	true;
	}
	return	false;
}


template<class StringT, class TraitsT>
auto iniconf<StringT, TraitsT>::get(text_type const& _key) const noexcept -> text_type
{
	if (super_type::has(_key))
	{
		return	super_type::get(_key);
	}
	if (mParent)
	{
		return	mParent->get(_key);
	}
	return	"";
}


template<class StringT, class TraitsT>
auto iniconf<StringT, TraitsT>::get(text_type const& _sec, text_type const& _key) const noexcept -> text_type
{
	if (super_type::has(_sec, _key))
	{
		return	super_type::get(_sec, _key);
	}
	if (mParent)
	{
		return	mParent->get(_sec, _key);
	}
	return	false;
}


template<class StringT, class TraitsT>
auto iniconf<StringT, TraitsT>::parent() const noexcept
{
	return	mParent.get();
}



AMTRS_NAMESPACE_END
#endif

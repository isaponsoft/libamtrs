/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef __libamtrs__filesystem__zlp__zipfs__hpp
#define __libamtrs__filesystem__zlp__zipfs__hpp
#include <unordered_map>
#include "zip_entry.hpp"
AMTRS_FILESYSTEM_ZIP_NAMESPACE_BEGIN



template<class In>
struct	zipfs_traits
{
	using	string_type		= std::string;
	using	archive_type	= zip_archive<In>;
	using	value_type		= typename archive_type::value_type;
	using	storage_type	= std::unordered_map<string_type, value_type>;
};



template<class In, class Traits = zipfs_traits<In>>
class	zipfs
{
public:
	using	traits_type		= Traits;
	using	string_type		= typename traits_type::string_type;
	using	archive_type	= typename traits_type::archive_type;
	using	value_type		= typename traits_type::value_type;
	using	storage_type	= typename traits_type::storage_type;

	class	iterator
	{
	public:
		iterator() = default;
		iterator(const iterator&) = default;
		iterator(iterator&&) = default;
		iterator(typename storage_type::iterator _i) : mIterator(_i) {}
		iterator& operator = (const iterator&) = default;
		iterator& operator = (iterator&&) = default;
		iterator& operator ++ () noexcept { ++mIterator; return *this; }
		iterator  operator ++ (int) noexcept { iterator r(*this); ++mIterator; return r; }
		value_type* operator -> () const noexcept { return &mIterator->second; }
		value_type& operator *  () const noexcept { return  mIterator->second; }
		bool operator == (const iterator& _r) const noexcept { return mIterator == _r.mIterator; }
		bool operator != (const iterator& _r) const noexcept { return mIterator != _r.mIterator; }
	private:
		typename storage_type::iterator	mIterator;
	};


	template<class... Args>
	zipfs(Args&&... _args)
		: mArchive(std::forward<Args>(_args)...)
	{
		for (auto e : mArchive)
		{
			mCache.emplace(e.filename(), std::move(e));
		}
		mArchive.rdata().clear();
	}


	auto open(const string_type& _filename)
	{
		using	type	= decltype(mArchive.open(std::declval<value_type>()));
		auto	e	= find(_filename);
		if (!e)
		{
			return	type(mArchive.rdata(), 0);
		}
		return	mArchive.open(*e);
	}


	auto open(const value_type& _entry)
	{
		return	mArchive.open(_entry);
	}


	value_type* find(const string_type& _filename)
	{
		auto	it	= mCache.find(_filename);
		return	it != mCache.end()
				? &it->second
				: nullptr;
	}

	iterator begin() { return iterator(mCache.begin()); }
	iterator end() { return iterator(mCache.end()); }

private:
	archive_type	mArchive;
	storage_type	mCache;
};


template<class In>
auto make_zipfs(In& _in)
{
	return	zipfs<In&>(_in);
}

template<class In>
auto make_zipfs(In&& _in)
{
	return	zipfs<In>(std::move(_in));
}



AMTRS_FILESYSTEM_ZIP_NAMESPACE_END
#endif

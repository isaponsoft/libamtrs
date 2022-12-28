/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__filesystem__findfiles__hpp
#define	__libamtrs__filesystem__findfiles__hpp
#include <stack>
#include "../memory/ref_object.hpp"
#include "../string/wildcard.hpp"
AMTRS_FILESYSTEM_NAMESPACE_BEGIN



/*
 * directory_iterator に似ているが、ワイルドカードで一致したファイルのみをリストアップする。
 * for (directory_entry const& e : findfiles("[Tt]est/?.zip"))
 *
 */
template<class MatchF>
class	basic_findfiles
{
public:
	using	string_type	= std::string;
	using	match_func	= MatchF;

	struct	findingstate : ref_object
	{
		std::string							parent;
		std::string							current;
		std::string							child;

		std::filesystem::directory_iterator	di;
		std::filesystem::directory_iterator	cur;
		std::filesystem::directory_iterator	end;
		std::filesystem::directory_entry	e;

		void init()
		{
			di		= std::filesystem::directory_iterator(parent.empty() ? "." : parent);
			cur		= std::filesystem::begin(di);
			end		= std::filesystem::end(di);
		}

		bool seek()
		{
			while (cur != end && !match_func{}(current, filesystem::filename(cur->path().string())))
			{
				++cur;
			}
			if (cur != end)
			{
				std::string	path;
				if (!parent.empty())
				{
					path	= parent;
					path	+= "/";
				}
				path	+= filesystem::filename(cur->path().string());
				e		= std::filesystem::directory_entry(normalize_path(path));
			}
			return	cur != end;
		}

		void next()
		{
			++cur;
		}
	};

	struct	findingstack : ref_object
	{
		std::stack<ref<findingstate>>	route;

		bool find_first(std::string_view _root, std::string_view _path)
		{
			ref<findingstate>	st		= new findingstate();
			auto				fdir	= first_dir(_path);

			st->parent	= _root;
			if (fdir.empty())
			{
				// ディレクトリ無し
				st->current	= _path;
			}
			else
			{
				// ディレクトリ有
				st->current	= trim_last_separator(fdir);
				st->child	= _path.substr(fdir.size());
			}
			st->init();
			route.push(std::move(st));
			return	seek();
		}

		bool next()
		{
			route.top()->next();
			return	seek();
		}

		bool seek()
		{
			while (!route.empty())
			{
				findingstate*	top	= route.top();
				if (!top->seek())
				{
					route.pop();
					continue;
				}
				// 名前が一致かつ、子の名前が存在しないならマッチしたものとして扱う
				if (top->child.empty())
				{
					break;
				}
				// さらに名前があるのにディレクトリではないので非マッチ
				if (!top->cur->is_directory())
				{
					top->next();
					continue;
				}
				// ディレクトリを辿る
				auto	root	= top->e.path().string();
				top->next();
				find_first(root, top->child);
				break;
			}
			return	!route.empty();
		}
	};


	class	iterator
	{
	public:
		iterator& operator ++ ()
		{
			if (!mRoute->next())
			{
				mRoute.clear();
			}
			return	*this;
		}

		std::filesystem::directory_entry const& operator * () const
		{
			return	mRoute->route.top()->e;
		}

		bool operator == (iterator const& r)
		{
			return	!operator != (r);
		}

		bool operator != (iterator const& r)
		{
			if (mRoute != r.mRoute)
			{
				return	true;
			}
			// empty() == empty()
			if (mRoute.empty())
			{
				return	false;
			}
			if (mRoute->route.top()->cur == r.mRoute->route.top()->cur)
			{
				return	false;
			}
			return	true;
		}

		void find_first(std::string_view _root, std::string_view _path)
		{
			if (!mRoute->find_first(_root, _path))
			{
				mRoute.clear();
			}
		}

	protected:
		ref<findingstack>	mRoute;


		friend	basic_findfiles;
	};


	basic_findfiles(string_type _patturn)
	{
		auto	dir	= compute_none_wildcard_path<mb_iterator<char>>(_patturn);
		mRootPath	= dir;
		mFilter		= _patturn.substr(dir.size());
	}

	iterator begin()
	{
		iterator	i;
		i.mRoute	= new findingstack();
		i.find_first(mRootPath, mFilter);
		return	i;
	}

	iterator end()
	{
		iterator	i;
		return	i;
	}


protected:
	template<class Iterator>
	static bool has_wildcard_patturn(std::string_view _path)
	{
		Iterator	ite;
		auto		end	= _path.data() + _path.size();
		for (auto cur = _path.data(); !ite.empty(cur, end); )
		{
			typename Iterator::int_type	c;
			cur	= ite.read(c, cur, end);
			if (c == '\\' || c == '[' || c == ']' || c == '*' || c == '+' || c == '?' || c == '(' || c == ')')
			{
				return	true;
			}
		}
		return	false;
	}


	template<class Iterator>
	static std::string_view compute_none_wildcard_path(std::string_view _path)
	{
		std::string_view	path	= _path;
		for (;;)
		{
			std::string_view	dir	= first_dir(path);
			if (dir.empty() || has_wildcard_patturn<Iterator>(trim_last_separator(dir)))
			{
				break;
			}
			path	= path.substr(dir.size());
		}
		return	_path.substr(0, path.data() - _path.data());
	}

	string_type	mRootPath;
	string_type	mFilter;
};



struct	wccompare
{
	bool operator () (std::string_view _patturn, std::string_view _filename)
	{
		return	wcmatch_mb(_patturn, _filename);
	}
};


struct	regexcompare
{
	bool operator () (std::string_view _patturn, std::string_view _filename)
	{
		std::regex	re{std::string(_patturn)};
		std::string	name(_filename);
		std::smatch	match;
		return	std::regex_match(name, match, re);
	}
};

using	findfiles		= basic_findfiles<wccompare>;
using	findfiles_regex	= basic_findfiles<regexcompare>;


AMTRS_FILESYSTEM_NAMESPACE_END
#endif

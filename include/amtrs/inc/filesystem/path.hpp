#if 0
/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef __libamtrs__filesystem__path__hpp
#define __libamtrs__filesystem__path__hpp
#include <cctype>
#include <string_view>
#include <vector>
#include "def.hpp"
AMTRS_FILESYSTEM_NAMESPACE_BEGIN
template<class PathTraits, class CharT = typename PathTraits::char_type, class TraitsT = std::char_traits<CharT>, class AllocatorT = std::allocator<CharT>> 
std::basic_string<CharT, TraitsT, AllocatorT> path_normalize(std::basic_string_view<CharT, TraitsT> _path)
{
	using	path_type	= PathTraits;
	using	view_type	= typename PathTraits::view_type;
	std::basic_string<CharT, TraitsT, AllocatorT>	retval;

	view_type	path		= _path;
	view_type	rootPath	= path_type::root_path(path);
	view_type	rootName	= path_type::root_name(path);
	if (!rootName.empty())
	{
		retval	+= rootName;
	}
	view_type	rootDir	= path_type::root_directory(path);
	if (!rootDir.empty())
	{
		retval	+= static_cast<CharT>(path_type::directory_separator());
	}
	std::vector<view_type>	items;
	view_type	relative	= path_type::relative_path(path);
	for (;;)
	{
		auto	pos	= path_type::find_first_directory_separator(relative);
		if (pos == view_type::npos)
		{
			items.push_back(relative);
			break;
		}
		auto	dir	= relative.substr(0, pos);
		if (dir.size() == 0 || !dir.compare(view_type(".", 1)))
		{
			// 空っぽ
		}
		else if (!dir.compare(view_type("..", 2)))
		{
			// ひとつ上へ
			if (!items.empty())
			{
				items.pop_back();
			}
		}
		else
		{
			items.push_back(dir);
		}
		relative	= relative.substr(dir.size() + 1);
	}

	if (!items.empty())
	{
		retval += items[0];
		for (auto i = 1; i < items.size(); ++i)
		{
			retval += static_cast<CharT>(path_type::directory_separator());
			retval += items[i];
		}
	}
	return	retval;
}

template<class CharT = char>
struct	posix_path_traits : std::char_traits<CharT>
{
	using	char_type	= typename std::char_traits<CharT>::char_type;
	using	int_type	= typename std::char_traits<CharT>::int_type;
	using	view_type	= std::basic_string_view<char_type>;

	static constexpr int_type directory_separator() noexcept { return '/'; }
	static constexpr bool is_directory_separator(int_type c) noexcept { return (c == '/'); }

	//! 最初に見つかったディレクトリ記号の位置を返す。
	//! 見つからない場合は view_type::npos を返す。
	static constexpr std::size_t find_first_directory_separator(view_type _path) noexcept
	{
		return	_path.find_first_of(view_type("/", 1));
	}

	//! 最後に見つかったディレクトリ記号の位置を返す。
	//! 見つからない場合は view_type::npos を返す。
	static constexpr std::size_t find_last_directory_separator(view_type _path) noexcept
	{
		return	_path.find_last_of(view_type("/", 1));
	}

	static constexpr view_type root_name(view_type _path) noexcept { return {_path.data(), 0}; }
	static constexpr view_type root_path(view_type _path) noexcept { return {_path.data(), 0}; }

	static constexpr view_type root_directory(view_type _path) noexcept
	{
		view_type	retval		= _path;
		return	(!retval.empty() && is_directory_separator(retval[0]))
				? retval.substr(0, 1)
				: view_type(retval.data(), 0);
	}

	static constexpr view_type relative_path(view_type _path) noexcept
	{
		return	root_directory(_path).empty()
				? _path
				: _path.substr(1);
	}

	static constexpr view_type parent_path(view_type _path) noexcept
	{
		auto	pos	= _path.find_last_of(directory_separator());
		if (pos == view_type::npos || pos == 0)
		{
			return	view_type(_path.data(), 0);
		}
		return	_path.substr(0, pos);
	}


	static constexpr view_type filename(view_type _path) noexcept
	{
		auto	retval	= _path.substr(parent_path(_path).size());
		// 先頭のディレクトリセパレータを除去
		if ((retval.size() > 0) && (is_directory_separator(retval[0])))
		{
			retval	= retval.substr(1);
		}
		return	retval;
	}

	static constexpr view_type stem(view_type _path) noexcept
	{
		auto	retval	= filename(_path);
		if (retval.size() > 0 && retval[0] != '.')
		{
			auto	pos		= retval.find_last_of('.');
			if (pos != view_type::npos)
			{
				retval	= retval.substr(0, pos);
			}
		}
		return	retval;
	}

	//! パスから拡張子を抜き出します。
	//! 拡張子が存在しない場合は empty を返します。
	static constexpr view_type extension(view_type _path) noexcept
	{
		auto	retval	= filename(_path);
		if ((retval.size() > 0) && (retval[0] == '.'))
		{
			return	view_type(nullptr, 0);
		}
		auto	pos		= retval.find_last_of('.');
		if (pos == view_type::npos)
		{
			return	view_type(nullptr, 0);
		}
		return	retval.substr(pos);
	}

	static constexpr std::basic_string<CharT> normalize(view_type _path)
	{
		return	path_normalize<posix_path_traits>(_path);
	}

};


template<class CharT = char>
struct	apple_path_traits : posix_path_traits<CharT>
{
	using	char_type	= typename std::char_traits<CharT>::char_type;
	using	int_type	= typename std::char_traits<CharT>::int_type;
	using	view_type	= std::basic_string_view<char_type>;

	static constexpr int_type directory_separator() noexcept { return '/'; }
	static constexpr bool is_directory_separator(int_type c) noexcept { return (c == '/'); }

	//! 最初に見つかったディレクトリ記号の位置を返す。
	//! 見つからない場合は view_type::npos を返す。
	static constexpr std::size_t find_first_directory_separator(view_type _path) noexcept
	{
		return	_path.find_first_of(view_type("/", 1));
	}

	//! 最後に見つかったディレクトリ記号の位置を返す。
	//! 見つからない場合は view_type::npos を返す。
	static constexpr std::size_t find_last_directory_separator(view_type _path) noexcept
	{
		return	_path.find_last_of(view_type("/", 1));
	}

	static constexpr view_type root_name(view_type _path) noexcept { return {_path.data(), 0}; }
	static constexpr view_type root_path(view_type _path) noexcept { return {_path.data(), 0}; }

	static constexpr view_type root_directory(view_type _path) noexcept
	{
		view_type	retval		= _path;
		return	(!retval.empty() && is_directory_separator(retval[0]))
				? retval.substr(0, 1)
				: view_type(retval.data(), 0);
	}

	static constexpr view_type relative_path(view_type _path) noexcept
	{
		return	root_directory(_path).empty()
				? _path
				: _path.substr(1);
	}

	static constexpr view_type parent_path(view_type _path) noexcept
	{
		auto	pos	= _path.find_last_of(directory_separator());
		if (pos == view_type::npos || pos == 0)
		{
			return	view_type(_path.data(), 0);
		}
		return	_path.substr(0, pos);
	}

	static constexpr view_type filename(view_type _path) noexcept
	{
		auto	retval	= _path.substr(parent_path(_path).size());
		if ((retval.size() > 0) && (is_directory_separator(retval[0])))
		{
			retval	= retval.substr(1);
		}
		return	retval;
	}


	static constexpr view_type stem(view_type _path) noexcept
	{
		auto	retval	= filename(_path);
		if (((retval.size() == 1) && (retval[0] == '.'))
		 || ((retval.size() == 2) && (retval[0] == '.') && (retval[1] == '.'))
		)
		{
			return	retval;
		}
		auto	pos		= retval.find_last_of('.');
		if (pos != view_type::npos)
		{
			retval	= retval.substr(0, pos);
		}
		return	retval;
	}

	//! パスから拡張子を抜き出します。
	//! 拡張子が存在しない場合は empty を返します。
	static constexpr view_type extension(view_type _path) noexcept
	{
		auto	retval	= filename(_path);
		if (((retval.size() == 1) && (retval[0] == '.'))
		 || ((retval.size() == 2) && (retval[0] == '.') && (retval[1] == '.'))
		)
		{
			return	view_type(nullptr, 0);
		}
		auto	pos		= retval.find_last_of('.');
		if (pos == view_type::npos)
		{
			return	view_type(nullptr, 0);
		}
		return	retval.substr(pos);
	}

	static constexpr std::basic_string<CharT> normalize(view_type _path)
	{
		return	path_normalize<apple_path_traits>(_path);
	}

};


template<class CharT = wchar_t>
struct	win32_path_traits : posix_path_traits<CharT>
{
	using	char_type	= typename std::char_traits<CharT>::char_type;
	using	int_type	= typename std::char_traits<CharT>::int_type;
	using	view_type	= std::basic_string_view<char_type>;

	static constexpr int_type directory_separator() noexcept { return '\\'; }
	static constexpr bool is_directory_separator(int_type c) noexcept { return (c == '\\') || (c == '/'); }

	//! 最初に見つかったディレクトリ記号の位置を返す。
	//! 見つからない場合は view_type::npos を返す。
	static constexpr std::size_t find_first_directory_separator(view_type _path) noexcept
	{
		return	_path.find_first_of(view_type("/\\", 2));
	}

	//! 最後に見つかったディレクトリ記号の位置を返す。
	//! 見つからない場合は view_type::npos を返す。
	static constexpr std::size_t find_last_directory_separator(view_type _path) noexcept
	{
		return	_path.find_last_of(view_type("/\\", 2));
	}

	static constexpr view_type root_name(view_type _path) noexcept
	{
		if (_path.size() >= 2 && std::isalpha(_path[0]) && (_path[1] == ':'))
		{
			return	{_path.data(), 2};
		}
		return {_path.data(), 0};
	}

	static constexpr view_type root_directory(view_type _path) noexcept
	{
		view_type	retval		= _path;
		view_type	rootName	= root_name(_path);
		if (!rootName.empty())
		{
			retval	= retval.substr(rootName.size());
		}
		return	(!retval.empty() && is_directory_separator(retval[0]))
				? retval.substr(0, 1)
				: view_type(retval.data(), 0);
	}

	// ========================================================================
	//! std::filesystem::path::root_path() の返す結果を満たす場合に root_path を返します。
	//! -----------------------------------------------------------------------
	//! 例えば path::root_path() は c:\ を返します。traits::root_path() は c:\ を返しますが
	//! 元の文字列が c:\ を持たない場合（例えば c:）のような場合には empty() になります。
	//! -----------------------------------------------------------------------
	static constexpr view_type root_path(view_type _path) noexcept
	{
		view_type	retval	= root_name(_path);
		return	(!retval.empty() && (_path.size() >= 3) && (is_directory_separator(_path[retval.size()])))
				? view_type(_path.data(), retval.size() + 1)
				: retval;
	}

	static constexpr view_type relative_path(view_type _path) noexcept
	{
		view_type	retval	= _path;
		view_type	root	= root_name(_path);
		if (!root.empty())
		{
			retval	= retval.substr(root.size());
		}
		return	root_directory(retval).empty()
				? retval
				: retval.substr(1);
	}

	static constexpr view_type parent_path(view_type _path) noexcept
	{
		auto	path	= relative_path(_path);
		auto	pos		= path.find_last_of(view_type("/\\", 2));
		if (pos == view_type::npos || pos == 0)
		{
			return	view_type(path.data(), 0);
		}
		return	path.substr(0, pos);
	}

	static constexpr view_type filename(view_type _path) noexcept
	{
		auto	path	= relative_path(_path);
		auto	retval	= path.substr(parent_path(path).size());
		if ((retval.size() > 0) && (is_directory_separator(retval[0])))
		{
			retval	= retval.substr(1);
		}
		return	retval;
	}

	static constexpr view_type stem(view_type _path) noexcept
	{
		auto	retval	= filename(_path);
		if (((retval.size() == 1) && (retval[0] == '.'))
		 || ((retval.size() == 2) && (retval[0] == '.') && (retval[1] == '.'))
		)
		{
			return	retval;
		}
		auto	pos		= retval.find_last_of('.');
		if (pos != view_type::npos)
		{
			retval	= retval.substr(0, pos);
		}
		return	retval;
	}

	//! パスから拡張子を抜き出します。
	//! 拡張子が存在しない場合は empty を返します。
	static constexpr view_type extension(view_type _path) noexcept
	{
		auto	retval	= filename(_path);
		if (((retval.size() == 1) && (retval[0] == '.'))
		 || ((retval.size() == 2) && (retval[0] == '.') && (retval[1] == '.'))
		)
		{
			return	view_type(nullptr, 0);
		}
		auto	pos		= retval.find_last_of('.');
		if (pos == view_type::npos)
		{
			return	view_type(nullptr, 0);
		}
		return	retval.substr(pos);
	}

	static constexpr std::basic_string<CharT> normalize(view_type _path)
	{
		return	path_normalize<win32_path_traits>(_path);
	}

};




#if		AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_WIN32
template<class CharT> using	path_traits	= win32_path_traits<CharT>;
#elif	(AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_IOS) || (AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_OSX)
template<class CharT> using	path_traits	= apple_path_traits<CharT>;
#else
template<class CharT> using	path_traits	= posix_path_traits<CharT>;
#endif





AMTRS_FILESYSTEM_NAMESPACE_END
#endif
#endif

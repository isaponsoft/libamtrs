/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__filesystem__path_util__hpp
#define	__libamtrs__filesystem__path_util__hpp
#include <cstring>
#include <string>
#include <string_view>
#include <type_traits>
#include "types.hpp"
#include "../string.hpp"
AMTRS_NAMESPACE_FILESYSTEM_BEGIN

template<class PathTraits>
using	path_string_view	= std::basic_string_view<typename PathTraits::value_type>;


// ============================================================================
//!	@brief	path_split で使用される正規化条件
// ----------------------------------------------------------------------------
enum class path_split_normalize
{
	none				= 0,		//!< 正規化しません
	lexically			= 1,		//!< 辞書的に正規化します
};



// ============================================================================
//!	@brief	path_split で使用される分割条件
// ----------------------------------------------------------------------------
enum class path_split_conditions
{
	all_required		= 0,		//!< 全てのパスを返します(デフォルト)
	root_name_exclusion	= 1,		//!< ルート名があればルート名を除外して返します
	root_path_exclusion	= 2,		//!< ルートパスがあればルートパスを除外して返します
};



// ****************************************************************************
//
// パス情報の比較
//
// ****************************************************************************

// ============================================================================
//!	@brief	二つのパスが同じかどうか比較します。
// ----------------------------------------------------------------------------
//!	windowsの時は / と \\ は同じ文字として扱われます。
// ----------------------------------------------------------------------------
template<class PathTraits>
constexpr int path_compare(const path_string_view<PathTraits>& _l, const path_string_view<PathTraits>& _r);


// ============================================================================
//!	@brief	ディレクトリを示すファイル名か調べます。
// ----------------------------------------------------------------------------
//!	_path == "." ||  _path == ".."
// ----------------------------------------------------------------------------
template<class PathTraits>
constexpr bool is_directory_filename(path_string_view<PathTraits> _path);


// ============================================================================
//!	@brief	カレントディレクトリを示すファイル名か調べます。
// ----------------------------------------------------------------------------
//!	_path == "."
// ----------------------------------------------------------------------------
template<class PathTraits>
constexpr bool is_current_directory_filename(path_string_view<PathTraits> _path);


// ============================================================================
//!	@brief	親ディレクトリを示すファイル名か調べます。
// ----------------------------------------------------------------------------
//!	_path == ".."
// ----------------------------------------------------------------------------
template<class PathTraits>
constexpr bool is_parent_directory_filename(path_string_view<PathTraits> _path);


template<class PathTraits>
constexpr bool has_root_path(path_string_view<PathTraits> _path);

template<class PathTraits>
constexpr bool has_root_name(path_string_view<PathTraits> _path);

template<class PathTraits>
constexpr bool has_root_directory(path_string_view<PathTraits> _path);

template<class PathTraits>
constexpr bool has_relative_path(path_string_view<PathTraits> _path);

template<class PathTraits>
constexpr bool has_parent_path(path_string_view<PathTraits> _path);

template<class PathTraits>
constexpr bool has_filename(path_string_view<PathTraits> _path);

template<class PathTraits>
constexpr bool has_stem(path_string_view<PathTraits> _path);

template<class PathTraits>
constexpr bool has_extension(path_string_view<PathTraits> _path);



// ****************************************************************************
//
// パスの正規化
//
// ****************************************************************************


// ============================================================================
//!	@brief	ディレクトリセパレータを正規化します。
// ----------------------------------------------------------------------------
//!	windows用の場合に / を \ に直します。generic の場合は何もしません。
// ----------------------------------------------------------------------------
template<class PathTraits, class Object>
constexpr void normalize_directory_separator(Object& _path_string, typename std::enable_if<!std::is_same<typename PathTraits::root_name::type, std::false_type>::value>::type*  = 0);


// ============================================================================
//!	@brief	ディレクトリセパレータを正規化します。
// ----------------------------------------------------------------------------
//!	windows用の場合に / を \ に直します。generic の場合は何もしません。
// ----------------------------------------------------------------------------
template<class PathTraits, class Object>
constexpr void normalize_directory_separator(Object& _path_string, typename std::enable_if< std::is_same<typename PathTraits::root_name::type, std::false_type>::value>::type*  = 0);





// ****************************************************************************
//
// パス情報の取得
//
// ****************************************************************************

// ============================================================================
//!	@brief	ルート名とディレクトリでパスを分離し、_destinate で指定するコンテナに辞書的な正規化を行って格納します。
// ----------------------------------------------------------------------------
//!	example) windows (normalize = none)
//!		z:/a/b/c/../d/ => [ "z:" "/" "a" "b" "c" ".." "d" "" ]
//!		この構成の場合、 std::filesystem::path::begin(), end() の場合は
//!		z:/a/b/c/../d/ => [ "z:" "/" "a" "b" "c" ".." "d" "." ]
//!		となり、最後の値が異なるので注意してください。これは、この関数が
//!		あくまでも元の文字列のビューを返しているからです。
//!
//!		/a/b/c/../d/ => [ "/" "a" "b" "c" ".." "d" "" ]
//!		a/b/c/..//d/e  => [ "a" "b" "c" ".." "" "d" "e" ]
//!
//!
//!	example) windows (normalize = lexically)
//!		/a/b/c/../d/    => [ "/" "a" "b" "d" ""  ]
//!		 a/b/c/..//d/e  => [     "a" "b" "d" "e" ]
//!		
// ----------------------------------------------------------------------------
//!	@param	_destinate
//!			_destinate.push_back({string_view.data(), string_view.size()}) が
//!			可能なコンテナ。
//!
//!	@param	_past_string
//!			パスを示す文字列。
//!
//!	@param	_normalize
//!			正規化するかどうか
// ----------------------------------------------------------------------------
template<class PathTraits, class PushBackContainor>
constexpr void path_split(PushBackContainor& _destinate, path_string_view<PathTraits> _path, path_split_normalize _normalize = path_split_normalize::none, path_split_conditions _conditions = path_split_conditions::all_required);

template<class PathTraits, class PushBackContainor>
constexpr void path_split(PushBackContainor& _destinate, path_string_view<PathTraits> _path, path_split_conditions _conditions);



// ============================================================================
//!	@brief	ルート名(a: や z:)を返します。
// ----------------------------------------------------------------------------
//!	名前が見つからない場合やgeneric なパスシステムの場合は {0, 0} の位置を
//!	返します。
// ----------------------------------------------------------------------------
template<class PathTraits>
constexpr auto root_name(path_string_view<PathTraits> _path, typename std::enable_if<!std::is_same<typename PathTraits::root_name::type, std::false_type>::value>::type*  = 0) -> path_string_view<PathTraits>;


// ============================================================================
//!	@brief	ルート名(a: や z:)を返します。
// ----------------------------------------------------------------------------
//!	名前が見つからない場合やgeneric なパスシステムの場合は {0, 0} の位置を
//!	返します。
// ----------------------------------------------------------------------------
template<class PathTraits>
constexpr auto root_name(path_string_view<PathTraits> _path, typename std::enable_if<std::is_same<typename PathTraits::root_name::type, std::false_type>::value>::type*  = 0) -> path_string_view<PathTraits>;


// ============================================================================
//!	@brief	ルートディレクトリを返します。
// ----------------------------------------------------------------------------
//!	下記のリストは実際に返される範囲を {} で示しています。
//!	example) windows
//!		{/}
//!		z:{/}
//!		z:{}
// ----------------------------------------------------------------------------
template<class PathTraits>
constexpr auto root_directory(path_string_view<PathTraits> _path) -> path_string_view<PathTraits>;


// ============================================================================
//!	@brief	ルート名とルートディレクトリを返します。
// ----------------------------------------------------------------------------
//!	下記のリストは実際に返される範囲を {} で示しています。
//!	example) windows
//!		{}a/b
//!		{z:}a/b
//!		{/}a/b
//!		{z:/}a/b
// ----------------------------------------------------------------------------
template<class PathTraits>
constexpr auto root_path(path_string_view<PathTraits> _path) -> path_string_view<PathTraits>;



// ============================================================================
//!	@brief	ルートパスからの相対位置を返します。
// ----------------------------------------------------------------------------
//!	下記のリストは実際に返される範囲を {} で示しています。
//!	example) windows
//!		{a/b}
//!		z:{a/b}
//!		/{a/b}
//!		z:/{a/b}
//!	path から root_path() を除いた範囲になります。
// ----------------------------------------------------------------------------
template<class PathTraits>
constexpr auto relative_path(path_string_view<PathTraits> _path) -> path_string_view<PathTraits>;


// ============================================================================
//!	@brief	親のパスを返します。
// ----------------------------------------------------------------------------
//!	下記のリストは実際に返される範囲を {} で示しています。
//!	example) windows _back_separator == false
//!		{a}/b
//!		{z:a}/b
//!		{/a}/b
//!		{z:/a}/b
//!	example) windows _back_separator == true
//!		{a/}b
//!		{z:a/}b
//!		{/a/}b
//!		{z:/a/}b
// ----------------------------------------------------------------------------
//!	@param	_back_separator
//!			true の時はセパレータ記号がある場合、セパレータ記号も含めて返します。
//!			例えば "/a/b/c/d/e" なら false = "/a/b/c/d"  true = "/a/b/c/d/"
// ----------------------------------------------------------------------------
template<class PathTraits>
constexpr auto parent_path(path_string_view<PathTraits> _path, bool _back_separator = false) -> path_string_view<PathTraits>;


// ============================================================================
//!	@brief	ルート名を除いたパスの範囲を返します。
// ----------------------------------------------------------------------------
//!	下記のリストは実際に返される範囲を {} で示しています。
//!	example) windows
//!		{a/b}
//!		z:{a/b}
//!		{/a/b}
//!		z:{/a/b}
// ----------------------------------------------------------------------------
template<class PathTraits>
constexpr auto remove_root_name(path_string_view<PathTraits> _path) -> path_string_view<PathTraits>;


// ============================================================================
//!	@brief	ファイル名を除いたパスの範囲を返します。
// ----------------------------------------------------------------------------
//!	下記のリストは実際に返される範囲を {} で示しています。
//!	example) windows
//!		{a/}b
//!		{z:a/}b
//!		{/a/}b
//!		{z:/a/}b
//!		{z:/a/b/v/}b.txt
//!	parent_path<PathTraits>(_path, true) と一致します。
// ----------------------------------------------------------------------------
template<class PathTraits>
constexpr auto remove_filename(path_string_view<PathTraits> _path) -> path_string_view<PathTraits>;


// ============================================================================
//!	@brief	拡張子を除いたパスの範囲を返します。
// ----------------------------------------------------------------------------
//!	下記のリストは実際に返される範囲を {} で示しています。
//!	example) windows
//!		{a/b}
//!		{z:a/b}.txt
//!		{z:a/b.c.d}.txt
//!		{z:a/b.c.d.}.txt
// ----------------------------------------------------------------------------
template<class PathTraits>
constexpr auto remove_extension(path_string_view<PathTraits> _path) -> path_string_view<PathTraits>;


// ============================================================================
//!	@brief	ファイル本体の名前と拡張子を返します。
// ----------------------------------------------------------------------------
//!	下記のリストは実際に返される範囲を {} で示しています。
//!	example) windows
//!		a/b/c/{.}
//!		a/b/c/{d.txt}
//!		z:a/b/c/{d.txt}
//!		z:/a/b/c/{d.txt}
//!	stem() + extension() です。
// ----------------------------------------------------------------------------
template<class PathTraits>
constexpr auto filename(path_string_view<PathTraits> _path) -> path_string_view<PathTraits>;


// ============================================================================
//!	@brief	ファイル本体の名前を返します。
// ----------------------------------------------------------------------------
//!	下記のリストは実際に返される範囲を {} で示しています。
//!	example) windows
//!		a/b/c/{.}
//!		a/b/c/{d}.txt
//!		z:a/b/c/{d}.txt
//!		z:/a/b/c/{d}.txt
//!		z:/a/b/c/{d.e.f}.txt
// ----------------------------------------------------------------------------
template<class PathTraits>
constexpr auto stem(path_string_view<PathTraits> _path) -> path_string_view<PathTraits>;



// ============================================================================
//!	@brief	ファイルの拡張子を返します。
// ----------------------------------------------------------------------------
//!	下記のリストは実際に返される範囲を {} で示しています。
//!	example) windows
//!		a/b/c/.{}
//!		z:/a/b/c/d{}
//!		a/b/c/d{.txt}
//!		z:a/b/c/d{.txt}
//!		z:/a/b/c/d.e.f{.txt}
// ----------------------------------------------------------------------------
template<class PathTraits>
constexpr auto extension(path_string_view<PathTraits> _path) -> path_string_view<PathTraits>;








template<class PathTraits>
constexpr int path_compare(const path_string_view<PathTraits>& _l, const path_string_view<PathTraits>& _r)
{
	auto	l		= _l.begin();
	auto	r		= _r.begin();
	while (l != _l.end() && r != _r.end())
	{
		const auto	diff	= PathTraits::compare(*l, *r);
		if (diff)
		{
			return	diff;
		}

		// 次の文字へ
		++l;
		++r;
	}
	return	(l == _l.end() && r == _r.end()) ? 0
			: (l != _l.end()) ? -1 : 1;
}


template<class PathTraits>
constexpr bool is_directory_filename(path_string_view<PathTraits> _path)
{
	return	(is_current_directory_filename<PathTraits>(_path) || is_parent_directory_filename<PathTraits>(_path))
			? true
			: false;
}


template<class PathTraits>
constexpr bool is_current_directory_filename(path_string_view<PathTraits> _path)
{
	return	(_path.size() == 1 && _path[0] == '.')
			? true
			: false;
}


template<class PathTraits>
constexpr bool is_parent_directory_filename(path_string_view<PathTraits> _path)
{
	return	(_path.size() == 2 && _path[0] == '.' && _path[1] == '.')
			? true
			: false;
}


template<class PathTraits>
constexpr bool has_root_path(path_string_view<PathTraits> _path)
{
	return	!root_path<PathTraits>(_path).empty();
}


template<class PathTraits>
constexpr bool has_root_name(path_string_view<PathTraits> _path)
{
	return	!root_name<PathTraits>(_path).empty();
}


template<class PathTraits>
constexpr bool has_root_directory(path_string_view<PathTraits> _path)
{
	return	!root_directory<PathTraits>(_path).empty();
}


template<class PathTraits>
constexpr bool has_relative_path(path_string_view<PathTraits> _path)
{
	return	!relative_path<PathTraits>(_path).empty();
}


template<class PathTraits>
constexpr bool has_parent_path(path_string_view<PathTraits> _path)
{
	return	!parent_path<PathTraits>(_path).empty();
}


template<class PathTraits>
constexpr bool has_filename(path_string_view<PathTraits> _path)
{
	return	!filename<PathTraits>(_path).empty();
}


template<class PathTraits>
constexpr bool has_stem(path_string_view<PathTraits> _path)
{
	return	!stem<PathTraits>(_path).empty();
}

template<class PathTraits>
constexpr bool has_extension(path_string_view<PathTraits> _path)
{
	return	!extension<PathTraits>(_path).empty();
}


template<class PathTraits, class Object>
constexpr void normalize_directory_separator(Object& _path_string, typename std::enable_if<!std::is_same<typename PathTraits::root_name::type, std::false_type>::value>::type*)
{
	constexpr auto	separator = PathTraits::separator;
	for (auto& c : _path_string)
	{
		if (PathTraits::is_separator(c) && (c != separator))
		{
			c = separator;
		}
	}
}


template<class PathTraits, class Object>
constexpr void normalize_directory_separator(Object& _path_string, typename std::enable_if< std::is_same<typename PathTraits::root_name::type, std::false_type>::value>::type*)
{
	;
}



template<class PathTraits, class PushBackContainor>
constexpr void path_split(PushBackContainor& _destinate, path_string_view<PathTraits> _path, path_split_normalize _normalize, path_split_conditions _conditions)
{
	using	size_type	= typename path_string_view<PathTraits>::size_type;
	auto	path		= _path;

	// ルート名
	auto	rootName	= root_name<PathTraits>(path);
	if (!rootName.empty())
	{
		if (_conditions == path_split_conditions::all_required)
		{
			_destinate.push_back({rootName.data(), rootName.size()});
		}
		path	= path.substr(rootName.size());
	}

	// ルートディレクトリ
	auto	rootDir	= root_directory<PathTraits>(path);
	if (!rootDir.empty())
	{
		// ルートディレクトリを出力
		if (_conditions == path_split_conditions::all_required || _conditions == path_split_conditions::root_name_exclusion)
		{
			_destinate.push_back({rootDir.data(), rootDir.size()});
		}
	}
	else if (path.empty())
	{
		// パスが無い
		return;
	}

	// ディレクトリ以降のパス
	while (!path.empty())
	{
		if (PathTraits::is_separator(path.front()))
		{
			path = path.substr(1);
		}
		auto 	it = path.begin();
		for (; it != path.end(); ++it)
		{
			if (PathTraits::is_separator(*it))
			{
				break;
			}
		}
		auto	sz	= static_cast<size_type>(it - path.begin());
		auto	p	= path.substr(0, sz);
		path	= path.substr(sz);
		if (_normalize == path_split_normalize::lexically)
		{
			if (is_current_directory_filename<PathTraits>(p))
			{
				continue;
			}
			else if (is_parent_directory_filename<PathTraits>(p))
			{
				_destinate.pop_back();
				continue;
			}
			else if (p.empty() && (it != path.end()))
			{
				continue;
			}
		}
		_destinate.push_back({p.data(), p.size()});
	}
}


template<class PathTraits, class PushBackContainor>
constexpr void path_split(PushBackContainor& _destinate, path_string_view<PathTraits> _path, path_split_conditions _conditions)
{
	path_split<PathTraits>(_destinate, _path, path_split_normalize::none, _conditions);
}


template<class PathTraits>
constexpr auto root_name(path_string_view<PathTraits> _path, typename std::enable_if<!std::is_same<typename PathTraits::root_name::type, std::false_type>::value>::type*) -> path_string_view<PathTraits>
{
	auto							pos	= _path.find_first_of(PathTraits::root_name::value);
	if (pos != path_string_view<PathTraits>::npos)
	{
		return	_path.substr(0, pos + 1);
	}
	return	_path.substr(0, 0);
}


template<class PathTraits>
constexpr auto root_name(path_string_view<PathTraits> _path, typename std::enable_if<std::is_same<typename PathTraits::root_name::type, std::false_type>::value>::type*) -> path_string_view<PathTraits>
{
	return	_path.substr(0, 0);
}


template<class PathTraits>
constexpr auto root_directory(path_string_view<PathTraits> _path) -> path_string_view<PathTraits>
{
	auto	path		= _path;
	auto	rootName	= root_name<PathTraits>(_path);
	if (!rootName.empty())
	{
		path	= path.substr(rootName.size());
	}
	if (!path.empty())
	{
		if (PathTraits::is_separator(path.front()))
		{
			return	path.substr(0, 1);
		}
	}
	return	_path.substr(0, 0);
}


template<class PathTraits>
constexpr auto root_path(path_string_view<PathTraits> _path) -> path_string_view<PathTraits>
{
	auto			path		= _path;
	auto			rootName	= root_name<PathTraits>(_path);
	decltype(path)	rootDir;
	if (!rootName.empty())
	{
		path	= path.substr(rootName.size());
	}
	if (!path.empty())
	{
		if (PathTraits::is_separator(path.front()))
		{
			rootDir	= path.substr(0, 1);
		}
	}
	return	_path.substr(0, rootName.size() + rootDir.size());
}


template<class PathTraits>
constexpr auto relative_path(path_string_view<PathTraits> _path) -> path_string_view<PathTraits>
{
	auto	rootPath	= root_path<PathTraits>(_path);
	return	_path.substr(rootPath.size());
}


template<class PathTraits>
constexpr auto parent_path(path_string_view<PathTraits> _path, bool _back_separator) -> path_string_view<PathTraits>
{
	using	size_type	= typename path_string_view<PathTraits>::size_type;

	auto	lastSep		= _path.begin();
	auto	rootName	= root_name<PathTraits>(_path);
	if (!rootName.empty())
	{
		lastSep	+= rootName.size();
	}
	for (auto it = lastSep; it != _path.end(); ++it)
	{
		if (PathTraits::is_separator(*it))
		{
			lastSep	= it;
			if (_back_separator && it != _path.end())
			{
				++lastSep;
			}
		}
	}
	return	_path.substr(0, static_cast<size_type>(lastSep - _path.begin()));
}


template<class PathTraits>
constexpr auto remove_root_name(path_string_view<PathTraits> _path) -> path_string_view<PathTraits>
{
	auto	rootName	= root_name<PathTraits>(_path);
	return	_path.substr(rootName.size());
}


template<class PathTraits>
constexpr auto remove_filename(path_string_view<PathTraits> _path) -> path_string_view<PathTraits>
{
	return	parent_path<PathTraits>(_path, true);
}


template<class PathTraits>
constexpr auto remove_extension(path_string_view<PathTraits> _path) -> path_string_view<PathTraits>
{
	using	size_type	= typename path_string_view<PathTraits>::size_type;
	auto	stm	= stem<PathTraits>(_path);
	return	_path.substr(0, static_cast<size_type>(stm.end() - _path.begin()));
}


template<class PathTraits>
constexpr auto filename(path_string_view<PathTraits> _path) -> path_string_view<PathTraits>
{
	return	_path.substr(parent_path<PathTraits>(_path, true).size());
}



template<class PathTraits>
constexpr auto stem(path_string_view<PathTraits> _path) -> path_string_view<PathTraits>
{
	auto	stm	= filename<PathTraits>(_path);
	if ((stm.size() > 1) && !is_parent_directory_filename<PathTraits>(stm))
	{
		auto	lastDot		= stm.begin();
		for (auto it = lastDot; it != stm.end(); ++it)
		{
			if (*it == '.')
			{
				lastDot	= it;
			}
		}
		stm	= stm.substr(0, lastDot - stm.begin());
	}
	return	stm;
}



template<class PathTraits>
constexpr auto extension(path_string_view<PathTraits> _path) -> path_string_view<PathTraits>
{
	using	size_type	= typename path_string_view<PathTraits>::size_type;
	auto	stm	= stem<PathTraits>(_path);
	return	_path.substr(static_cast<size_type>(stm.end() - _path.begin()));
}






AMTRS_NAMESPACE_FILESYSTEM_END
#endif

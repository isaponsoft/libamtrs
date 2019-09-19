/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__filesystem__basic_path__hpp
#define	__libamtrs__filesystem__basic_path__hpp
#include <cstring>
#include <string>
#include <string_view>
#include "types.hpp"
#include "../string.hpp"
#include "path_util.hpp"

#include "path.inc/impl.hpp"

AMTRS_NAMESPACE_FILESYSTEM_BEGIN

// ==================================================================
//!	@brief	ファイルのパスを定義します。
//!			基本的には std::string のラッパーとなっていますが、
//!			一部ファイルパスを扱うのに便利な機能が追加されています。
//!
//!			ある程度は std::filesystem::path に寄せてありますので
//!			使い方は
//!				http://en.cppreference.com/w/cpp/filesystem/path
//!			を参考にしてください。			
// ------------------------------------------------------------------
template<class PathTraits>
class	basic_path
{
public:
	using	this_type		= basic_path<PathTraits>;
	using	value_type		= typename PathTraits::value_type;
	using	string_type		= std::basic_string<value_type>;
	class	iterator;
	using	const_iterator	= iterator;
	using	path_traits		= PathTraits;

	static constexpr value_type	separator = PathTraits::separator;

	enum format
	{
		native_format,
		generic_format,
		auto_format
	};
private:
	string_type		m_string;

public:

	basic_path() noexcept = default;
	basic_path(const basic_path& ) = default;
	basic_path(      basic_path&&) noexcept = default;

	basic_path(string_type&& _source, format _fmt = auto_format);

	template<class Source>
	basic_path(const Source& _source, format _fmt = auto_format);

	template<class InputIt>
	basic_path(InputIt _first, InputIt _last, format _fmt = auto_format);

	template<class Source>
	basic_path(const Source& _source, const std::locale& _locale, format _fmt = auto_format);

	template<class InputIt>
	basic_path(InputIt _first, InputIt _last, const std::locale& _locale, format _fmt = auto_format);

#ifdef	AMTRS_STL17_FILESYSTEM
	// STL からのコンバート
	basic_path(const std::filesystem::path& _std_path) : basic_path(_std_path.native()) {}
#endif

	~basic_path() = default;
	basic_path& operator = (const basic_path& ) = default;
	basic_path& operator = (      basic_path&&) = default;

	                        basic_path& operator = (string_type&& _r) { m_string = std::move(_r); return *this; }
	template<class Source>  basic_path& operator = (const Source& _r);

	                        basic_path& assign(string_type&& _source);
	template<class Source>  basic_path& assign(const Source& _source);
	template<class InputIt> basic_path& assign(InputIt _first, InputIt _last);


	                        basic_path& operator /= (const basic_path& _path);
	template<class Source>  basic_path& operator /= (const Source& _path);
	template<class Source>  basic_path& append(const Source& _path);
	template<class InputIt> basic_path& append(InputIt _first, InputIt _last);



	operator const string_type& () const noexcept { return m_string; }
	const string_type& native() const noexcept { return m_string; }



	// ==============================================================
	//!	@brief	ルートのディレクトリを取得します。
	//!			http://en.cppreference.com/w/cpp/filesystem/path/root_directory
	//!			このメソッドを呼び出すたびにインスタンスを生成します。
	// --------------------------------------------------------------
	auto root_directory() const -> this_type { auto v = filesystem::root_directory<path_traits>({m_string.data(), m_string.size()}); return { string_type(v.begin(), v.end()) }; }

	// ==============================================================
	//!	@brief	ルートの名前を取得します。
	//!			http://en.cppreference.com/w/cpp/filesystem/path/root_name
	//!			このメソッドを呼び出すたびにインスタンスを生成します。
	//!			Generic形式のシステムでは path() になります。
	// --------------------------------------------------------------
	auto root_name() const -> this_type { auto v = filesystem::root_name<path_traits>({m_string.data(), m_string.size()}); return { string_type(v.begin(), v.end()) }; }


	// ==============================================================
	//!	@brief	ルートのパスを取得します。
	//!			http://en.cppreference.com/w/cpp/filesystem/path/root_path
	//!			このメソッドを呼び出すたびにインスタンスを生成します。
	// --------------------------------------------------------------
	auto root_path() const -> this_type { auto v = filesystem::root_path<path_traits>({m_string.data(), m_string.size()}); return { string_type(v.begin(), v.end()) }; }

	// ==============================================================
	//!	@brief	親となるパスを抜き出します。
	//!			http://en.cppreference.com/w/cpp/filesystem/path/relative_path
	//!			このメソッドを呼び出すたびにインスタンスを生成します。
	// --------------------------------------------------------------
	auto relative_path() const -> this_type { auto v = filesystem::relative_path<path_traits>({m_string.data(), m_string.size()}); return { string_type(v.begin(), v.end()) }; }

	// ==============================================================
	//!	@brief	親となるパスを抜き出します。
	//!			http://en.cppreference.com/w/cpp/filesystem/path/parent_path
	//!			このメソッドを呼び出すたびにインスタンスを生成します。
	// --------------------------------------------------------------
	auto parent_path() const -> this_type { auto v = filesystem::parent_path<path_traits>({m_string.data(), m_string.size()}); return { string_type(v.begin(), v.end()) }; }

	// ==============================================================
	//!	@brief	ファイル名を抜き出します。拡張子を含みます。
	//!			http://en.cppreference.com/w/cpp/filesystem/path/filename
	//!			このメソッドを呼び出すたびにインスタンスを生成します。
	// --------------------------------------------------------------
	auto filename() const -> this_type;

	// ==============================================================
	//!	@brief	一般的に basename() とも呼ばれるファイル名から拡張子を取り除いたもの。
	//!			http://en.cppreference.com/w/cpp/experimental/fs/path/stem
	//!			このメソッドを呼び出すたびにインスタンスを生成します。
	// --------------------------------------------------------------
	auto stem() const -> this_type { auto v = filesystem::stem<path_traits>({m_string.data(), m_string.size()}); return { string_type(v.begin(), v.end()) }; }

	// ==============================================================
	//!	@brief	拡張子を抜き出します。
	//!			http://en.cppreference.com/w/cpp/filesystem/path/extension
	//!			このメソッドを呼び出すたびにインスタンスを生成します。
	// --------------------------------------------------------------
	auto extension() const -> this_type { auto v = filesystem::extension<path_traits>({m_string.data(), m_string.size()}); return { string_type(v.begin(), v.end()) }; }


	bool operator == (const this_type& _r) const { return m_string == _r.m_string; }
	bool operator != (const this_type& _r) const { return m_string != _r.m_string; }
	bool operator <  (const this_type& _r) const { return m_string <  _r.m_string; }
	bool operator <= (const this_type& _r) const { return m_string <= _r.m_string; }
	bool operator >  (const this_type& _r) const { return m_string >  _r.m_string; }
	bool operator >= (const this_type& _r) const { return m_string >= _r.m_string; }


	// ****************************************************
	// Format observers
	// ----------------------------------------------------
	const value_type* c_str() const noexcept { return m_string.c_str(); }
	void clear() noexcept { m_string.clear(); }

	template<class RCharT, class RTraits = std::char_traits<RCharT>, class RAlloc = std::allocator<RCharT>>
	std::basic_string<RCharT, RTraits, RAlloc> string(const RAlloc& _alloc = RAlloc()) const;
	auto string()  const -> decltype(str::to_string(m_string, std::locale())) { return str::to_string(m_string, std::locale("")); }
	auto wstring() const -> decltype(str::to_wstring(m_string, std::locale())) { return str::to_wstring(m_string, std::locale("")); }
#if 0
	// MEMO: 未実装
	std::string u8string() const;
	std::u16string u16string() const;
	std::u32string u32string() const;

	template<class RCharT, class RTraits = std::char_traits<RCharT>, class RAlloc = std::allocator<RCharT>>
	std::basic_string<RCharT, RTraits, RAlloc> generic_string(const RAlloc& a = RAlloc() ) const;
	std::string generic_string() const;
	std::wstring generic_wstring() const;
	std::string generic_u8string() const;
	std::u16string generic_u16string() const;
	std::u32string generic_u32string() const;
#endif


	// ****************************************************
	// Compare
	// ----------------------------------------------------
	int compare(const basic_path& _path) const noexcept { return path_compare<path_traits>({m_string.data(), m_string.size()}, {_path.m_string.data(), _path.m_string.size()}); }
	int compare(const string_type& _s) const { return compare(basic_path(_s)); }
	int compare(std::basic_string_view<value_type> _s) const { return compare(basic_path(_s)); }
	int compare(const value_type* _s) const { return compare(basic_path(_s)); }


	// ****************************************************
	// Queries
	// ----------------------------------------------------
	amtrs_constexpr14 bool empty() const noexcept { return m_string.empty(); }
	amtrs_constexpr14 bool has_root_path     () const { return filesystem::has_root_path     <path_traits>({m_string.data(), m_string.size()}); }
	amtrs_constexpr14 bool has_root_name     () const { return filesystem::has_root_name     <path_traits>({m_string.data(), m_string.size()}); }
	amtrs_constexpr14 bool has_root_directory() const { return filesystem::has_root_directory<path_traits>({m_string.data(), m_string.size()}); }
	amtrs_constexpr14 bool has_relative_path () const { return filesystem::has_relative_path <path_traits>({m_string.data(), m_string.size()}); }
	amtrs_constexpr14 bool has_parent_path   () const { return filesystem::has_parent_path   <path_traits>({m_string.data(), m_string.size()}); }
	amtrs_constexpr14 bool has_filename      () const { return filesystem::has_filename      <path_traits>({m_string.data(), m_string.size()}); }
	amtrs_constexpr14 bool has_stem          () const { return filesystem::has_stem          <path_traits>({m_string.data(), m_string.size()}); }
	amtrs_constexpr14 bool has_extension     () const { return filesystem::has_extension     <path_traits>({m_string.data(), m_string.size()}); }


	iterator begin() const noexcept;
	iterator end() const noexcept;

private:
	void separator_normalize() { normalize_directory_separator<path_traits>(m_string); }
};


template<class PathTraits>
basic_path<PathTraits> operator / (const basic_path<PathTraits>& _lhs, const basic_path<PathTraits>& _rhs)
{
	basic_path<PathTraits>	retval(_lhs);
	retval /= _rhs;
	return	retval;
}



template<class PathTraits>
basic_path<PathTraits>::basic_path(string_type&& _source, format _fmt)
	: m_string(_source)
{
	separator_normalize();
}


template<class PathTraits>
template<class Source>
basic_path<PathTraits>::basic_path(const Source& _source, format)
	: m_string(str::convert_to<string_type::value_type>(_source, std::locale("")))
{
	separator_normalize();
}


template<class PathTraits>
template<class InputIt>
basic_path<PathTraits>::basic_path(InputIt _first, InputIt _last, format)
	: m_string(path_impl::convert<string_type>::cv(std::locale(""), _first, _last))
{
	separator_normalize();
}


template<class PathTraits>
template<class Source>
basic_path<PathTraits>::basic_path(const Source& _source, const std::locale& _locale, format)
	: m_string(path_impl::convert<string_type>::cv(_locale, _source))
{
	separator_normalize();
}


template<class PathTraits>
template<class InputIt>
basic_path<PathTraits>::basic_path(InputIt _first, InputIt _last, const std::locale& _locale, format)
	: m_string(path_impl::convert<string_type>::cv(_locale, _first, _last))
{
	separator_normalize();
}


template<class PathTraits>
template<class Source>
auto basic_path<PathTraits>::operator = (const Source& _r) -> basic_path& 
{
	m_string	= path_impl::convert<string_type>::cv(std::locale(""), _r);
	separator_normalize();
	return	*this;
}


template<class PathTraits>
auto basic_path<PathTraits>::operator /= (const basic_path& _path) -> basic_path&
{
	return	append(std::forward<const basic_path>(_path));
}


template<class PathTraits>
template<class Source>
auto basic_path<PathTraits>::operator /= (const Source& _path) -> basic_path&
{
	return	append(std::forward<const Source>(_path));
}


template<class PathTraits>
template<class Source>
auto basic_path<PathTraits>::append(const Source& _path) -> basic_path&
{
	auto	r	= path_impl::convert<string_type>::cv(std::locale(""), _path);
	if (!m_string.empty() && !r.empty())
	{
		m_string	= m_string + separator + r;
		separator_normalize();
	}
	else if (m_string.empty())
	{
		m_string	= std::move(r);
	}
	return	*this;
}


template<class PathTraits>
template<class InputIt>
auto basic_path<PathTraits>::append(InputIt _first, InputIt _last) -> basic_path&
{
	auto	r	= path_impl::convert<string_type>::cv(std::locale(""), _first, _last);
	if (!m_string.empty() && !r.empty())
	{
		m_string	= m_string + separator + r;
		separator_normalize();
	}
	else if (m_string.empty())
	{
		m_string	= std::move(r);
	}
	return	*this;
}


template<class PathTraits>
auto basic_path<PathTraits>::assign(string_type&& _source) -> basic_path&
{
	m_string	= path_impl::convert<string_type>::cv(std::locale(""), _source);
	separator_normalize();
	return	*this;
}


template<class PathTraits>
template<class Source>
auto basic_path<PathTraits>::assign(const Source& _source) -> basic_path&
{
	m_string	= path_impl::convert<string_type>::cv(std::locale(""), _source);
	separator_normalize();
	return	*this;
}


template<class PathTraits>
template<class InputIt>
auto basic_path<PathTraits>::assign(InputIt _first, InputIt _last) -> basic_path&
{
	m_string	= path_impl::convert<string_type>::cv(std::locale(""), _first, _last);
	separator_normalize();
	return	*this;
}


template<class PathTraits>
template<class RCharT, class RTraits, class RAlloc>
std::basic_string<RCharT, RTraits, RAlloc> basic_path<PathTraits>::string(const RAlloc& _alloc) const
{
	return	path_impl::convert<std::basic_string<RCharT, RTraits, RAlloc>>::cv(std::locale(""), m_string.begin(), m_string.end());
}


template<class PathTraits>
auto basic_path<PathTraits>::filename() const -> this_type
{
	static constexpr value_type	root[]	= { separator, 0 };
	static constexpr value_type	dot[]	= { static_cast<value_type>('.'), 0 };

	auto	path	= filesystem::remove_root_name<PathTraits>({m_string.data(), m_string.size()});
	if (path.empty())
	{
		// "" or "c:"
		return	{};
	}
	if (path.size() == 1 && PathTraits::is_separator(path.front()))
	{
		return	this_type { static_cast<const value_type*>(root) };
	}
	auto	fname	= filesystem::filename<PathTraits>(path);
	return	fname.empty() ? this_type{ static_cast<const value_type*>(dot) } : this_type{ string_type(fname.begin(), fname.end()) };
}


AMTRS_NAMESPACE_FILESYSTEM_END
namespace std {


template<class Traits, class PathTraits>
basic_ostream<char, Traits>& operator << (basic_ostream<char, Traits>& _os, AMTRS_NAMESPACE_FILESYSTEM_FULL::basic_path<PathTraits> _sv)
{
	auto	str = _sv.string();
	return	_os << str;
}

template<class Traits, class PathTraits>
basic_ostream<wchar_t, Traits>& operator << (basic_ostream<wchar_t, Traits>& _os, AMTRS_NAMESPACE_FILESYSTEM_FULL::basic_path<PathTraits> _sv)
{
	auto	str = _sv.wstring();
	return	_os << str;
}


}
#endif

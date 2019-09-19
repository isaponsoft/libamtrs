/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__filesystem__stdfs__path__hpp
#define	__libamtrs__filesystem__stdfs__path__hpp
#include <string>
#include "file_status.hpp"
#include "types.hpp"
AMTRS_FILESYSTEM_STDFS_NAMESPACE_BEGIN
AMTRS_IMPLEMENTS_BEGIN(path)
template<class CharT>
struct	path_traits;				// Prototype. not use.

template<class CharT, class TraitsT = path_traits<CharT>>
class	basic_path;					// Prototype. not use.
AMTRS_IMPLEMENTS_END(path)


//! std::filesystem が存在しないときの代理ライブラリ
//! 機能不足なので、コンパイルを通すのと最低限の動きをするためのものと割り切ってください。
using	path	= AMTRS_IMPLEMENTS(path)::basic_path<char>;



AMTRS_IMPLEMENTS_BEGIN(path)

template<class CharT>
struct	path_traits : std::char_traits<CharT>
{
private:
	using	char_traits_type	= std::char_traits<CharT>;
public:
	using	char_type			= typename char_traits_type::char_type;
	using	int_type			= typename char_traits_type::int_type;

	
	static constexpr int_type directory_separator() noexcept { return '/'; }
};

// std::filesystem::path compatible base.
template<class CharT, class TraitsT>
class	basic_path
{
	using	_path_traits_type	= TraitsT;
public:
	using	value_type			= CharT;
	using	size_type			= std::size_t;
	using	string_type			= std::basic_string<CharT>;

	enum	format
	{
		native_format,
		generic_format,
		auto_format
	};

	basic_path() = default;
	basic_path(const basic_path&) = default;
	basic_path(basic_path&&) = default;
	basic_path& operator = (const basic_path&) = default;
	basic_path& operator = (basic_path&&) = default;

	template<class CharTraitsT>
	basic_path(const std::basic_string<CharT, CharTraitsT>& _s) : mPathString(_s) {}
	template<class CharTraitsT>
	basic_path(std::basic_string<CharT, CharTraitsT>&& _s) : mPathString(_s) {}

	template<class CharTraitsT>
	basic_path(const std::basic_string_view<CharT, CharTraitsT>& _s) : mPathString(_s) {}
	template<class CharTraitsT>
	basic_path(std::basic_string_view<CharT, CharTraitsT>&& _s) : mPathString(_s) {}

	template<std::size_t N>
	basic_path(const value_type (&_s)[N]) : mPathString(_s) {}


	basic_path(const value_type* _s) : mPathString(_s) {}

/*
	template<class... Args>
	basic_path(Args&&... _args)
		: mPathString(std::forward<Args>(_args)...) {}
*/
	operator string_type () const { return mPathString; }
	const string_type& native() const noexcept { return mPathString; }
	const value_type* c_str() const noexcept { return native().c_str(); }


	const string_type& string() const noexcept { return mPathString; }

	void clear() noexcept { mPathString.clear(); }

	bool empty() const noexcept { return mPathString.empty(); }

	basic_path operator / (const basic_path& _r) const
	{
		return	basic_path(native() + static_cast<value_type>(_path_traits_type::directory_separator()) + _r.native());
	}

	basic_path& operator /= (const basic_path& _r)
	{
		*this	= *this / _r;
		return	*this;
	}

	basic_path extension() const
	{
		basic_path	retval;
		auto		pos	= mPathString.find_last_of('.');
		if ((pos != string_type::npos) && (pos > 0) && (mPathString[pos - 1] != '/'))
		{
			retval	= mPathString.substr(pos);
		}
		return	retval;
	}

	basic_path filename() const
	{
		basic_path	retval	= stem();
		auto		sz		= retval.extension().string().size();
		if (!retval.mPathString.empty())
		{
			retval.mPathString	= retval.mPathString.substr(0, retval.mPathString.size()-sz);
		}
		return	retval;
	}

	basic_path stem() const
	{
		basic_path	retval;
		auto		pos	= mPathString.find_last_of('/');
		if (pos != string_type::npos)
		{
			retval	= mPathString.substr(pos + 1);
		}
		return	retval;
	}

	basic_path parent_path() const
	{
		basic_path	retval;
		auto		pos	= mPathString.find_last_of('/');
		if (pos != string_type::npos)
		{
			retval	= mPathString.substr(0, pos);
		}
		return	retval;
	}


protected:
	string_type		mPathString;
};

AMTRS_IMPLEMENTS_END(path)
AMTRS_FILESYSTEM_STDFS_NAMESPACE_END
#endif

/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__filesystem__path__path_inc__impl__hpp
#define	__libamtrs__filesystem__path__path_inc__impl__hpp
AMTRS_NAMESPACE_FILESYSTEM_BEGIN namespace path_impl {



template<class T>
struct	convert
{
	static const std::string& cv(const std::locale& _locale, const std::string& _string)
	{
		return	_string;
	}

	static std::string cv(const std::locale& _locale, const char* _string)
	{
		return	_string;
	}

	static std::string cv(const std::locale& _locale, const std::wstring& _string)
	{
		return	str::to_string(_string, _locale);
	}

	static std::string cv(const std::locale& _locale, const wchar_t* _string)
	{
		return	str::to_string(std::basic_string_view<wchar_t>{_string}, _locale);
	}

	template<int N>
	static std::string cv(const std::locale& _locale, char (&_string)[N])
	{
		return	{ _string, _string + N };
	}

	template<int N>
	static std::string cv(const std::locale& _locale, const char (&_string)[N])
	{
		return	{ _string, _string + N };
	}

	static std::string cv(const std::locale& _locale, const char* _begin, const char* _end)
	{
		return	{ _begin, _end };
	}

	static std::string cv(const std::locale& _locale, std::string::const_iterator _begin, std::string::const_iterator _end)
	{
		return	{ _begin, _end };
	}

	template<class It>
	static std::string cv(const std::locale& _locale, It _begin, It _end)
	{
		return	str::to_string(_begin, _end, _locale);
	}
};

template<>
struct	convert<std::wstring>
{
	static const std::wstring& cv(const std::locale&, const std::wstring& _string)
	{
		return	_string;
	}

	static std::wstring cv(const std::locale& _locale, const char* _string)
	{
		return	str::to_wstring(std::string_view{_string}, _locale);
	}

	template<int N>
	static std::wstring cv(const std::locale& _locale, char (&_string)[N])
	{
		return	str::to_wstring(std::string_view{_string}, _locale);
	}

	template<int N>
	static std::wstring cv(const std::locale& _locale, const char (&_string)[N])
	{
		return	str::to_wstring(std::string_view{_string}, _locale);
	}

	static std::wstring cv(const std::locale&, const wchar_t* _string)
	{
		return	{_string};
	}

	template<int N>
	static std::wstring cv(const std::locale&, wchar_t (&_string)[N])
	{
		return	{_string, _string + N};
	}

	template<int N>
	static std::wstring cv(const std::locale&, const wchar_t (&_string)[N])
	{
		return	{_string, _string + N};
	}

	template<class It>
	static std::wstring cv(const std::locale& _locale, It _begin, It _end)
	{
		return	str::to_wstring(_begin, _end, _locale);
	}
};



} AMTRS_NAMESPACE_FILESYSTEM_END
#endif
/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__logging__logging__hpp
#define	__libamtrs__logging__logging__hpp
#include <functional>
#include <string>
#include <string_view>
#include <vector>
AMTRS_LOGGING_NAMESPACE_BEGIN

struct	location
{
	std::string_view	filename;
	long				line;
};


struct	config
{
	using size_type	= std::size_t;

	static constexpr size_type	nosize	= static_cast<size_type>(-1);

	// ==============================================================
	//! プラットフォームごとに異なるログの最大値。
	// --------------------------------------------------------------
	//! サイズの制限が存在しない場合は nosize を返す。
	//! 同じプラットフォームでもデバッガに接続している場合とそうでない
	//! 場合でサイズが変わる場合がある。
	// --------------------------------------------------------------
	static size_type max_size() noexcept;

	// ==============================================================
	//! プラットフォームごとに異なるログの出力本体。
	// --------------------------------------------------------------
	static void output(const char* _msg, size_type _size);


	static inline std::function<std::string()>	prefix;


};

AMTRS_LOGGING_NAMESPACE_END
#include AMTRS_MAKE_INCLUDE_PLATFORM(logging)
AMTRS_LOGGING_NAMESPACE_BEGIN
AMTRS_IMPLEMENTS_BEGIN(logging)

inline const char* get_value(const char* _value) { return _value; }
inline int get_value(int _value) { return _value; }
inline double get_value(float _value) { return _value; }
inline double get_value(double _value) { return _value; }
inline void* get_value(void* _value) { return _value; }

template<class T>
const T& get_value(const T& _value) { return _value; }

template<class Traits, class Allocator> const char* get_value(const std::basic_string<char, Traits, Allocator>& _value) { return _value.c_str(); }
template<class T> void* get_value(const ref<T>& _value) { return _value.get(); }

template<class T>
struct	get_printf_args
{
	static auto get(const T& _value) { return get_value(_value); }
};

AMTRS_IMPLEMENTS_END(logging)

inline void prefix(std::function<std::string()> _prefix)
{
	config::prefix	= _prefix;
}


template<class... Args>
inline void output(const char* _format, Args&&... _args)
{
	using namespace AMTRS_IMPLEMENTS(logging);
	auto	maxsize	= config::max_size();
	if (maxsize == config::nosize)
	{
		maxsize	= 8192;
	}
	// clang = -Wno-error=format-security
	std::vector<char>	buff(maxsize);
	auto				size	= snprintf(buff.data(), buff.size(), _format, get_printf_args<Args>::get(_args)...);
	config::output(buff.data(), size);
}


inline void output(location _locate, const std::string& _msg)
{
	using namespace AMTRS_IMPLEMENTS(logging);
	auto	maxsize	= config::max_size();
	if (maxsize == config::nosize)
	{
		maxsize	= 8192;
	}

	auto		pos		= _locate.filename.find_last_of("/\\");
	const char*	name	= _locate.filename.data();
	if (pos != std::string_view::npos)
	{
		name += pos + 1;
	}

	std::vector<char>	buff1;	buff1.resize(maxsize);
	std::vector<char>	buff2;	buff2.resize(maxsize);
	std::vector<char>	buff3;	buff3.resize(maxsize);
								  snprintf(buff1.data(), buff1.size(), "%s(%d)", name, (int)_locate.line);
								// clang = -Wno-error=format-security
								  snprintf(buff2.data(), buff2.size(), "%s", _msg.c_str());
	auto				size3	= snprintf(buff3.data(), buff3.size(), "%s : %s", buff1.data(), buff2.data());
	config::output(buff3.data(), size3);
}




AMTRS_LOGGING_NAMESPACE_END
#endif

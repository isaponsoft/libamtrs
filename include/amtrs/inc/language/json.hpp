/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__language__json__hpp
#define	__libamtrs__language__json__hpp
#include <map>
#include <string>
#include <string_view>
#include <vector>
#include "../../amtrs.hpp"
#include "../../logging.hpp"
#include "../../variant.hpp"
AMTRS_NAMESPACE_BEGIN

template<class CharT>
struct	json_traits
{
	using	char_type	= CharT;

	constexpr bool is_space(char_type c) const noexcept { return c == ' ' || c == '\t' || c == '\n' || c == '\r'; }

	constexpr bool is_return(char_type c) const noexcept { return c == '\n' || c == '\r'; }

	constexpr bool is_string_begin(char_type c) const noexcept
	{
		return	(c == '\'' || c == '"') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_');
	}

	constexpr bool is_string_end(char_type first, char_type c) const noexcept
	{
		if ((first == '\'' || first == '"') && (first == c))
		{
			return	true;
		}
	}

	constexpr bool is_assoc_separator(char_type c) const noexcept
	{
		return	c == ':';
	}

	constexpr bool is_number(char_type c) const noexcept
	{
		return	(c >= '0' && c <= '9');
	}

	constexpr bool is_keyword_first(char_type c) const noexcept
	{
		return	(c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_');
	}

	constexpr bool is_keyword_second(char_type c) const noexcept
	{
		return	(c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || (c == '_');
	}

private:
	constexpr bool is_assoc_name(char_type c) const noexcept
	{
		return	(c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_');
	}
};


template<class VarT, class CharT, class Traits = json_traits<CharT>>
class	basic_json_parser;


using	json_parser	= basic_json_parser<variantobj<bool, std::nullptr_t, std::intmax_t, double, std::string>, char, json_traits<char>>;




AMTRS_IMPLEMENTS_BEGIN(json_parser)
// rfc8259
template<class Iterator, class VarT, class CharT, class Traits>
class	parser
		: private Traits
{
public:
	using	char_type	= CharT;
	using	var_type	= VarT;
	using	traits_type	= Traits;
	using	char_traits	= std::char_traits<char_type>;
	using	iterator	= Iterator;
	using	this_type	= parser<Iterator, VarT, CharT, Traits>;

	using	boolean_type	= bool;
	using	integer_type	= std::intmax_t;
	using	real_type		= double;
	using	string_type		= std::string;
	using	array_type		= typename var_type::array_type;
	using	object_type		= typename var_type::assoc_type;

	enum error_code
	{
		unkown_char,
		unclosed_string,
		unkown_terminate,
	};

	
	static var_type parse(iterator _begin, iterator _end)
	{
		this_type	thiz(_begin, _end);
		return		thiz.go();
	}

private:
	parser(iterator _begin, iterator _end)
		: mCursor(_begin)
		, mEnd(_end)
	{}


	var_type go()
	{
		return	go_value();
	}

	var_type go_value()
	{
		var_type	retval;
		skip();
		if (!finished())
		{
			char_type	c	= *mCursor++;
			if (c == '[')
			{
				retval	= go_array();
			}
			else if (c == '{')
			{
				retval	= go_object();
			}
			else if (c == '-' || c == '+' || traits_type::is_number(c))
			{
				retval	= go_number(c);
			}
			else if (c == '\"')
			{
				retval	= go_string();
			}
			else if (c == 't' || c == 'f' || c == 'n')
			{
				retval	= go_literal_names(c);
			}
			else
			{
				--mCursor;
				throw_errror(unkown_char, format<std::string>("unkown char '%c'", c));
			}
		}
		return	retval;
	}

	// false = %x66.61.6c.73.65   ; false
	// null  = %x6e.75.6c.6c      ; null
	// true  = %x74.72.75.65      ; true
	var_type go_literal_names(char_type _c)
	{
		if (_c == 't')
		{
			read_literal_names("rue");
			return	var_type((bool)true);
		}
		if (_c == 'f')
		{
			read_literal_names("alse");
			return	var_type((bool)false);
		}
		read_literal_names("ull");
		return	var_type((std::nullptr_t)nullptr);
	}

	void read_literal_names(std::string_view _text)
	{
		for (auto& c : _text)
		{
			if (finished())
			{
				throw_errror(unkown_terminate);
			}
			if (c != *mCursor)
			{
				throw_errror(unkown_char, format<std::string>("unkown char '%c'", *mCursor));
			}
			++mCursor;
		}
	}


	// number = [ minus ] int [ frac ] [ exp ]
	var_type go_number(char_type _c)
	{
		integer_type	v		= 0;
		bool			minus	= _c == '-';
		if (_c == '+' || _c == '-')
		{
			skip();
			if (finished())
			{
				throw_errror(unkown_terminate);
			}
			_c	= *mCursor++;
			if (!traits_type::is_number(_c))
			{
				throw_errror(unkown_char, format<std::string>("unkown char '%c'", _c));
			}
		}

		for (;;)
		{
			v	= (v * 10) + (_c - '0');
			if (finished())
			{
				break;
			}
			_c	= *mCursor;
			if (_c == '.')
			{
				++mCursor;
				return	go_number_with_real_frac(static_cast<real_type>(v), minus);
			}

			if (!traits_type::is_number(_c))
			{
				break;
			}
			++mCursor;
		}
		return	var_type(minus ? -v : v);
	}

	// go_number() から呼ばれる
	// [ frac ] [ exp ]
	var_type go_number_with_real_frac(real_type _v, bool _minus)
	{
		real_type		v		= _v;
		real_type		frac	= 0;

		// 最低でも１桁が必要
		if (finished())
		{
			throw_errror(unkown_terminate);
		}
		if (!traits_type::is_number(*mCursor))
		{
			throw_errror(unkown_char, format<std::string>("unkown char '%c'", *mCursor));
		}

		auto		_c	= *mCursor++;
		real_type	per	= 0.1f;
		for (;;)
		{
			frac	=  frac + per * (_c - '0');
			per		*= 0.1f;
			if (finished())
			{
				break;
			}
			_c	= *mCursor;
			if (!traits_type::is_number(_c))
			{
				break;
			}
			++mCursor;
		}
		v	+= frac;
		return	var_type(_minus ? -v : v);
	}

	var_type go_array()
	{
		array_type	retval;

		skip();
		if (finished())
		{
			throw_errror(unkown_terminate);
		}
		if (*mCursor == ']')
		{
			++mCursor;
		}
		else
		{
			for (;;)
			{

				retval.push_back(go_value());
				skip();

				if (finished())
				{
					throw_errror(unkown_terminate);
				}

				if (*mCursor == ']')
				{
					++mCursor;
					break;
				}

				if (*mCursor != ',')
				{
					throw_errror(unkown_char, format<std::string>("unkown char '%c'", *mCursor));
				}
				++mCursor;
			}
		}
		return	retval;
	}

	var_type go_object()
	{
		object_type	retval;
		skip();
		if (finished())
		{
			throw_errror(unkown_terminate);
		}
		if (*mCursor == '}')
		{
			++mCursor;
		}
		else
		{
			for (;;)
			{
				auto	key	= go_key();
				skip();
				if (finished())
				{
					throw_errror(unkown_terminate);
				}
				if (*mCursor != ':')
				{
					throw_errror(unkown_char, format<std::string>("unkown char '%c'", *mCursor));
				}
				++mCursor;

				auto	val	= go_value();
				retval.emplace(typename object_type::value_type{std::move(key), std::move(val)});
				skip();

				if (finished())
				{
					throw_errror(unkown_terminate);
				}

				if (*mCursor == '}')
				{
					++mCursor;
					break;
				}

				if (*mCursor != ',')
				{
					throw_errror(unkown_char, format<std::string>("unkown char '%c'", *mCursor));
				}
				++mCursor;
			}
		}
		return	retval;
	}

	std::string go_key()
	{
		skip();
		if (finished())
		{
			throw_errror(unclosed_string);
		}

		if (*mCursor == '\"')
		{
			++mCursor;
			return	std::get<std::string>(go_string());
		}

		if (!traits_type::is_keyword_first(*mCursor))
		{
			throw_errror(unkown_char, format<std::string>("unkown char '%c'", *mCursor));
		}

		std::string	retval;
		retval.push_back(*mCursor++);
		for (;;)
		{
			if (finished())
			{
				break;
			}
			char_type	c	= *mCursor;
			if (!traits_type::is_keyword_second(c))
			{
				break;
			}
			retval.push_back(c);
			++mCursor;
		}
		return	retval;
	}

	var_type go_string()
	{
		string_type	tmp;
		for (;;)
		{
			if (finished())
			{
				throw_errror(unclosed_string);
			}
			char_type	c	= *mCursor++;
			if (c == '\"')
			{
				break;
			}
			tmp.push_back(c);
		}
		return	var_type(tmp);
	}


	[[noreturn]]
	void throw_errror(error_code _ec, const std::string& _msg = "")
	{
		throw	std::runtime_error(std::string("json_decode faulure. type is ") + _msg);
	}

	void skip()
	{
		while ((mCursor != mEnd) && traits_type::is_space(*mCursor))
		{
			++mCursor;
		}
	}

	bool finished()
	{
		return	mCursor == mEnd;
	}

	iterator		mCursor;
	iterator		mEnd;
};
AMTRS_IMPLEMENTS_END(json_parser)


template<class VarT, class CharT, class Traits>
class	basic_json_parser
{
public:
	using	char_type	= CharT;
	using	var_type	= VarT;
	using	traits_type	= Traits;
	using	char_traits	= std::char_traits<char_type>;

	template<class Iterator>
	var_type parse(Iterator _begin, Iterator _end)
	{
		using	parser_type	= AMTRS_IMPLEMENTS(json_parser)::parser<Iterator, VarT, CharT, Traits>;
		Iterator	begin	= _begin;
		Iterator	end		= _end;
		return	parser_type::parse(begin, end);
	}


	template<class In>
	var_type parse(const In& _in)
	{
		auto	begin	= _in.begin();
		auto	end		= _in.end();
		return	parse(begin, end);
	}
};


template<class O>
inline void json_encode(O& _out, const typename json_parser::var_type& _v)
{
	if (auto* _bool = std::get_if<bool>(&_v); _bool)
	{
		_out << (*_bool ? "true" : "false");
	}

	if (auto* _null = std::get_if<std::nullptr_t>(&_v); _null)
	{
		_out << "null";
	}

	if (auto* _num = std::get_if<std::intmax_t>(&_v); _num)
	{
		_out << *_num;
	}

	if (auto* _num = std::get_if<double>(&_v); _num)
	{
		_out << *_num;
	}

	if (auto* _str = std::get_if<std::string>(&_v); _str)
	{
		_out << '"' << *_str << '"';
	}

	if (auto* _value = std::get_if<typename json_parser::var_type::array_type>(&_v); _value)
	{
		std::size_t	c	= 0;
		_out << "[ ";
		for (auto& i : *_value)
		{
			if (c > 0) { _out << ", "; }
			json_encode<O>(_out, i);
			++c;
		}
		_out << " ]";
	}

	if (auto* _value = std::get_if<typename json_parser::var_type::assoc_type>(&_v); _value)
	{
		std::size_t	c	= 0;
		_out << "{ ";
		for (auto& i : *_value)
		{
			if (c > 0) { _out << ", "; }
			_out << "\"" << i.first << "\" : ";
			json_encode<O>(_out, i.second);
			++c;
		}
		_out << " }";
	}
}


inline auto json_decode(std::string_view _data) -> typename json_parser::var_type
{
	using	char_type	= typename std::string_view::value_type;

	const char_type*	cursor	= _data.data();
	const char_type*	end		= cursor + _data.size();
	return	json_parser().parse(cursor, end);
}

AMTRS_NAMESPACE_END
#endif

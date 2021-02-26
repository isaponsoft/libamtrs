/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__passion__error__error__hpp
#define	__passion__error__error__hpp
#include "../token/token.hpp"
PASSION_NAMESPACE_BEGIN



// ============================================================================
//! コンパイラエラー
// ----------------------------------------------------------------------------
#define	PSNCMPLR_COMPILER_ERROR()	\
			throw program_error(error_code::compiler_error, {__FILE__, __LINE__}, __FILE__, __LINE__);


// ****************************************************************************
//!	エラーコード
// ****************************************************************************
#define	PSNEC_DEFINE(_code, _name, _msg)	_name = _code,
enum class error_code
{
#include "error_table.hpp"
};
#undef	PSNEC_DEFINE


inline const char* get_errorcode_name(error_code _ec)
{
	switch ((int)_ec)
	{
#define	PSNEC_DEFINE(_code, _name, _msg)	case (int)error_code::_name : return _msg;
#include "error_table.hpp"
#undef	PSNEC_DEFINE
	}
	return	"unkown error code";
}


//!	コンパイル時のエラー
class	compiler_error
		: public std::exception
{
public:
	using	line_type		= std::size_t;
	using	location_type	= location;

	compiler_error(error_code _ec, const location_type& _location, const std::string& _msg, const std::string& _filename, line_type _line)
		: m_error_code(_ec)
		, m_location(_location)
		, m_msg(_msg)
		, m_filename(_filename)
		, m_line(_line)
	{
#ifdef	PASSION_COMPILER_SHOW_ERRORPOINT
		m_msg	+= " in ";
		m_msg	+= _filename;
		m_msg	+= "(";
		m_msg	+= std::to_string(_line);
		m_msg	+= ")";
#endif
	}



	virtual const char* what() const noexcept
	{
		return	m_msg.c_str();
	}

	line_type line() const noexcept { return m_line; }

	const location_type& location() const noexcept
	{
		return	m_location;
	}

	error_code code() const noexcept
	{
		return	m_error_code;
	}
private:
	error_code		m_error_code;
	location_type	m_location;
	std::string		m_msg;
	std::string		m_filename;
	line_type		m_line;
};



//!	プログラムエラー
struct	program_error
		: compiler_error
{
	template<class... Args>
	program_error(error_code _ec, const class location& _location, Args... _args)
		: compiler_error(_ec, _location, "", std::forward<Args>(_args)...)
	{}
};


//!	構文エラー
struct	syntax_error
		: compiler_error
{
	template<class... Args>
	syntax_error(error_code _ec, const class location& _location, std::string&& _msg, const std::string& _filename, line_type _line)
		: compiler_error(_ec, _location, std::move(_msg), _filename, _line)
	{}

	template<class... Args>
	syntax_error(error_code _ec, const class location& _location, const Token& _token, Args... _args)
		: compiler_error(_ec, _location, (std::string)_token.str(), std::forward<Args>(_args)...)
	{}
};

//!	クラス解析時のエラー
struct	class_parse_error
		: syntax_error
{
	template<class... Args>
	class_parse_error(error_code _ec, const class location& _location, const Token& _token, Args... _args)
		: syntax_error(_ec, _location, _token, std::forward<Args>(_args)...)
	{}
};

//!	関数解析時のエラー
struct	function_parse_error
		: syntax_error
{
	template<class... Args>
	function_parse_error(error_code _ec, const class location& _location, const Token& _token, Args... _args)
		: syntax_error(_ec, _location, _token, std::forward<Args>(_args)...)
	{}
};


//!	仮引数のエラー
struct	parameter_list_error
		: syntax_error
{
	template<class... Args>
	parameter_list_error(error_code _ec, const class location& _location, const Token& _token, Args... _args)
		: syntax_error(_ec, _location, _token, std::forward<Args>(_args)...)
	{}
};


PASSION_NAMESPACE_END
#endif

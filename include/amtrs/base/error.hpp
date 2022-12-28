/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__base__error__hpp
#define	__libamtrs__base__error__hpp
#include "namespace.hpp"
#include "platform.hpp"
AMTRS_NAMESPACE_BEGIN
template<class T>
struct	errorval_type
{
	size_t operator () () { return -1; }
};

struct	errorval
{
	using	fn_errormsg	= char const*(int);

	errorval() : _en(0), _type(0) {}
	errorval(errorval const&) = default;
	errorval& operator = (errorval const&) = default;

	bool good() const noexcept { return _en == 0 ? true : false; }
	bool bad() const noexcept  { return !good(); }
	size_t type() const noexcept { return _type; }
	int code() const noexcept { return _en; }

	explicit errorval(int e)		// errno
		: _type(0)
		, _en(e)
	{}

	explicit errorval(size_t _type_index_hash, int _errno)
		: _type(_type_index_hash)
		, _en(_errno)
	{}

	template<class ErrorType>
	errorval(ErrorType _err)
		: _type(errorval_type<ErrorType>{}())
		, _en((int)_err)
	{}

	template<class ErrorType>
	bool is(ErrorType _err) const noexcept
	{
		return	(_type == errorval_type<ErrorType>{}()) && (_en == (int)_err);
	}

	template<class ErrorType>
	bool operator == (ErrorType _err) const noexcept
	{
		return	is<ErrorType>(_err);
	}

	template<class ErrorType>
	bool operator != (ErrorType _err) const noexcept
	{
		return	!is<ErrorType>(_err);
	}

	AMTRS_API int to_errno() const noexcept;


	AMTRS_API char const* message() const noexcept;
	AMTRS_API static void add_error_type(size_t _type_index_hash, fn_errormsg* _msg);

private:
	AMTRS_API char const* _message() const noexcept;
	size_t		_type	= 0;
	int			_en		= 0;
};

/*
 * errorval とセットで値を返します。
 */
template<class V>
struct	result : V
{
	using	value_type	= V;

	errorval	err;

	bool good() const noexcept { return err.good(); }
	bool bad() const noexcept { return err.bad(); }

	V& val() noexcept { return *this; }
	V const& val() const noexcept { return *this; }

	result(errorval e) : err(std::move(e)) , V() {}
	template<class... Args>
	result(errorval e, Args&&... _args) : err(std::move(e)) , V(std::forward<Args>(_args)...) {}
	result(V v) : err(0) , V(std::move(v)) {}

	result() = default;
	result(result const&) = default;
	result(result&&) = default;
	result& operator = (result const&) = default;
	result& operator = (result&&) = default;

	V& value() noexcept { return *this; }
	V const& value() const noexcept { return *this; }
};

AMTRS_NAMESPACE_END
#endif

/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__inarikvs__redis_protocol__hpp
#define	__inarikvs__redis_protocol__hpp
#include <amtrs/string/@>
#include <amtrs/variant/@>
#include "inarikvs-def.hpp"
#include "inarikvs-types.hpp"
INARIKVS_NAMESPACE_BEGIN

struct	redis_protocol
{
	struct	simple_string;
	struct	errors;
	struct	integers;
	struct	bulk_string;

	using	value_type	= amtrs::variantobj<simple_string, errors, integers, bulk_string>;

	redis_protocol(value_type* _value)
		: mValue(_value)
	{}

	value_type*	mValue;

	struct	stringable
	{
		std::string_view	string;

		template<class It>
		int operator () (It _cursor, It _end)
		{
			auto	begin	= _cursor;
			while (_cursor != _end)
			{
				if (is_crlf(_cursor, _end))
				{
					string	= std::string_view(&*begin, std::distance(begin, _cursor));
					return	_cursor - begin + 2;	// + \r\n
				}
				++_cursor;
			}
			return	0;
		}
	};


	struct	simple_string : stringable
	{
		using	stringable::operator ();
	};

	struct	errors : stringable
	{
		using	stringable::operator ();
	};

	struct	integers : stringable
	{
		int64_t				value		= 0;
		template<class It>
		int operator () (It _cursor, It _end)
		{
			int	r	= stringable::operator () (_cursor, _end);
			if (r)
			{
				auto		cur		= stringable::string.begin();
				auto		end		= stringable::string.end();
				bool		minus	= false;
				if (*cur == '-')
				{
					minus	= true;
					++cur;
				}
				while (cur != end)
				{
					value = value * 10 + (*cur++ - '0');
				}
				if (minus)
				{
					value	= -value;
				}
			}
			return	r;
		}
	};


	struct	bulk_string
	{
		std::string_view	string;

		template<class It>
		int operator () (It _cursor, It _end)
		{
			integers	num;
			int			r		= num(_cursor, _end);
			auto		cursor	= _cursor + r;
			if ((_end - _cursor) < (num.value + 2))
			{
				return	0;
			}
			if (!is_crlf(cursor + num.value, _end))
			{
				return	0;
			}
			string	= std::string_view(&*cursor, num.value);
			return	r + num.value + 2;
		}
	};



	struct	arrays
	{
		value_type::array_type	ary;

		template<class It>
		int operator () (It _cursor, It _end)
		{
			integers				num;
			int						r		= num(_cursor, _end);
			auto					cursor	= _cursor + r;
			ary	= value_type::array_type((std::size_t)num.value);	
			for (int i = 0; i < num.value; ++i)
			{
				value_type		v;
				redis_protocol	rp(&v);
				r	= rp(cursor, _end);
				if (!r)
				{
					return	r;
				}
				ary[i]	= std::move(v);
				cursor += r;
			}
			return	cursor - _cursor;
		}
	};


	value_type	value;


	template<class It>
	static constexpr bool is_crlf(It _cursor, It _end)
	{
		return	 ((_cursor + 2 <= _end) && (_cursor[0] == '\r') && (_cursor[1] == '\n')) ? true : false;
		
	}


	template<class It>
	int operator () (It _cursor, It _end)
	{
		if (_cursor == _end)
		{
			return	0;
		}

		if (*_cursor == '+')
		{
			// simple string
			simple_string	ss;
			auto			r	= ss(_cursor+1, _end);
			if (r)
			{
				*mValue	= std::move(ss);
			}
			return	(r) ? r + 1 : r;
		}
		else if (*_cursor == '-')
		{
			// errors
			errors			ss;
			auto			r	= ss(_cursor+1, _end);
			if (r)
			{
				*mValue	= std::move(ss);
			}
			return	(r) ? r + 1 : r;
		}
		else if (*_cursor == ':')
		{
			// integers
			integers			ss;
			auto			r	= ss(_cursor+1, _end);
			if (r)
			{
				*mValue	= std::move(ss);
			}
			return	(r) ? r + 1 : r;
		}
		else if (*_cursor == '$')
		{
			// bulk string
			bulk_string			ss;
			auto			r	= ss(_cursor+1, _end);
			if (r)
			{
				*mValue	= std::move(ss);
			}
			return	(r) ? r + 1 : r;
		}
		else if (*_cursor == '*')
		{
			// array
			arrays				ss;
			auto			r	= ss(_cursor+1, _end);
			if (r)
			{
				*mValue	= std::move(ss.ary);
			}
			return	(r) ? r + 1 : r;
		}
		else
		{
			return	1;
		}
	}
};

INARIKVS_NAMESPACE_END
#endif

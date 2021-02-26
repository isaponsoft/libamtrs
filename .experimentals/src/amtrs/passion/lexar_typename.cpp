/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <passion/lexar.hpp>
#include <passion/lexar_rules.hpp>
PASSION_NAMESPACE_BEGIN


vmstring read_typename(int& _error, lexar& _lexar)
{
	lexar::cursor	backup(_lexar.position());
	lexar::cursor	next;
	vmstring		retval;
	_error	= 0;

	if (auto s = _lexar.peek_string<keyword_rules>(next); !s.empty())
	{
		if (s == "typename")
		{
			_lexar.position(next);
		}
	}

	// "::"
	if (auto s = _lexar.read_string<namespace_separator_rules>(); !s.empty())
	{
		retval	= vmstring(s);
	}


	for (;;)
	{
		if (auto s = _lexar.read_string<keyword_rules>(); !s.empty())
		{
			retval	+= vmstring(s);
		}
		else
		{
			_lexar.position(backup);
			_error	= 1;
			return	{};
		}

		// template parameter
		if (auto cr = _lexar.peek_compare(next, "<"); !cr.empty())
		{
			retval	+= vmstring("<");
			_lexar.position(next);

			if (cr = _lexar.peek_compare(">"); cr.empty())
			{
				vmstring	intype	= read_typename(_error, _lexar);
				if (_error)
				{
					_lexar.position(backup);
					_error	= 1;
					return	{};
				}
				retval	+= intype;
			}
			if (cr = _lexar.peek_compare(next, ">"); cr.empty())
			{
				_lexar.position(backup);
				_error	= 1;
				return	{};
			}
			retval	+= vmstring(">");
			_lexar.position(next);
		}


		if (auto s = _lexar.peek_string<namespace_separator_rules>(next); s.empty())
		{
			break;
		}
		else
		{
			retval += vmstring(s);
			_lexar.position(next);
		}
	}

	return	retval;
}


PASSION_NAMESPACE_END

/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <passion/lexar_rules.hpp>
#include <passion/syntax.hpp>
using namespace amtrs;
using namespace PASSION_NAMESPACE;




struct_node::result struct_node::parse(int _phase, stntaxparser& _parser)
{
	enum	phase_type
	{
		checkextends	= 100,
		baseclass		= 101,
		open			= 102,
		declaration		= 103,
		close			= 104,
	};


	switch (_phase)
	{
		case start :
		{
			return	{checkextends, new typenamepath_node()};
		}

		case checkextends :
		{
			mTypedata	= vmtypedata(vmtypedata::classes, static_cast<typenamepath_node*>(_parser.prev.get())->name());
			if (auto r = _parser.lex->peek_compare(":"); !r.empty())
			{
				// ベースクラスを読み取る
				return	{baseclass, this};
			}
			return	{open, this};
		}

		case baseclass :
		{
			return	{open, this};
		}

		case open :
		{
			_parser.typeList.add(mTypedata);
/*
			vmtoken	tkn	= _parser.lex->read_token<vmtoken, bracket_rules>();
			if (tkn.string() != "{")
			{
				return	{err, nullptr};
			}

			return	{declaration, child};
*/
			return	{declaration, this};
		}

		case declaration :
		{
			vmtoken	tkn	= _parser.lex->peek_token<vmtoken, keyword_rules>();
			if (!tkn.empty())
			{
				return	{err, nullptr};
			}

			


			return	{err, nullptr};
		}

		case close :
		{
			vmtoken	tkn	= _parser.lex->read_token<vmtoken, bracket_rules>();
			if (tkn.string() != "}")
			{
				return	{err, nullptr};
			}

			tkn	= _parser.lex->read_token<vmtoken, lineterminate_rules>();
			if (tkn.string() != ";")
			{
				return	{err, nullptr};
			}

			return	{end, nullptr};
		}

		default:
			break;
	}


	return	{super_type::err, this};
}

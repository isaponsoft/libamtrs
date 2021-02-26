/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <iostream>
#include <passion/lexar_rules.hpp>
#include <passion/syntax.hpp>
using namespace amtrs;
using namespace PASSION_NAMESPACE;


declaration_node::result declaration_node::parse(int _phase, stntaxparser& _parser)
{
	vmtoken	tkn	= _parser.lex->read_token<vmtoken, keyword_rules, bracket_rules>();
	if (tkn.empty())
	{
		return	{super_type::err, nullptr};
	}

	if (tkn.string() == "namespace")
	{
		amtrs::ref<syntax_node>	child	= new namespace_block_node();
		mChilds.push_back(child);
		return	{proc, child};
	}

	if (tkn.string() == "struct")
	{
		amtrs::ref<syntax_node>	child	= new struct_node();
		mChilds.push_back(child);
		return	{proc, child};
	}

	_parser.lex->rollback();
	return	{end, nullptr};
}



namespace_block_node::result namespace_block_node::parse(int _phase, stntaxparser& _parser)
{
	enum	phase_type
	{
		open	= 100,
		close	= 101
	};

	switch (_phase)
	{
		case start :
		{
			mTypename	= new typenamepath_node();
			return	{open, mTypename};
		}

		case open :
		{
			vmtoken	tkn	= _parser.lex->read_token<vmtoken, bracket_rules>();
			if (tkn.string() != "{")
			{
				return	{err, nullptr};
			}

			ref<syntax_node>	child	= new declaration_node();
			mChilds.push_back(child);
			return	{close, child};
		}

		case close :
		{
			vmtoken	tkn	= _parser.lex->read_token<vmtoken, bracket_rules>();
			if (tkn.string() != "}")
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




typenamepath_node::result typenamepath_node::parse(int _phase, stntaxparser& _parser)
{
	switch (_phase)
	{
		case start :
		{
			vmtoken	tkn	= _parser.lex->read_token<vmtoken, keyword_rules>();
			if (tkn.empty())
			{
				return	{end, nullptr};
			}
			mName	+= tkn.string();
			return	{separator, this};
		}

		case separator :
		{
			vmtoken	tkn	= _parser.lex->read_token<vmtoken, namespace_separator_rules>();
			if (tkn.empty())
			{
				return	{end, nullptr};
			}
			mName	+= vmstring("::");
			return	{start, this};
		}

		default:
			break;
	}

	return	{super_type::end, this};
}

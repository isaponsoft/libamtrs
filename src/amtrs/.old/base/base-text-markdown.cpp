/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/amtrs.hpp>
#include <amtrs/text-commonmark.hpp>
#include <cstdlib>
#include <iostream>
#include <string_view>
#define	CMARK_GFM_STATIC_DEFINE				1
#define	CMARK_GFM_EXTENSIONS_STATIC_DEFINE	1
#if		__has_include(<cmark/cmarklib.h>)
#include <cmark/cmarklib.h>
#define	AMTRS_CMARK_USE	1
#endif

#if		AMTRS_CMARK_USE
AMTRS_NAMESPACE_BEGIN

struct	parsestatus
{
	bool	in_header	= false;
	int		line_number	= -1;
};


static bool parsing(parsestatus& _status, commonmark::node& nd, cmark_strbuf& buff, cmark_node* n);



template<>
void basic_commonmark<char>::parse(view_type _source, callback_func _callback, void* _userparam)
{
	cmark_mem*	mem		= cmark_get_default_mem_allocator();
	auto*		parser	= cmark_parser_ex_new(CMARK_OPT_TABLE_PREFER_STYLE_ATTRIBUTES);

	// parse
	cmark_parser_feed(parser, _source.data(), _source.size());
	auto					doc		= cmark_parser_finish(parser);
	cmark_iter*				iter	= cmark_iter_new(doc);
	cmark_event_type		ev;
	parsestatus				status;
	while ((ev = cmark_iter_next(iter)) != CMARK_EVENT_DONE)
	{
		node			nd
		{
			.type		= nodetype::none,
			.entering	= (ev == CMARK_EVENT_ENTER),
		};

		cmark_node*		n		= cmark_iter_get_node(iter);
		cmark_strbuf	buff;
		cmark_strbuf_init(mem, &buff, 0);
		parsing(status, nd, buff, n);
		if (nd.type != nodetype::none)
		{
			_callback(nd, _userparam);
		}
		cmark_strbuf_free(&buff);
	}
	cmark_iter_free(iter);
	cmark_node_free(doc);
}




bool parsing(parsestatus& _status, commonmark::node& nd, cmark_strbuf& buff, cmark_node* n)
{
	using	view_type	= commonmark::view_type;
	using	nodetype	= commonmark::nodetype;

	switch (n->type)
	{
		case CMARK_NODE_BLOCK_QUOTE :
		{
			nd.type		= nodetype::blockquote;
			break;
		}

		case CMARK_NODE_LIST :
		{
			nd.type			= nodetype::list;
			nd.list.bullet	= (cmark_list_type)n->as.list.list_type == CMARK_BULLET_LIST;
			nd.list.start	= n->as.list.start;
			break;
		}
		
		case CMARK_NODE_ITEM :
		{
			nd.type			= nodetype::item;
			break;
		}
		
		case CMARK_NODE_HEADING :
		{
			nd.type				= nodetype::heading;
			nd.heading.level	= (int)n->as.heading.level;
			break;
		}

		case CMARK_NODE_CODE_BLOCK :
		{
			nd.type					= nodetype::codeblock;

			// language
			size_t		langsize	= 0;
			if (n->as.code.info.len > 0)
			{
				int	size = 0;
				while (size < n->as.code.info.len && !cmark_isspace(n->as.code.info.data[size]))
				{
					++size;
				}
				houdini_escape_html0(&buff, n->as.code.info.data, size, 0);
				langsize	= size;
			}

			// text
			houdini_escape_html0(&buff, n->as.code.literal.data, n->as.code.literal.len, 0);

			nd.codeblock.language	= view_type((char const*)buff.ptr           , langsize);
			nd.codeblock.text		= view_type((char const*)buff.ptr + langsize, buff.size - langsize);
			break;
		}

		case CMARK_NODE_HTML_BLOCK :
		{
			nd.type					= nodetype::htmlblock;
			nd.htmlblock.text		= view_type((char const*)n->as.literal.data, n->as.literal.len);
			break;
		}

		case CMARK_NODE_CUSTOM_BLOCK :
		{
			nd.type					= nodetype::rawblock;
			if (nd.entering)
			{
				nd.rawblock.text		= view_type((char const*)n->as.custom.on_enter.data, n->as.custom.on_enter.len);
			}
			else
			{
				nd.rawblock.text		= view_type((char const*)n->as.custom.on_exit.data, n->as.custom.on_exit.len);
			}
			break;
		}
		
		case CMARK_NODE_THEMATIC_BREAK :
		{
			nd.type					= nodetype::thematic_break;
			break;
		}

		case CMARK_NODE_PARAGRAPH :
		{
			cmark_node*	grandparent	= cmark_node_parent(cmark_node_parent(n));
			if (!(grandparent && grandparent->type == CMARK_NODE_LIST && grandparent->as.list.tight))
			{
				nd.type	= nodetype::paragraph;
			}
			break;
		}
		
		case CMARK_NODE_TEXT :
		{
			nd.type					= nodetype::text;
			houdini_escape_html0(&buff, n->as.literal.data, n->as.literal.len, 0);
			nd.text.text			= view_type((char const*)buff.ptr, buff.size);
			break;
		}

		case CMARK_NODE_LINEBREAK :
		{
			nd.type					= nodetype::linebreak;
			break;
		}

		case CMARK_NODE_SOFTBREAK :
		{
			nd.type					= nodetype::softbreak;
			break;
		}

		case CMARK_NODE_CODE :
		{
			nd.type					= nodetype::code;
			houdini_escape_html0(&buff, n->as.literal.data, n->as.literal.len, 0);
			nd.code.text			= view_type((char const*)buff.ptr, buff.size);
			break;
		}

		case CMARK_NODE_HTML_INLINE :
		{
			nd.type					= nodetype::htmlinline;
			nd.htmlinline.text		= view_type((char const*)n->as.literal.data, n->as.literal.len);
			break;
		}
		
		case CMARK_NODE_CUSTOM_INLINE :
		{
			auto&	block = nd.entering ? n->as.custom.on_enter : n->as.custom.on_exit;
			nd.type					= nodetype::custominline;
			nd.htmlinline.text		= view_type((char const*)block.data, block.len);
			break;
		}
		
		case CMARK_NODE_STRONG :
		{
			nd.type					= nodetype::strong;
			break;
		}
		
		case CMARK_NODE_EMPH :
		{
			nd.type					= nodetype::emph;
			break;
		}

		case CMARK_NODE_LINK :
		{
			nd.type					= nodetype::link;
			if (nd.entering)
			{

				size_t		urlsize	= 0;
				if (n->as.link.url.len)
				{
					cmark_strbuf_put(&buff, n->as.link.url.data, n->as.link.url.len);
					urlsize	= buff.size;
				}
				if (n->as.link.title.len)
				{
					houdini_escape_html0(&buff, n->as.link.title.data, n->as.link.title.len, 0);
				}
				nd.link.url		= view_type((char const*)buff.ptr, urlsize);
				nd.link.title	= view_type((char const*)buff.ptr + urlsize, buff.size - urlsize);
			}
			break;
		}

		case CMARK_NODE_IMAGE :
		{
			nd.type					= nodetype::image;
			if (nd.entering)
			{
				if (n->as.link.url.len)
				{
					cmark_strbuf_put(&buff, n->as.link.url.data, n->as.link.url.len);
				}
				nd.image.url		= view_type((char const*)buff.ptr, buff.size);
			}
			else
			{
				if (n->as.link.title.len)
				{
					houdini_escape_html0(&buff, n->as.link.title.data, n->as.link.title.len, 0);
				}
				nd.image.title		= view_type((char const*)buff.ptr, buff.size);
			}
			break;
		}

		case CMARK_NODE_TABLE :
		{
			nd.type				= nodetype::table;
			_status.in_header	= false;
			_status.line_number	= -1;
			break;
		}

		case CMARK_NODE_TABLE_ROW :
		{
			_status.in_header	= ((cmark_table_row*)n->as.opaque)->is_header ? true : false;
			if (!_status.in_header && !nd.entering)
			{
				_status.line_number	= -1;
			}
			nd.type						= nodetype::table_row;
			nd.table_row.header			= _status.in_header;
			nd.table_row.line_number	= _status.line_number;
			if (!_status.in_header && !nd.entering)
			{
				++_status.line_number;
			}
			break;
		}

		case CMARK_NODE_TABLE_CELL :
		{
			uint8_t*	alignments	= ((cmark_table*) n->parent->parent->as.opaque)->alignments;
			int			colidx		= 0;
			for (auto* c = n->parent->first_child; c; c = c->next, ++colidx)
			{
				if (c == n)
				{
					break;
				}
			}
			nd.type						= nodetype::table_cell;
			nd.table_cell.header		= _status.in_header;
			nd.table_cell.line_number	= _status.line_number;
			nd.table_cell.col_number	= colidx;
			nd.table_cell.alignments	= alignments[colidx];
			break;
		}

		default :
		{
			break;
		}
	}
	return	true;
}

AMTRS_NAMESPACE_END
#endif
